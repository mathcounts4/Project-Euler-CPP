#pragma once

#include "MacroUtils.hpp"
#include "Optional.hpp"
#include "Resetter.hpp"

#include <sstream>
#include <string>

/* Partially or fully specialize Class for your class, with these functions defined */
template<class T> struct Class {
    static std::ostream& print(std::ostream& os, T const& t);
    static Optional<T> parse(std::istream& is); // should reset "is" on failure. See Resetter below.
    static std::string name();
    static std::string format();
};

/* Call these functions, which are already defined and use the functions above */
template<class T> std::string to_string(T const& t);
template<class T> Optional<T> from_string(std::string const& s);
template<class T> std::string class_name();
template<class T> std::string cvref_string();
// declared in Impl/Class.hpp: std::ostream& << T

// The following consume whitespace then look for c
// They reset "is" upon failure
[[nodiscard]] Optional<void> consume_opt(std::istream& is, char const c);
[[nodiscard]] bool consume(std::istream& is, char const c);
[[nodiscard]] Optional<void> consume_opt(std::istream& is, std::string const& str);
[[nodiscard]] bool consume(std::istream& is, std::string const& str);

// sentinel that resets the input on destruction unless .ignore() is called
extern template class Resetter<std::istream>;

#define COMMA_IF_STARTED_AND_MEMBER(MEMBER)			\
    << comma_if_started(started) << (Class<decltype(t.MEMBER)>::print(oss, t.MEMBER), "")
#define MEMBER_OPT_PARSE(MEMBER)					\
    if (started) {							\
	if (auto opt = consume_opt(is, ','); !opt)			\
	    return Failure("Before " #MEMBER ": " + opt.cause());	\
    } else {								\
	started = true;							\
    }									\
    auto MEMBER##_opt = Class<decltype(declval<Type>().MEMBER)>::parse(is); \
    if (!MEMBER##_opt)							\
	return Failure("No " #MEMBER ": " + MEMBER##_opt.cause());
#define MEMBER_OPT_COMMA(MEMBER) *MEMBER##_opt,
#define COMMA_IF_STARTED_AND_FORMAT(MEMBER)				\
    << comma_if_started(started) << Class<decltype(declval<Type>().MEMBER)>::format()
#define DEFINE_SIMPLE_AGGREGATE_CLASS(NAME /* X */, ... /* members: a, b, ... */) \
    template<>								\
    struct Class<NAME> {						\
	using Type = NAME;						\
	static char const* comma_if_started(bool& started) {		\
	    if (started) {						\
		return ", ";						\
	    } else {							\
		started = true;						\
		return "";						\
	    }								\
	}								\
	static std::ostream& print(std::ostream& oss, Type const& t) {	\
	    bool started = false;					\
	    return oss << '{' FOR_EACH(COMMA_IF_STARTED_AND_MEMBER, __VA_ARGS__) << '}'; \
	}								\
	static Optional<NAME> parse(std::istream& is) {			\
	    Resetter resetter{is};					\
	    if (auto opt = consume_opt(is, '{'); !opt)			\
		return Failure(opt.cause());				\
	    bool started = false;					\
	    FOR_EACH(MEMBER_OPT_PARSE, __VA_ARGS__);			\
	    if (auto opt = consume_opt(is, '}'); !opt)			\
		return Failure(opt.cause());				\
	    resetter.ignore();						\
	    return Type{FOR_EACH(MEMBER_OPT_COMMA, __VA_ARGS__)};	\
	}								\
	static std::string name() {					\
	    return #NAME;						\
	}								\
	static std::string format() {					\
	    bool started = false;					\
	    std::ostringstream oss;					\
	    oss << '{';							\
	    oss FOR_EACH(COMMA_IF_STARTED_AND_FORMAT, __VA_ARGS__);	\
	    oss << '}';							\
	    return oss.str();						\
	}								\
    }
    
#include "Impl/Class.hpp"
