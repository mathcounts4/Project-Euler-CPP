#include "../Overload.hpp"
#include "../TypeUtils.hpp"
#include "harness.hpp"

TEST(Overload, generic_lambda) {
    using T = std::variant<int, std::string>;
    auto make_string = [](auto const& x) {
	if constexpr (is_same<No_cvref<decltype(x)>, std::string>) {
	    return x;
	} else {
	    return std::to_string(x);
	}
    };
    
    CHECK(std::visit(Overload{make_string}, T(3)), equals("3"));
    CHECK(std::visit(Overload{make_string}, T("hi")), equals("hi"));
}

static std::string add_one_to_string(int x) {
    return std::to_string(x+1);
}

TEST(Overload, function_and_functor) {
    using T = std::variant<int, std::string>;
    Overload overload{add_one_to_string, [](std::string const& str) { return str + " + 1"; }};
    CHECK(std::visit(overload, T(3)), equals("4"));
    CHECK(std::visit(overload, T("3")), equals("3 + 1"));
}

