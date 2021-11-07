#pragma once

#include "TypeUtils.hpp"

#include <iostream>
#include <string>

// default false
extern bool print_full_path_in_stack_trace;

#ifdef __APPLE__
// gets the current process name
std::string const& process_name_with_path();
#endif

// prints the stack trace
void print_stack_trace(std::ostream& os = std::cerr, SZ skip_frames = 0);

// returns the stack trace as a string, including line breaks
std::string stack_trace(SZ skip_frames = 0);

// throws an exception of the templatized type with a stack trace.
template<class Exception>
[[noreturn]] void throw_exception(std::string const& reason) {
    throw Exception(reason + "\n" + stack_trace(1));
}

// throws an exception of the templatized type without a stack trace.
template<class Exception>
[[noreturn]] void throw_exception_no_stack(std::string const& reason) {
    throw Exception(reason);
}

