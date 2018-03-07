#pragma once
#ifndef EXIT_UTILS
#define EXIT_UTILS

#include <execinfo.h>
#include <iostream>
#include <string>
#include <unistd.h>

// does not return
[[noreturn]] void my_exit(std::string const & s)
{
    std::cerr << "Error: " << s << std::endl;
    
    void * array[100];
    std::size_t size = backtrace(array,100);

    backtrace_symbols_fd(array,size,STDERR_FILENO);
    exit(1);
}

#endif /* EXIT_UTILS */
