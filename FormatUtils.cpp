#include "FormatUtils.hpp"

template<> std::ostream& Class<Float_Format>::print(std::ostream& os, Float_Format const& fmt) {
    switch (fmt) {
      case Float_Format::Unset:
	return os << "Unset";
      case Float_Format::Fixed:
	return os << "Fixed";
      case Float_Format::Scientific:
	return os << "Scientific";
    }
    __builtin_unreachable();
}

template<> std::string Class<Float_Format>::name() {
    return "Float_Format";
}

template<> std::ostream& Class<Adjust_Format>::print(std::ostream& os, Adjust_Format const& fmt) {
    switch (fmt) {
      case Adjust_Format::Left:
	return os << "Left";
      case Adjust_Format::Right:
	return os << "Right";
      case Adjust_Format::Internal:
	return os << "Internal";
    }
    __builtin_unreachable();
}

template<> std::string Class<Adjust_Format>::name() {
    return "Adjust_Format";
}

