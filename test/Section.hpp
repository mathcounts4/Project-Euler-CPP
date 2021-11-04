#pragma once

#include <iostream>
#include <string>

struct Section
{
    std::string fInfo;
    Section(std::string const& info);
    ~Section();
};
