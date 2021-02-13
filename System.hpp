#pragma once

#include <string>

#include "Optional.hpp"

struct Command_Result {
    int return_value;
    std::string output;
};

Optional<Command_Result> system_call(std::string const & command);
