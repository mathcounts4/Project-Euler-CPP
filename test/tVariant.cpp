#include "../Variant.hpp"
#include "harness.hpp"

#include "../TypeUtils.hpp"

#include <string>

TEST(Variant, Construction) {
    CHECK(std::holds_alternative<int>(Variant<int, std::string>(3)), isTrue());
    CHECK(std::holds_alternative<std::string>(Variant<int, std::string>("hi")), isTrue());
}

TEST(Variant, split) {
    auto inc = [](auto& x) { ++x; };
    Variant<int, std::string> x(3);
    auto [asInt, asString] = x.split();
    ASSERT(asInt != nullptr, isTrue());
    CHECK(asString == nullptr, isTrue());
    inc(*asInt);
    CHECK(std::get<int>(x), equals(4));
    x = "hello";
    auto [asInt2, asString2] = x.split();
    CHECK(asInt2 == nullptr, isTrue());
    ASSERT(asString2 != nullptr, isTrue());
    *asString2 += " world";
    CHECK(std::get<std::string>(x), equals("hello world"));

    auto const& y = x;
    auto [asConstInt, asConstString] = y.split();
    CHECK(asConstInt == nullptr, isTrue());
    ASSERT(asConstString != nullptr, isTrue());
    static_assert(is<std::remove_reference_t<decltype(*asConstInt)>, 'c'>, "Expected .split to return const pointers from a const object");
    static_assert(is<std::remove_reference_t<decltype(*asConstString)>, 'c'>, "Expected .split to return const pointers from a const object");
}
