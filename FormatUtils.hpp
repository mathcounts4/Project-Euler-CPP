#pragma once

#include "Class.hpp"
#include "StringUtils.hpp"
#include "TypeUtils.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>

enum class Float_Format { Unset, Fixed, Scientific };
template<Float_Format> extern std::ios_base::fmtflags const float_flag;
template<> extern std::ostream& Class<Float_Format>::print(std::ostream& os, Float_Format const& fmt);
template<> extern std::string Class<Float_Format>::name();

enum class Adjust_Format { Left, Right, Internal };
template<Adjust_Format> extern std::ios_base::fmtflags const adjust_flag;
template<> extern std::ostream& Class<Adjust_Format>::print(std::ostream& os, Adjust_Format const& fmt);
template<> extern std::string Class<Adjust_Format>::name();

template<> extern std::ios_base::fmtflags const float_flag<Float_Format::Unset>;
template<> extern std::ios_base::fmtflags const float_flag<Float_Format::Fixed>;
template<> extern std::ios_base::fmtflags const float_flag<Float_Format::Scientific>;

template<> extern std::ios_base::fmtflags const adjust_flag<Adjust_Format::Left>;
template<> extern std::ios_base::fmtflags const adjust_flag<Adjust_Format::Right>;
template<> extern std::ios_base::fmtflags const adjust_flag<Adjust_Format::Internal>;

// width = min
// precision = max (only used for floating point)
// default Float_Format = fixed = same number of digits after the decimal point
template<class T, UI width = 8, Adjust_Format adjust_t = Adjust_Format::Right, Float_Format float_t = Float_Format::Fixed, UI precision = width>
struct Formatted {
    T value;
    Formatted(T const& t) : value(t) {}
    Formatted() : value{} {}
};
template<class T> Formatted(T const&) -> Formatted<Cond<std::is_array_v<T>,std::remove_extent_t<T> const*,T> >;

template<class T, UI width, Adjust_Format adjust_t, Float_Format float_t, UI precision>
struct Class<Formatted<T,width,adjust_t,float_t,precision> > {
    using FT = Formatted<T,width,adjust_t,float_t,precision>;
    static std::ostream& print(std::ostream& os, FT const& ft);
    static std::string name();
};




template<class T, UI width, Adjust_Format adjust_t, Float_Format float_t, UI precision>
std::ostream& Class<Formatted<T,width,adjust_t,float_t,precision> >::print(std::ostream& os, FT const& ft) {
    auto flags = os.flags();
    os.setf(adjust_flag<adjust_t>,std::ios_base::adjustfield);
    os.setf(float_flag<float_t>,std::ios_base::floatfield);
    os << std::setw(width);
    os << std::setprecision(precision);
    os << ft.value;
    os.flags(flags);
    return os;
}

template<class T, UI width, Adjust_Format adjust_t, Float_Format float_t, UI precision>
std::string Class<Formatted<T,width,adjust_t,float_t,precision> >::name() {
    std::vector<std::string> opts = {
        to_string(width),
        to_string(adjust_t),
	to_string(float_t),
	to_string(precision)
    };
    
    return "Formatted_" + Class<T>::name() +
	"[" + string_concat(",", opts) + "]";
}

