#pragma once

#include "../AssertUtils.hpp"
#include "../Class.hpp"
#include "../MacroUtils.hpp"
#include "../Optional.hpp"
#include "../Resetter.hpp"
#include "../TypeUtils.hpp"

#include <array>
#include <iostream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#define PRINT_LIMIT 10

namespace std {
    inline namespace __1 {
	       class __iom_t1;
	       class __iom_t2;
	       class __iom_t3;
	       template<class> class __iom_t4;
	       class __iom_t5;
	       class __iom_t6;
	       template<class> class __iom_t7;
	       template<class> class __iom_t8;
	       template<class> class __iom_t9;
	       template<class> class __iom_t10;
    }
}

// std::ostream& << T
template<class T> constexpr bool predefined_basic_ostream_shift_out =
    !is_a<std::basic_string,No_cvref<T> > && !std::is_pointer_v<No_cvref<T> > && !is_same<std::__1::__iom_t1,No_cvref<T> > && !is_same<std::__1::__iom_t2,No_cvref<T> > && !is_same<std::__1::__iom_t3,No_cvref<T> > && !is_a<std::__1::__iom_t4,No_cvref<T> > && !is_same<std::__1::__iom_t5,No_cvref<T> > && !is_same<std::__1::__iom_t6,No_cvref<T> > && !is_a<std::__1::__iom_t7,No_cvref<T> > && !is_a<std::__1::__iom_t8,No_cvref<T> > && !is_a<std::__1::__iom_t9,No_cvref<T> > && !is_a<std::__1::__iom_t10,No_cvref<T> >;

namespace std {
template<class... Args, class T>
std::enable_if_t<predefined_basic_ostream_shift_out<T>,
		 std::basic_ostream<Args...>&> operator<<(std::basic_ostream<Args...>& oss, T&& t) {
    return Class<No_cvref<T> >::print(oss,t);
}
} /* namespace std */

/* global io and class info functions */
template<class T> std::string to_string(T const& t) {
    std::ostringstream oss;
    Class<T>::print(oss,t);
    return oss.str();
}

template<class T> Optional<T> from_string(std::string const& s) {
    std::istringstream iss(s);
    Optional<T> result = Class<T>::parse(iss);
    auto failure = [&s](std::string const& reason) -> Failure {
	return {"Failed to parse " + class_name<T>() +
		" from " + "\"" + s + "\"" +
		" due to:\n" +
		reason};
    };
    
    if (!result)
	return failure(result.cause());
    if (iss.bad())
	return failure("istringstream was bad");
    if (iss.fail())
	iss.clear(iss.rdstate() & ~iss.failbit);
    std::ws(iss);
    if (!iss.eof()) {
	std::ostringstream oss;
	iss >> oss.rdbuf();
	return failure("Extra input provided:\n" + oss.str());
    }
    return result;
}

template<class T> std::string class_name() {
    return Class<T>::name();
}

template<class T> std::string cvref_string() {
    std::string result;
    for (std::pair<int,char const *> p :
	{
	    std::pair{is<No_ref<T>,'c'>,"const"},
	    {is<No_ref<T>,'v'>,"volatile"},
	    {is<T,'l'>,"&"},
	    {is<T,'r'>,"&&"}
	}) {
	if (p.first) {
	    if (result.size() > 0)
		result += " ";
	    result += p.second;
	}
    }
    if (result.size() > 0)
	result = " " + result;
    return result;
}

#define SPECIAL_CHAR_DECL(CHAR_T)					\
    template<class... Args> std::basic_istream<Args...>& operator>>	\
    (std::basic_istream<Args...>& is, CHAR_T& c) {			\
	CHAR_T _c;							\
	is >> _c;							\
	c = _c;								\
	return is;							\
    }									\
    template<class... Args> std::basic_ostream<Args...>& operator<<	\
    (std::basic_ostream<Args...>& os, CHAR_T const& c) {		\
	CHAR_T const _c = c;						\
	return os << _c;						\
    }
SPECIAL_CHAR_DECL(char16_t)
SPECIAL_CHAR_DECL(char32_t)
SPECIAL_CHAR_DECL(wchar_t)
#undef SPECIAL_CHAR_DECL

template<> extern std::ostream& Class<std::string>::print(std::ostream& oss, std::string const & s);
template<> extern std::string Class<std::string>::format();
template<> extern std::string Class<std::string>::name();
template<> extern Optional<std::string> Class<std::string>::parse(std::istream& is);

std::ostream& operator<<(std::ostream& os, __int128_t x);
std::ostream& operator<<(std::ostream& os, __uint128_t x);
std::istream& operator>>(std::istream& is, __int128_t& x);
std::istream& operator>>(std::istream& is, __uint128_t& x);

template<class X> struct Class<Optional<X> > {
    using T = Optional<X>;
    static std::ostream& print(std::ostream& os, T const& t) {
	if (t) {
	    os << *t;
	} else {
	    os << "Failure: " << t.cause();
	}
	return os;
    }
    static Optional<T> parse(std::istream& is) {
	return Class<X>::parse(is);
    }
    static std::string name() {
	return "Optional<" + Class<X>::name() + ">";
    }
    static std::string format() {
	return Class<X>::format();
    }
};

template<class X, class Y> struct Class<std::pair<X,Y> > {
    using T = std::pair<X,Y>;
    static std::ostream& print(std::ostream& os, T const& t) {
	return os << "{" << t.first << "," << t.second << "}";
    }
    static Optional<T> parse(std::istream& is) {
	Resetter resetter{is};
	auto opt = consume_opt(is,'{');
	if (!opt)
	    return Failure(opt.cause());
	    
	auto x = Class<X>::parse(is);
	if (!x)
	    return Failure(x.cause());
	opt = consume_opt(is,',');
	if (!opt)
	    return Failure(opt.cause());
	auto y = Class<Y>::parse(is);
	if (!y)
	    return Failure(y.cause());
	    
	opt = consume_opt(is,'}');
	if (!opt)
	    return Failure(opt.cause());
	resetter.ignore();
	return {*x,*y};
    }
    static std::string name() {
	return "std::pair<" + Class<X>::name() + "," + Class<Y>::name() + ">";
    }
    static std::string format() {
	return "{" + Class<X>::format() + "," + Class<Y>::format() + "}";
    }
};

template<class X, SZ len> struct Class<std::array<X,len> > {
    using T = std::array<X,len>;
    static std::ostream& print(std::ostream& os, T const& t) {
	os << "{";
	int printed = 0;
	for (X const& x : t) {
	    if (printed != 0)
		os << ",";
	    if (printed == PRINT_LIMIT) {
		os << "...";
		break;
	    }
	    os << x;
	    ++printed;
	}
	os << "}";
	return os;
    }
    static Optional<T> parse(std::istream& is) {
	Resetter resetter{is};
	auto opt = consume_opt(is,'{');
	if (!opt)
	    return Failure(opt.cause());
	T t;
	for (SZ i = 0; i < len; ++i) {
	    if (i == 0)
		if (!consume(is,','))
		    break;
	    auto next = Class<X>::parse(is);
	    if (!next)
		return Failure(next.cause());
	    t[i] = std::move(*next);
	}
	opt = consume_opt(is,'}');
	if (!opt)
	    return Failure(opt.cause());
	resetter.ignore();
	return t;
    }
    static std::string name() {
	return "std::array<" + Class<X>::name() + to_string(len) + ">";
    }
    static std::string format() {
	return "{" + Class<X>::format() + ", ...[" + to_string(len) + "]}";
    }
};

#define CONTAINER_ONE_ARG_CLASS(TYPE)					\
    template<class X> struct Class<TYPE<X> > {				\
	using T = TYPE<X>;						\
	static std::ostream& print(std::ostream& os, T const& t) {	\
	    os << "{";							\
	    int printed = 0;						\
	    for (X const& x : t) {					\
		if (printed != 0)					\
		    os << ",";						\
		if (printed == PRINT_LIMIT) {				\
		    os << "...";					\
		    break;						\
		}							\
		os << x;						\
		++printed;						\
	    }								\
	    os << "}";							\
	    return os;							\
	}								\
	static Optional<T> parse(std::istream& is) {			\
	    Resetter resetter{is};					\
	    auto failure = [](std::string const& cause) -> Failure {	\
		return name() + " parse error:\n" + cause;		\
	    };								\
	    auto opt = consume_opt(is,'{');				\
	    if (!opt)							\
		return failure(opt.cause());				\
	    T t;							\
	    if (consume_opt(is,'}')) { /* empty */			\
		resetter.ignore();					\
		return t;						\
	    }								\
	    for (bool started = false; true; ) {			\
		if (started)						\
		    if (!consume(is,','))				\
			break;						\
		auto next = Class<X>::parse(is);			\
		if (!next)						\
		    return failure(next.cause());			\
		t.insert(t.end(),std::move(*next));			\
		started = true;						\
	    }								\
	    opt = consume_opt(is,'}');					\
	    if (!opt)							\
		return failure(opt.cause());				\
	    resetter.ignore();						\
	    return t;							\
	}								\
	static std::string name() {					\
	    return #TYPE "<" + Class<X>::name() + ">";			\
	}								\
	static std::string format() {					\
	    return "{" + Class<X>::format() + ", ...}";			\
	}								\
    }
FOR_EACH_STD_CONTAINER_ONE_ARG(CONTAINER_ONE_ARG_CLASS,;);
#undef CONTAINER_ONE_ARG_CLASS

#define CONTAINER_TWO_ARG_CLASS(TYPE)					\
    template<class X, class Y> struct Class<TYPE<X,Y> > {		\
	using T = TYPE<X,Y>;						\
	static std::ostream& print(std::ostream& os, T const& t) {	\
	    os << "{";							\
	    int printed = 0;						\
	    for (auto const& pair : t) {				\
		if (printed != 0)					\
		    os << ",";						\
		if (printed == PRINT_LIMIT) {				\
		    os << "...";					\
		    break;						\
		}							\
		os << pair;						\
		++printed;						\
	    }								\
	    os << "}";							\
	    return os;							\
	}								\
	static Optional<T> parse(std::istream& is) {			\
	    Resetter resetter{is};					\
	    auto opt = consume_opt(is,'{');				\
	    if (!opt)							\
		return Failure(opt.cause());				\
	    T t;							\
	    for (bool started = false; true; ) {			\
		if (started)						\
		    if (!consume(is,','))				\
			break;						\
	        auto next = Class<std::pair<X,Y> >::parse(is);		\
		if (!next)						\
		    return Failure(next.cause());			\
		t.emplace(std::move(next->first),std::move(next->second)); \
		started = true;						\
	    }								\
	    opt = consume_opt(is,'}');					\
	    if (!opt)							\
		return Failure(opt.cause());				\
	    resetter.ignore();						\
	    return t;							\
	}								\
	static std::string name() {					\
	    return #TYPE "<" + Class<X>::name() + "," + Class<Y>::name() + ">"; \
	}								\
	static std::string format() {					\
	    return "{" + Class<std::pair<X,Y> >::format() + ", ...}";	\
	}								\
    }
FOR_EACH_STD_CONTAINER_TWO_ARGS(CONTAINER_TWO_ARG_CLASS,;);
#undef CONTAINER_TWO_ARGS_CLASS

#define PRIMITIVE_CLASS_DECL(T)						\
    template<>								\
    std::ostream& Class<T>::print(std::ostream& oss, T const& t);	\
    template<> std::string Class<T>::name();				\
    template<> Optional<T> Class<T>::parse(std::istream& is); 		\
    template<> std::string Class<T>::format()

FOR_EACH_PRIMITIVE_TYPE(PRIMITIVE_CLASS_DECL,;);
PRIMITIVE_CLASS_DECL(__int128_t);
PRIMITIVE_CLASS_DECL(__uint128_t);
#undef PRIMITIVE_CLASS_DECL

// std::variant
template<class... Ts> struct Class<std::variant<Ts...>> {
  public:
    using Type = std::variant<Ts...>;
    
  private:
    template<class X>
    static Optional<Type> parseImpl(std::istream& is) {
	Resetter resetter{is};
	if (auto optClassName = consume_opt(is, Class<X>::name() + "{")) {
	    if (auto result = Class<X>::parse(is)) {
		if (auto optCloseBrace = consume_opt(is, '}')) {
		    resetter.ignore();
		    return *result;
		} else {
		    return Failure(optCloseBrace.cause());
		}
	    } else {
		return Failure(result.cause());
	    }
	} else {
	    return Failure(optClassName.cause());
	}
    }
    template<class X1, class X2, class... Xs>
    static Optional<Type> parseImpl(std::istream& is) {
	if (auto first = parseImpl<X1>(is)) {
	    return *first;
	} else if (auto rest = parseImpl<X2, Xs...>(is)) {
	    return *rest;
	} else {
	    return Failure(first.cause() + " | " + rest.cause());
	}
    }
    
  public:
    static std::ostream& print(std::ostream& os, Type const& ts) {
	return std::visit([&os](auto const& t) -> decltype(auto) {
	    using T = No_cvref<decltype(t)>;
	    auto&& newOs = os << Class<T>::name() << "{";
	    return Class<T>::print(newOs, t) << "}";
	}, ts);
    }
    static Optional<Type> parse(std::istream& is) {
	return parseImpl<Ts...>(is);
    }
    static std::string name() {
	std::string separator = ", ";
	std::string withSep = ((separator + Class<Ts>::name()) + ...);
	return "std::variant<" + withSep.substr(separator.size()) + ">";
    }
    static std::string format() {
	std::string separator = " | ";
	std::string withSep = ((separator + Class<Ts>::name() + "{" + Class<Ts>::format() + "}") + ...);
	return withSep.substr(separator.size());
    }
};

