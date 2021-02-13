#include "Section.hpp"

Section::Section(std::string const& info)
    : fInfo(info)
{
    std::cout << "\nSection start: " << info << "\n";
}

Section::~Section()
{
    std::cout << "Section end: " << fInfo << "\n\n";
}
