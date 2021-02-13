#include "FormatUtils.hpp"

template<> extern std::ios_base::fmtflags const float_flag<Float_Format::Unset> = {};
template<> extern std::ios_base::fmtflags const float_flag<Float_Format::Fixed> = std::ios_base::fixed;
template<> extern std::ios_base::fmtflags const float_flag<Float_Format::Scientific> = std::ios_base::scientific;

template<> extern std::ios_base::fmtflags const adjust_flag<Adjust_Format::Left> = std::ios_base::left;
template<> extern std::ios_base::fmtflags const adjust_flag<Adjust_Format::Right> = std::ios_base::right;
template<> extern std::ios_base::fmtflags const adjust_flag<Adjust_Format::Internal> = std::ios_base::internal;

template<> std::ostream& Class<Float_Format>::print(std::ostream& os, Float_Format const & fmt) {
    switch (fmt) {
      case Float_Format::Unset:
	return os << "Unset";
      case Float_Format::Fixed:
	return os << "Fixed";
      case Float_Format::Scientific:
	return os << "Scientific";
    }
}

template<> std::string Class<Float_Format>::name() {
    return "Float_Format";
}

template<> std::ostream& Class<Adjust_Format>::print(std::ostream& os, Adjust_Format const & fmt) {
    switch (fmt) {
      case Adjust_Format::Left:
	return os << "Left";
      case Adjust_Format::Right:
	return os << "Right";
      case Adjust_Format::Internal:
	return os << "Internal";
    }
}

template<> std::string Class<Adjust_Format>::name() {
    return "Adjust_Format";
}

