#pragma once

#include "Optional.hpp"
#include "Resetter.hpp"

#include <sstream>
#include <string>

/* Partially or fully specialize Class for your class, with these functions defined */
template<class T> struct Class {
    static std::ostream& print(std::ostream& os, T const & t);
    static Optional<T> parse(std::istream& is); // should reset "is" on failure. See Resetter below.
    static std::string name();
    static std::string format();
};

/* Call these functions, which are already defined and use the functions above */
template<class T> std::string to_string(T const & t);
template<class T> Optional<T> from_string(std::string const & s);
template<class T> std::string class_name();
template<class T> std::string cvref_string();
// declared in Impl/Class.hpp: std::ostream& << T

// The following consume whitespace then look for c
// They reset "is" upon failure
[[nodiscard]] Optional<void> consume_opt(std::istream& is, char const c);
[[nodiscard]] bool consume(std::istream& is, char const c);
[[nodiscard]] Optional<void> consume_opt(std::istream& is, std::string const & str);
[[nodiscard]] bool consume(std::istream& is, std::string const & str);

// sentinel that resets the input on destruction unless .ignore() is called
extern template class Resetter<std::istream>;

#define VAR_STR(var) (#var " = " + ::to_string(var))

#include "Impl/Class.hpp"
