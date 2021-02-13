#pragma once

#define STACK_PRINTER_CONCAT(X,Y) X##Y
#define STACK_PRINTER StackPrinter STACK_PRINTER_CONCAT(stackPrinter,__LINE__)(__PRETTY_FUNCTION__)

#include <string>

struct StackPrinter {
  private:
    static unsigned int depth;
    std::string fFunctionName;
  public:
    StackPrinter(char const * functionName);
    ~StackPrinter();
};
