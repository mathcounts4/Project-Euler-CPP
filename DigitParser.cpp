#include "DigitParser.hpp"

std::optional<DigitParser> DigitParser::fromPrefixChar(SI prefix) {
    switch (prefix) {
      case 'x':
      case 'X':
	// 0x... -> hex
	return HexParser;
      case 'o':
      case 'O':
	// 0o... -> octal
	return OctalParser;
	// 0b... -> binary
      case 'b':
      case 'B':
	return BinaryParser;
	// 0d... -> decimal
      case 'd':
      case 'D':
	return DecimalParser;
      default:
	return std::nullopt;
    }
}

std::optional<UI> hexDigitParser(C value) {
    if (auto decimal = distFromBottomIfInRangeInclusive<'0', '9'>(value)) {
	return *decimal;
    } else if (auto lowercaseHex = distFromBottomIfInRangeInclusive<'a', 'f'>(value)) {
	return *lowercaseHex + 10u;
    } else if (auto uppercaseHex = distFromBottomIfInRangeInclusive<'A', 'F'>(value)) {
	return *uppercaseHex + 10u;
    } else {
	return std::nullopt;
    }
}
