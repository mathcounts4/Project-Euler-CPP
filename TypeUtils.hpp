#pragma once
#ifndef TYPE_UTILS
#define TYPE_UTILS

// Defines template function type_name<T>() for getting the name of a type

#include <string>
#include <vector>

#include "AssertUtils.hpp"
#include "MacroUtils.hpp"

// remove cv and ref
template<class T>
using my_remove_cv = typename std::remove_cv<T>::type;

template<class T>
using my_remove_ref = typename std::remove_reference<T>::type;

template<class T>
using my_remove_cvref = my_remove_cv<my_remove_ref<T> >;

// function type
template<class Output_Type, class... Input_Types>
using function_type = Output_Type(*)(Input_Types...);

// type_name behind-the-scenes
namespace
{
    /* general template */
    template<class T> struct type_name_helper
    {
	static inline std::string name()
	    {
		error_user_define(T,type_name_helper,name());
		return "";
	    }
    };
    
#define make_type_name(type)					\
    template<> struct type_name_helper<type> {			\
    static inline std::string name() { return #type; }};

    for_each_primitive_type(make_type_name)
    
#undef make_type_name

    /* vector specialization */
    template<class T> struct type_name_helper<std::vector<T> >
    {
	static inline std::string name()
	    {
		return "vector<" + type_name_helper<T>::name() + ">";
	    }
    };
}

// type_name definition

template<class T>
std::string
type_name()
{
    return type_name_helper<T>::name();
}

#endif /* TYPE_UTILS */
