#pragma once

#include "harnessImpl.hpp"

#include <iostream>
#include <regex>
#include <string>

// note - do NOT declare/define your own main
int main();

/* usage:
 * TEST(BigInt,Cast) {
 *   BigInt x(7);
 *   ASSERT(x,isGreaterThan(6));
 *   CHECK(*x.convert<int>(),equals(7));
 * }
 */
#define TEST(PACKAGE,NAME) TEST_IMPL(PACKAGE,NAME)

// fails the test (printing info) but the test keeps running
#define CHECK(VALUE,FUNCTOR) CHECK_IMPL(VALUE,FUNCTOR)

// fails the test (printing info) and the test point stops
#define ASSERT(VALUE,FUNCTOR) ASSERT_IMPL(VALUE,FUNCTOR)

#define CHECK_THROWS(EXPR,EXPECTED_EXCEPTION) CHECK_THROWS_IMPL(EXPR,EXPECTED_EXCEPTION)
bool shouldRunSlowTests();

TrueFunctor isTrue();
FalseFunctor isFalse();
template<class T> auto equals(T const& y);
template<class T> auto isNotEqualTo(T const& y);
template<class T> auto isLessThan(T const& y);
template<class T> auto isLeq(T const& y);
template<class T> auto isGreaterThan(T const& y);
template<class T> auto isGeq(T const& y);
auto matches(std::string const& s);
auto matches(std::regex const& r);
auto doesNotMatch(std::string const& s);
auto doesNotMatch(std::regex const& r);
auto regexEscape(std::string const& s); // escapes special characters

inline auto matches(std::string const& s) {
    return [&s](std::regex const& r) -> bool { return std::regex_match(s,r); };
}

inline auto matches(std::regex const& r) {
    return [&r](std::string const& s) -> bool { return std::regex_match(s,r); };
}

inline auto doesNotMatch(std::string const& s) {
    return [&s](std::regex const& r) -> bool { return !std::regex_match(s,r); };
}

inline auto doesNotMatch(std::regex const& r) {
    return [&r](std::string const& s) -> bool { return !std::regex_match(s,r); };
}

inline auto regexEscape(std::string const& s) {
    std::string result;
    for (char c : s) {
	switch (c) {
	  case '^':
	  case '$':
	  case '\\':
	  case '.':
	  case '*':
	  case '+':
	  case '?':
	  case '(':
	  case ')':
	  case '[':
	  case ']':
	  case '{':
	  case '}':
	  case '|':
	    result += '\\';
	    [[fallthrough]];
	  default:
	    result += c;
	}
    }
    return result;
}
