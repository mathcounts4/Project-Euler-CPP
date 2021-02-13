#pragma once

#include <ios>
#include <iostream>
#include <termios.h>

// defines functions and classes for modifying how terminal is treated

// Attempts to get the ios to the input one.
// On failure, throw an exception or prints the failure and returns 1.
int get_ios(termios & ios, bool throw_on_failure = false);

// Attempts to set the ios to the input one.
// On failure, throws or returns 1.
int set_ios(termios const & ios, bool throw_on_failure = false);

// attempts to disable io echo
int disable_echo(bool throw_on_failure = false);

// attempts to enable io echo
int enable_echo(bool throw_on_failure = false);

// disables echo while this object exists.
// resets to previous ios on destruction
struct DisableEcho {
    termios ios;
    bool had_echo;
    DisableEcho(bool throw_on_failure = false);
    // destructor does not throw
    ~DisableEcho();
};
