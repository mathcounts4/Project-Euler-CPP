#pragma once
#ifndef FORMAT_UTILS
#define FORMAT_UTILS

#include <iomanip>
#include <sstream>
#include <string>

#include "StringUtils.hpp"

template<unsigned int precision>
struct double_precision
{
    double const value;
    double_precision(double const value) : value(value) {}
};

template<unsigned int precision>
struct to_string_helper<double_precision<precision> >
{
    static inline
    std::string to_string(double_precision<precision> const & d)
	{
	    std::stringstream ss;
	    ss << std::setprecision(precision) << std::fixed << d.value;
	    return ss.str();
	}
};


#endif /* FORMAT_UTILS */
