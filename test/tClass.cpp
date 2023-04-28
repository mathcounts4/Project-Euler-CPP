#include "../Class.hpp"
#include "../Vec.hpp"
#include "harness.hpp"

TEST(Class, variant) {
    using T = std::variant<int, long, std::string>;
    using CL = Class<T>;
    CHECK(class_name<T>(), equals("std::variant<int, long, std::string>"));
    CHECK(CL::format(), equals("int | long | \"characters...\""));
    CHECK(to_string(T(3)), equals("3"));
    CHECK(to_string(T("hello")), equals("\"hello\""));
    auto val = from_string<T>("std::string{\"hola\"}");
    ASSERT(val, isTrue());
    CHECK(*val, equals(T("hola")));
}

TEST(Class, PrintVecOfBool) {
    Vec<B> v{0, 1, 0};
    CHECK(to_string(v), equals("{0,1,0}"));
}

