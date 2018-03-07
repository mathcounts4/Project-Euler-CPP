#pragma once
#ifndef STRING_UTILS
#define STRING_UTILS

/*

Defines template functions for converting to and from strings
std_string_to<T> and to_std_string<T>

*/

#include <cstdlib>
#include <exception>
#include <limits>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "AssertUtils.hpp"
#include "MacroUtils.hpp"
#include "TupleUtils.hpp"

/* std_string_to: */

/* strto_ functions not defined in <cstdlib> */
int strtoi(char const * str,
	   char** endptr,
	   int base)
{
    long val = strtol(str,endptr,base);
    if (val > std::numeric_limits<int>::max() ||
	val < std::numeric_limits<int>::min())
    {
	val = 0;
	if (endptr != nullptr)
	    *endptr = nullptr;
    }
    return val;
}

unsigned int strtoui(char const * str,
		     char** endptr,
		     int base)
{
    unsigned long val = strtoul(str,endptr,base);
    if (val > std::numeric_limits<unsigned int>::max())
    {
	val = 0;
	if (endptr != nullptr)
	    *endptr = nullptr;
    }
    return val;
}

// to_std_string behind-the-scenes
// we need to use structs and not functions
// since we use a general template with partial specializations

namespace
{
    // general template
    template<class T>
    struct Std_String_To_Helper
    {
	static inline T
	convert(char const *,
		char**)
	    {
		error_user_define(T,Std_String_To_Helper,convert(char const *,char**));
	    }
    };
    
    /* macro since the body is the same except for the type and conversion function call */
#define std_string_to_decl(type,fcncall)	\
    template<>					\
    struct Std_String_To_Helper<type>		\
    {						\
	static inline type			\
	    convert(char const * str,		\
		    char** endptr)		\
	{					\
	    return fcncall;			\
	}					\
    };

    /* integer types */
    std_string_to_decl(int,strtoi(str,endptr,0))
    std_string_to_decl(unsigned int,strtoui(str,endptr,0))
    std_string_to_decl(long,strtol(str,endptr,0))
    std_string_to_decl(unsigned long,strtoul(str,endptr,0))
    std_string_to_decl(long long,strtoll(str,endptr,0))
    std_string_to_decl(unsigned long long,strtoull(str,endptr,0))
    
    /* floating point types */
    std_string_to_decl(float,strtof(str,endptr))
    std_string_to_decl(double,strtod(str,endptr))
    std_string_to_decl(long double,strtold(str,endptr))
    
#undef std_string_to_decl

    /* vector */
    template<class T>
    struct Std_String_To_Helper<std::vector<T> >
    {
	static inline std::vector<T>
        convert(char const * str,
	        char** endptr)
	    {
		char* temp = nullptr;
		if (endptr == nullptr)
		    endptr = &temp;
		
		// check for valid start
		if (str == nullptr || str[0] != '[')
		{
		    *endptr = nullptr;
		    return {};
		}
		
		// special case for empty list
		if (str != nullptr && str[0] == '[' && str[1] == ']')
		{
		    *endptr = const_cast<char *>(&str[2]);
		    return {};
		}
		
		// parse (a character then an item) in a loop
		std::vector<T> result;
		char match = '[';
		while (true)
		{
		    // check for end
		    if (str != nullptr && str[0] == ']')
		    {
			*endptr = const_cast<char *>(&str[1]);
			return result;
		    }

		    // check for mismatch (start of list or comma)
		    if (str == nullptr || str[0] != match)
		    {
			*endptr = nullptr;
			return {};
		    }
		    // after first loop, desired character is a comma
		    match = ',';

		    // add an item to the list, starting with the next character
		    char * end = nullptr;
		    result.push_back(Std_String_To_Helper<T>::convert(&str[1],&end));
		    str = end;
		}
	    }
    };
}

// if endptr is not nullptr,
// sets *endptr to nullptr (failure) or the char after T ends in str
template<class T> inline T
std_string_to(char const * str,
	      char** endptr = nullptr)
{
    return Std_String_To_Helper<T>::convert(str,endptr);
}

// pass = if it matched the entire string
template<class T> inline T
std_string_to(std::string const & str,
	      bool & pass)
{
    char* c = nullptr;
    T t = std_string_to<T>(&str[0],&c);
    pass = c == &*str.end();
    return t;
}

// plain string version - don't care if it passed
template<class T> inline T
std_string_to(std::string const & str)
{
    bool throwaway = false;
    return std_string_to<T>(str,throwaway);
}





// useful method
inline std::string string_concat(std::string delimeter,
				 std::vector<std::string> args)
{
    std::string result;
    if (args.size() > 0)
	result += args[0];
    for (std::size_t index = 1; index < args.size(); ++index)
    {
	result += delimeter;
	result += args[index];
    }
    return result;
}

// to_std_string behind-the-scenes
// we need to use structs and not functions
// since we use a general template with partial specializations

namespace
{
    // general template
    template<class... Types>
    struct to_string_helper
    {
	static inline std::string
	to_string(Types const & ... inputs)
	    {
		return string_concat(",",{to_string_helper<Types>::to_string(inputs)...});
	    }
    };

    // empty
    template<>
    struct to_string_helper<>
    {
	static inline std::string
	to_string()
	    {
		return "";
	    }
    };

#define basic_to_string_decl(type)		\
    template<> struct to_string_helper<type> {	\
	static inline std::string		\
	to_string(type const & x)			\
	    { return std::to_string(x); }};
    
    for_each_primitive_type(basic_to_string_decl)
    
#undef basic_to_string_decl
    
    // for individual arguments: error out in the general case
    template<class T>
    struct to_string_helper<T>
    {
	static inline std::string
	to_string(T const & x)
	    {
		error_user_define(T,to_string_helper,to_string(T const &));
		return "";
	    }
    };

    // std::string itself:
    template<>
    struct to_string_helper<std::string>
    {
	static inline std::string
	to_string(std::string const & x)
	    { return x; }
    };

    // partial specialization for std::vector
    template<class T>
    struct to_string_helper<std::vector<T> >
    {
	static inline std::string
	to_string(std::vector<T> const & v)
	    {
		std::size_t max_printed = 10;
		std::string result;
		result += "[";
		std::size_t sz = v.size();
		if (sz > 0)
		    result += to_string_helper<T>::to_string(v[0]);
		for (std::size_t index = 1; index < sz && index < max_printed; ++index)
		{
		    result += ",";
		    result += to_string_helper<T>::to_string(v[index]);
		}
		if (sz > max_printed)
		    result += ",...";
		result += "]";
		return result;
	    }
    };
    
    // partial specialization for std::tuple
    template<class... Types>
    struct to_string_helper<std::tuple<Types...> >
    {
	static inline std::string
	to_string(std::tuple<Types...> const & t)
	    {
		return "{" + apply_function(to_string_helper<Types...>::to_string,t) + "}";
	    }
    };
    
}

// to_std_string definition

template<class... Types>
std::string
to_std_string(Types... x)
{
    return to_string_helper<Types...>::to_string(x...);
}



// input_format behind-the-scenes
// we need to use structs and not functions
// since we use a general template with partial specializations

namespace
{
    // general template
    template<class T>
    struct Input_Format_Helper
    {
	static inline std::string format()
	    {
		error_user_define(T,Input_Format_Helper,format());
		return "";
	    }
    };

#define input_format_decl(type)			\
    template<> struct Input_Format_Helper<type> {	\
	static inline std::string format()		\
	    { return #type; }};

    for_each_primitive_type(input_format_decl)

#undef input_format_decl
    
    // partial specialization for std::vector
    template<class T>
    struct Input_Format_Helper<std::vector<T> >
    {
	static inline std::string format()
	    {
		return "{" + Input_Format_Helper<T>::format() + "...}";
	    }
    };
    
}

// input_format definition

template<class T>
std::string
input_format()
{
    return Input_Format_Helper<T>::format();
}


#endif /* STRING_UTILS */
