#pragma once

#include <string>
#include <vector>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wheader-hygiene"
using namespace std::string_literals;
#pragma clang diagnostic pop

std::string string_concat(
    std::string const & delimeter,
    std::vector<std::string> const & args);

bool string_eq_ignore_case(char const * x, char const * y, std::size_t len = static_cast<std::size_t>(-1));

std::string quoted(std::string const & in);

