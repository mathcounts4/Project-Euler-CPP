#include "../Class.hpp"
#include "harness.hpp"

TEST(Class,variant) {
    using T = std::variant<int, long, std::string>;
    using CL = Class<T>;
    CHECK(class_name<T>(), equals("std::variant<int, long, std::string>"));
    CHECK(CL::format(), equals("int{int} | long{long} | std::string{\"characters...\"}"));
    CHECK(to_string(T(3)), equals("int{3}"));
    CHECK(to_string(T("hello")), equals("std::string{\"hello\"}"));
    auto val = from_string<T>("std::string{\"hola\"}");
    ASSERT(val, isTrue());
    CHECK(*val, equals(T("hola")));
}

