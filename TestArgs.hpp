/*

2 useful functions: get_inputs and run_test


get_inputs<...>(...) sets up easy declaration of:
1. function inputs
2. input descriptions
3. command-line input processing with nice error messages
4. default inputs
5. test cases


// returns vector<tuple<InputTypes...> >
auto inputs = get_inputs<InputTypes...>
(argc,
 argv,
 {default_inputs},
 {"description of input field 1","description of input field 2",...}
 {{test_input_1...},{test_input_2...},...}
)

In both versions, the descriptions and test cases are optional.
The descriptions list can have a different length than the number of arguments.



*/

#pragma once
#ifndef TEST_ARGS_HPP
#define TEST_ARGS_HPP



#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "AssertUtils.hpp"
#include "StringUtils.hpp"
#include "TupleUtils.hpp"
#include "TypeUtils.hpp"

namespace {

    // Mimics a vector of strings,
    // but can accept a vector of anything coercible to a string
    struct Names_Array : std::vector<std::string>
    {
	Names_Array(std::initializer_list<std::string> && l) :
	    std::vector<std::string>(std::move(l))
	    {}
	Names_Array(std::initializer_list<std::string> const & l) :
	    std::vector<std::string>(l)
	    {}
	Names_Array(std::vector<std::string> && input) :
	    std::vector<std::string>(std::move(input))
	    {}
        template<class T>
	Names_Array(std::vector<T> const & input) :
	    std::vector<std::string>(input.size())
	    {
		for (std::size_t index = 0; index < input.size(); ++index)
		    (*this)[index] = std::string(input[index]);
	    }
	template<class T>
	Names_Array(T const & t) :
	    std::vector<std::string>{std::string(t)}
	    {}
    };

    // Argument handling:

    // Single argument unpacking template
    // Default = consume a single field
    // Must define the following functions:
    // 1. T std_string_to<T>(std::string const &, size_t *)
    // 2. std::string type_name<T>()
    // 3. std::string input_format<T>()
    template<class T>
    struct Unpack_Arg
    {
	// unpacks arguments into the specified T
	// returns how many arguments have been used so far
	static inline std::size_t
	unpack_arg(std::size_t const arg_st_idx,
		   std::size_t const num_args,
		   char const * const * const arg_start,
		   T & unpack_to)
	    {
		if (arg_st_idx >= num_args)
		{
		    std::cout << "Error: reached end of argument list but expected "<< type_name<T>() << std::endl;
		    return -1;
		}

		std::string arg(arg_start[arg_st_idx]);
	        bool ok = false;
		try
		{
		    unpack_to = std_string_to<T>(arg,ok);
		}
		catch (...)
		{
		    ok = false;
		}
		if (!ok)
		{
		    std::cout << "Error parsing " << type_name<T>() << ": " << arg << std::endl;
		    return -1;
		}
		return arg_st_idx+1;
	    }
	static inline std::string
	to_str(bool const print_brackets = true)
	    {
		std::string result = input_format<T>();
		return print_brackets ? "{" + result + "}" : result;
	    }
    };

    // Multiple argument unpacking template:
    // template propogation works from the back to the front

    template<std::size_t idx, class... Types>
    struct Unpack_Args
    {
	using element_type = typename std::tuple_element<idx-1,no_cvref_tuple<Types...> >::type;
	
	// unpacks arguments into the specified tuple of inputs
	// returns how many arguments have been used so far
	static inline std::size_t
	unpack_args(std::size_t const num_args,
		    char const * const * const arg_start,
		    no_cvref_tuple<Types...> & tuple)
	    {
		std::size_t arg_st_idx = Unpack_Args<idx-1,Types...>::unpack_args(num_args,arg_start,tuple);

		if (arg_st_idx != -1)
		    return Unpack_Arg<element_type>::unpack_arg(arg_st_idx,num_args,arg_start,std::get<idx-1>(tuple));
		return arg_st_idx;
	    }
	static inline std::string
	to_str(Names_Array const & arg_names)
	    {
		std::string before = Unpack_Args<idx-1,Types...>::to_str(arg_names);
		if (idx > 1)
		    before += " ";
		bool const use_arg_name =
		    arg_names.size() >= idx &&
		    arg_names[idx-1] != std::string("");
		if (use_arg_name)
		{
		    before += "{";
		    before += arg_names[idx-1];
		    before += ":";
		}
		// don't print the brackets around the type if there's an arg name
		before +=  Unpack_Arg<element_type>::to_str(!use_arg_name);
		if (use_arg_name)
		    before += "}";
		return before;
	    }
    };

    // Multiple argument unpacking template specialization: beginning of tuple
    // template propogation works from the back to the front, so this is the base case

    template<class... Types>
    struct Unpack_Args<0,Types...>
    {
	// unpacks arguments into the specified tuple of inputs
	// returns how many arguments have been used so far
	static inline std::size_t
	unpack_args(std::size_t const num_args,
		    char const * const * arg_start,
		    no_cvref_tuple<Types...> & tuple)
	    {
		return 0;
	    }
	static inline std::string
	to_str(Names_Array const &)
	    {
		return std::string();
	    }
    };
}



// get_inputs functions, for external use

// inputs are given as tuples (multiple fields)
// takes the user input (command-line as argv)
// returns a list of tuples of fields to run

template<class... Input_Types>
std::vector<no_cvref_tuple<Input_Types...> >
get_inputs(int const argc,
	   char const * const * const argv,
	   no_cvref_tuple<Input_Types...> const & challenge_args,
	   Names_Array const & arg_names = {},
	   std::vector<no_cvref_tuple<Input_Types...> > const tests = {})
{   
    using test_index_type = unsigned int;

    if (arg_names.size() > sizeof...(Input_Types))
	std::cout << "Warning: argument names list is longer than the number of arguments" << std::endl;
    
    // no arguments: use the challenge arguments
    if (argc <= 1)
	return {challenge_args};
    
    // special case: test x
    if (argc == 3 && std::string("test") == std::string(argv[1]))
    {
	std::string arg2(argv[2]);
	
	if (arg2 == std::string("all"))
	    return tests;
	
	bool ok = false;
	long test_index;
	try
	{
	    test_index = std_string_to<test_index_type>(arg2,ok);
	}
	catch (...)
	{
	    ok = false;
	    test_index = 0;
	}
	if (!ok)
	{
	    // didn't use the entire string as the number, error out
	    std::cout << "Invalid argument for test index (" << type_name<test_index_type>() << "):" << std::endl;
	    std::cout << arg2 << std::endl;
	    return {};
	}
	
	if (test_index <= 0 || test_index > tests.size())
	{
	    // invalid index, error out
	    std::cout << "Invalid test index " << test_index << std::endl;
	    std::cout << "Expecting an index in the range ";
	    std::cout << "[1," << tests.size() << "]" << std::endl;
	    return {};
	}
	
	return {tests[test_index-1]};
    }

    constexpr size_t num_inputs = sizeof...(Input_Types);

    // regular case: unpack arguments
    no_cvref_tuple<Input_Types...> inputs;
    std::size_t num_args_unpacked = Unpack_Args<num_inputs,Input_Types...>::unpack_args(argc-1,argv+1,inputs);

    if (num_args_unpacked < argc-1)
    {
	std::cout << "Error: extra arguments provided:";
	for (std::size_t index = num_args_unpacked; index < argc-1; ++index)
	    std::cout << " " << argv[1+index];
	std::cout << std::endl;
    }
    
    if (num_args_unpacked != argc-1)
    {
	std::cout << "1. See all valid argument formats contained within [] following these instructions." << std::endl;
	std::cout << "2. When entering arguments, remove the outermost []." << std::endl;
	std::cout << "3. {name:type} or {type} represent a data type." << std::endl;
	std::cout << "4. [x...] represents a comma-separated list of x." << std::endl;
	std::cout << "[]" << std::endl;
	if (tests.size() > 0)
	{
	    std::cout << "[test all]" << std::endl;
	    std::cout << "[test {x:" << type_name<test_index_type>() << "}] ";
	    std::cout << "where x is the range [1," << tests.size() << "], ";
	    std::cout << "for example: [test 1]" << std::endl;
	}
	if (num_inputs > 0)
	    std::cout << "[" << Unpack_Args<num_inputs,Input_Types...>::to_str(arg_names) << "]" << std::endl;
	return {};
    }
    
    return {inputs};
}

declare_enable_if(run_test_enable,
		  "Any function passed to run_test must "
		  "take each input by value or by const &")

// combine get_inputs and apply_function to make a complete test case runner
// also takes an optional input validation function pointer

template<class Output_Type, class... Input_Types>
typename use_enable_if(run_test_enable,
		       my_all<is_value_or_const_lref<Input_Types>()...>(),
		       bool)
run_test(int const argc,
	 char const * const * const argv,
	 no_cvref_tuple<Input_Types...> const & challenge_args,
	 function_type<Output_Type,Input_Types...> function,
	 char const * const function_name,
	 Names_Array const & arg_names = {},
	 std::vector<no_cvref_tuple<Input_Types...> > const & tests = {},
	 function_type<bool,Input_Types...> valid_input = nullptr)
{   
    auto inputs_list = get_inputs<my_remove_cvref<Input_Types>...>
	(argc,argv,challenge_args,arg_names,tests);
    
    bool ran_ok = inputs_list.size() > 0;
    
    for (auto & inputs : inputs_list)
    {
	std::string inputs_string = apply_function(to_std_string<my_remove_cvref<Input_Types>...>,inputs);
	if (valid_input != nullptr && !apply_function(valid_input,inputs))
	{
	    ran_ok = false;
	    std::cout << "Invalid inputs: " << inputs_string << std::endl;
	}
	else
	{
	    Output_Type output = apply_function(function,inputs);
	    std::cout << function_name << "(" << inputs_string << ") = " << to_std_string(output) << std::endl;
	}
    }

    return ran_ok;
}


#endif /* TEST_ARGS_HPP */
