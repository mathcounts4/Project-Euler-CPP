#pragma once
#ifndef MACRO_UTILS
#define MACRO_UTILS

// defines useful aggregate macros

// for_each_integral_type(macro)
#define for_each_integral_type(macro)		\
    macro(bool)					\
    macro(char)					\
    macro(char16_t)				\
    macro(char32_t)				\
    macro(wchar_t)				\
    macro(signed char)				\
    macro(unsigned char)			\
    macro(short)				\
    macro(unsigned short)			\
    macro(int)					\
    macro(unsigned int)				\
    macro(long)					\
    macro(unsigned long)			\
    macro(long long)				\
    macro(unsigned long long)

// for_each_float_type(macro)
#define for_each_float_type(macro)		\
    macro(float)				\
    macro(double)				\
    macro(long double)

// for_each_primitive_type(macro)
#define for_each_primitive_type(macro)		\
    for_each_integral_type(macro)		\
    for_each_float_type(macro)

#endif /* MACRO_UTILS */
