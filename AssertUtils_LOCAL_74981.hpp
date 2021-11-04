#pragma once
#ifndef ASSERT_UTILS
#define ASSERT_UTILS

#include <type_traits>

// defines utility types for compile time static_assert statements


template<class... Args>
struct My_False { static constexpr bool value = false; };

// Macro to ask the user to define a certain class<T> with a certain static inline method
#define error_user_define(type,struct,method)				\
    static_assert(My_False<type>::value,				\
		  "Please define struct "				\
		  #struct "<" #type ">"					\
		  " for your type, with static inline method "		\
		  #method)


/* type handling: */

/* determines if a type is a value type or a const lvalue reference */
template<class T>
constexpr bool
is_value_or_const_lref()
{
    return !std::is_reference<T>::value ||
	(std::is_lvalue_reference<T>::value &&
	 std::is_const<typename std::remove_reference<T>::type>::value);
}


/*
My_Enable_If : disables a function with a specified compiler message

Usage:

declare_enable_if(unique_name, message)

template<...>
typename use_enable_if(unique_name, bool_value, return_type)
function_name(...)
*/

template<const int *, bool, class T>
struct My_Enable_If;

#define declare_enable_if(unique_name,message)			\
    constexpr int unique_name = 0;				\
    template<class T>						\
    struct My_Enable_If<&unique_name,true,T>			\
    {								\
	using type = T;						\
    };								\
    template<class T>						\
    struct My_Enable_If<&unique_name,false,T>			\
    {								\
	static_assert(My_False<T>::value,message);		\
    };

#define use_enable_if(unique_name,bool_value,return_type)	\
    My_Enable_If<&unique_name,bool_value,return_type>::type
    


/* boolean operations: any, all */

/* helper for my_any */
namespace
{
    /* template class declaration */
    template<bool... values>
    struct my_any_helper;

    /* no arguments -> any is false */
    template<>
    struct my_any_helper<>
    {
	static constexpr bool value = false;
    };

    /* first argument true -> any is true */
    template<bool... values>
    struct my_any_helper<true,values...>
    {
	static constexpr bool value = true;
    };
    
    /* first argument false -> look at the rest */
    template<bool... values>
    struct my_any_helper<false,values...>
    {
	static constexpr bool value = my_any_helper<values...>::value;
    };
}

/* my_any<true,false,false>() */
template<bool... values>
constexpr bool
my_any()
{
    return my_any_helper<values...>::value;
}

/* helper for my_all */
namespace
{
    /* template class declaration */
    template<bool... values>
    struct my_all_helper;

    /* no arguments -> all is true */
    template<>
    struct my_all_helper<>
    {
	static constexpr bool value = true;
    };

    /* first argument false -> all is false */
    template<bool... values>
    struct my_all_helper<false,values...>
    {
	static constexpr bool value = false;
    };
    
    /* first argument true -> look at the rest */
    template<bool... values>
    struct my_all_helper<true,values...>
    {
	static constexpr bool value = my_all_helper<values...>::value;
    };
}

/* my_all<true,false,true>() */
template<bool... values>
constexpr bool
my_all()
{
    return my_all_helper<values...>::value;
}


#endif /* ASSERT_UTILS */
