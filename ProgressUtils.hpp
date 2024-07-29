#pragma once

#include <iostream>
#include <string>

#include "Class.hpp"
#include "IOUtils.hpp"

// defines a few functions useful when tracking progress of a program



// My_Remove: a class to delete something from an ostream (usually cout)
// Example usage:

// std::string printed("Hi!");
// std::cout << printed << std::flush;
// std::cout << My_Remove(printed);

class My_Remove {
    std::size_t const fLength;
    bool const fFlush;
  public:
    My_Remove(std::size_t length, bool flush = true);
    My_Remove(std::string const& s, bool flush = true);
    friend struct Class<My_Remove>;
};



// Percent_Tracker: a class to track percentage of completion

// potentially prints a new percentage complete every time ++ or update() is called
// by default, deletes the last printed message upon object destruction

// Do NOT write to the ostream while this object is using the ostream

// Only necessary constructor argument = total amount
class Percent_Tracker {
  private:
    DisableEcho fDisableEcho;
    std::chrono::time_point<std::chrono::high_resolution_clock> fStart;
    double fPercent;
    double const fThresholdPercent;
    double const fTimeEstThreshold;
    double const fTotal;
    double fCurrent;
    std::string const fPrefix;
    std::string fLastPrinted;
    std::ostream& fStream;
    bool const fCleanUp;
    bool const fFlushFinish;

    std::string validate_name(std::string const& name);
    
  public:
    // name can be at most 20 characters
    Percent_Tracker(
	double total,
	std::string const& name = "",
	double thresholdPercent = 1e-2,
	double timeEstThreshold = 10,
	std::ostream& o = std::cout,
	bool cleanUp = true,
	bool flushFinish = false);
    
    ~Percent_Tracker();
    
    // If the percentage is greater than before (including decimals), prints it and returns true
    // Otherwise does nothing and returns false
    bool update(double value);

    // only pre-increment is supported
    // adds one to the existing value
    Percent_Tracker& operator++();

    // adds to the existing value
    Percent_Tracker& operator+=(double value);
};

class Timer {
  private:
    std::string fName;
    std::chrono::time_point<std::chrono::high_resolution_clock> fStart;

  public:
    Timer(std::string name);
    ~Timer();
};

