#pragma once
#ifndef TUPLE_UTILS
#define TUPLE_UTILS

#include <tuple>

#include "AssertUtils.hpp"
#include "TypeUtils.hpp"

// defines useful tuple-handling functions:
// apply_function applies a function to the arguments of a tuple

// tuple_element with a few less characters
template<std::size_t index, class... Types>
using tuple_element = typename std::tuple_element<index,std::tuple<Types...> >::type;

// tuple type - removes cv and references
template<class... Types>
using no_cvref_tuple = std::tuple<my_remove_cvref<Types>...>;

// apply_function internals
// handles forwarding tuple arguments to a function
namespace
{
    template<std::size_t index, class... Input_Types>
    struct Apply_Function_Helper
    {
	template <class Output_Type, class... Converted>
	static inline Output_Type
	apply_function(function_type<Output_Type,Input_Types...> function,
		       no_cvref_tuple<Input_Types...> const & input_tuple,
		       Converted const & ... converted)
	    {
		return Apply_Function_Helper<index-1,Input_Types...>::template apply_function<Output_Type,tuple_element<index-1,my_remove_cvref<Input_Types>...>,Converted...>
		    (
			function,
			input_tuple,
			std::get<index-1>(input_tuple),
			converted...);
	    }
    };
    template<class... Input_Types>
    struct Apply_Function_Helper<0,Input_Types...>
    {
	template<class Output_Type, class... Converted>
	static inline Output_Type
	apply_function(function_type<Output_Type,Input_Types...> function,
		       no_cvref_tuple<Input_Types...> const & input_tuple,
		       Converted const & ... converted)
	    {
		return function(converted...);
	    }
    };
}

// apply_function interface:

declare_enable_if(apply_function_enable,
		  "Any function passed to apply_function must "
		  "take each input by value or by const &")

template<class Output_Type, class... Input_Types>
typename use_enable_if(apply_function_enable,
		       my_all<is_value_or_const_lref<Input_Types>()...>(),
		       Output_Type)
apply_function(function_type<Output_Type,Input_Types...> function,
	       no_cvref_tuple<Input_Types...> const & input_arguments)
{
    return Apply_Function_Helper<sizeof...(Input_Types),Input_Types...>::template apply_function<Output_Type>(function,input_arguments);
}



#endif /* TUPLE_UTILS */
