#pragma once

#include "TypeUtils.hpp"

#include <optional>

struct DigitParser {
    function_pointer<std::optional<UI>, C> fCharToDigit;
    UI fMultPerDigit;
    std::optional<UI> fExactLog2;

    // recognizes x/X/o/O/b/B/d/D for parsing 0xff / 0o77 / 0b110 / 0d99
    // input is an int since std::istream::peek() returns int
    static std::optional<DigitParser> fromPrefixChar(SI prefix);
};

template<C Low, C High>
static std::optional<UI> distFromBottomIfInRangeInclusive(C value) {
    if (Low <= value && value <= High) {
	return static_cast<UI>(value - Low);
    } else {
	return std::nullopt;
    }
}

std::optional<UI> hexDigitParser(C value);

constexpr DigitParser DecimalParser{&distFromBottomIfInRangeInclusive<'0', '9'>, 10, std::nullopt};
constexpr DigitParser BinaryParser{&distFromBottomIfInRangeInclusive<'0', '1'>, 2, 1};
constexpr DigitParser OctalParser{&distFromBottomIfInRangeInclusive<'0', '7'>, 8, 3};
constexpr DigitParser HexParser{&hexDigitParser, 16, 4};
