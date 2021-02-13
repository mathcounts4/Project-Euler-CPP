#include <cctype>

#include "StringUtils.hpp"

std::string string_concat(
    std::string const & delimeter,
    std::vector<std::string> const & args) {
    std::string result;
    if (args.size() > 0)
	result += args[0];
    for (std::size_t index = 1; index < args.size(); ++index) {
	result += delimeter;
	result += args[index];
    }
    return result;
}

bool string_eq_ignore_case(char const * x, char const * y, std::size_t len) {
    for (std::size_t i = 0; i < len; ++i) {
	if (std::tolower(x[i]) != std::tolower(y[i])) {
	    return false;
	} else if (x[i] == 0) {
	    return true;
	}
    }
    return true;
}

std::string quoted(std::string const & in) {
    return '"' + in + '"';
}

