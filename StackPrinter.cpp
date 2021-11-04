#include "StackPrinter.hpp"

#include <iostream>
#include <cstring>

unsigned int StackPrinter::depth = 0;

StackPrinter::StackPrinter(char const * functionName)
    : fFunctionName(functionName) {
    auto last_rbracket = strstr(&fFunctionName[0]," [");
    if (last_rbracket)
	fFunctionName = fFunctionName.substr(0,static_cast<std::size_t>(last_rbracket-&fFunctionName[0]));
    std::cout << std::string(2*depth,' ') << "                [" << depth << "+] " << fFunctionName << std::endl;
    ++depth;
}

StackPrinter::~StackPrinter() {
    --depth;
    std::cout << std::string(2*depth,' ') << "                [" << depth << "-] " << fFunctionName << std::endl;
}
