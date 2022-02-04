#include <istream>
#include <sstream>
#include <string>

#include "Class.hpp"
#include "../MacroUtils.hpp"
#include "../Optional.hpp"
#include "../SafeMath.hpp"
#include "../TypeUtils.hpp"

#define PRIMITIVE_CLASS_DEF(T)						\
    template<>								\
    std::ostream& Class<T>::print(std::ostream& oss, T const & t) {	\
	return oss << t;						\
    }									\
    template<> std::string Class<T>::name() {				\
	return #T;							\
    }									\
    template<> Optional<T> Class<T>::parse(std::istream& is) {		\
	Resetter resetter{is};						\
	T result;							\
	std::ws(is);							\
	if (is.eof())							\
	    return Failure("EOF encountered when parsing " + name());	\
	is >> result;							\
	if (!is)							\
	    return Failure("Invalid input for " + name());		\
	resetter.ignore();						\
	return result;							\
    }									\
    template<> std::string Class<T>::format() {				\
	return #T;							\
    }

FOR_EACH_PRIMITIVE_TYPE(PRIMITIVE_CLASS_DEF,;)
PRIMITIVE_CLASS_DEF(__int128_t)
PRIMITIVE_CLASS_DEF(__uint128_t)
#undef PRIMITIVE_CLASS_DEF

template<> std::ostream& Class<std::string>::print(std::ostream& oss, std::string const & s) {
    return oss << "\"" << s << "\"";
}

template<> std::string Class<std::string>::format() {
    return "\"characters...\"";
}

template<> std::string Class<std::string>::name() {
    return "std::string";
}

template<> Optional<std::string> Class<std::string>::parse(std::istream& is) {
    Resetter resetter{is};
    std::ws(is);
    if (auto opt = consume_opt(is,'"'); !opt)
	return Failure("Parsing " + name() + ": " + opt.cause());

    std::ostringstream oss;
    bool escape = false;
    while (is.good()) {
	char const next = static_cast<char>(is.get());
	if (!escape && next == '\\') {
	    escape = true;
	} else if (!escape && next == '"') {
	    resetter.ignore();
	    return oss.str();
	} else {
	    escape = false;
	    oss << next;
	}
    }
    return Failure("Parsing " + name() + " reached end of input. String so far: " +
		   "\"" + oss.str() + "\"");
}

std::ostream& operator<<(std::ostream& os, __int128_t x) {
    std::string result;
    bool const negative = x < 0;
    do {
	result += '0' + static_cast<char>(my_abs(static_cast<int>(x%10)));
    } while (x /= 10);
    if (negative)
	result += '-';
    std::reverse(result.begin(),result.end());
    return os << result;
}
std::ostream& operator<<(std::ostream& os, __uint128_t x) {
    std::string result;
    do {
	result += '0' + static_cast<char>(my_abs(static_cast<int>(x%10)));
    } while (x /= 10);
    std::reverse(result.begin(),result.end());
    return os << result;
}

std::istream& operator>>(std::istream& is, __int128_t& x) {
    Resetter resetter{is};
    auto failure = [&resetter,&is]() -> std::istream& {
	resetter.reset();
	is.setstate(std::ios_base::failbit);
	return is;
    };
    
    std::ws(is);

    __int128_t result = 0;
    SafeMath<__int128_t> safe_result(result);
    bool const negative = is.peek() == '-';
    if (negative)
	is.get();
    bool started = false;
    while (is) {
	char const next = static_cast<char>(is.peek());
	if (std::isdigit(next)) {
	    is.get();
	    started = true;
	    char const digit = next-'0';
	    safe_result *= 10;
	    if (negative) {
	        safe_result -= digit;
	    } else {
	        safe_result += digit;
	    }
	} else if (std::isspace(next) || next == std::char_traits<char>::eof()) {
	    break;
	} else {
	    return failure();
	}
    }
    if (!started || !safe_result.ok())
	return failure();
    resetter.ignore();
    x = result;
    return is;
}

std::istream& operator>>(std::istream& is, __uint128_t& x) {
    Resetter resetter{is};
    auto failure = [&resetter,&is]() -> std::istream& {
	resetter.reset();
	is.setstate(std::ios_base::failbit);
	return is;
    };
    
    std::ws(is);

    __uint128_t result = 0;
    SafeMath<__uint128_t> safe_result(result);
    bool started = false;
    while (is) {
	char const next = static_cast<char>(is.peek());
	if (std::isdigit(next)) {
	    char const digit = next-'0';
	    is.get();
	    started = true;
	    (safe_result *= 10) += digit;
	} else if (std::isspace(next) || next == std::char_traits<char>::eof()) {
	    break;
	} else {
	    return failure();
	}
    }
    if (!started || !safe_result.ok())
	return failure();
    resetter.ignore();
    x = result;
    return is;
}
