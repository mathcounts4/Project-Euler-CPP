#pragma once
#ifndef PROGRESS_UTILS
#define PROGRESS_UTILS

#include <cmath>
#include <iostream>
#include <string>

// defines a few functions useful when tracking progress of a program



// My_Remove: a class to delete something from an ostream (usually cout)
// Example usage:

// std::string printed("Hi!");
// std::cout << printed << std::flush;
// std::cout << My_Remove(printed);

class My_Remove
{
    std::size_t const length;
    bool const flush;
public:
    My_Remove(std::string const & s, bool flush = true) :
	length(s.size()),
	flush(flush) {}
    friend std::ostream& operator<<(std::ostream& o, My_Remove const & r)
	{
	    for (std::size_t i = r.length; i > 0; --i)
		// move back, write space, move back
		o << "\b \b";
	    if (r.flush)
	        o << std::flush;
	    return o;
	}
};



// Percent_Tracker: a class to track percentage of completion

// potentially prints a new percentage complete every time ++ or update() is called
// by default, deletes the last printed message upon object destruction

// Do not write to the ostream while this object is using the ostream

// Only necessary constructor argument = total amount, though there are a few optional arguments, including the name of the thing being tracked (max 20 chars)
class Percent_Tracker
{
private:
    int percent;
    double const total;
    double current;
    std::string const name;
    std::string last_printed;
    std::ostream& stream;
    bool const clean_up;
    bool const flush_finish;
public:
    Percent_Tracker(double total,
		    std::string const & name = "",
		    std::ostream& o = std::cout,
		    bool clean_up = true,
		    bool flush_finish = false) :
	percent(-1),
	total(total),
	current(0),
	name(name.substr(0,20)),
	last_printed("[%" +
		     std::string(this->name.size() ? " " + this->name : "") +
		     std::string(total ? "" : ": max 0") +
		     "]"),
	stream(o),
	clean_up(clean_up),
	flush_finish(flush_finish)
	{
	    o << last_printed << std::flush;
	}
    ~Percent_Tracker()
	{
	    if (clean_up)
		finish();
	}
    // If the percentage is new, prints it and returns true
    // Otherwise does nothing and returns false
    bool update(double value)
	{
	    current = value;
	    double curPercent = floor(current/total*100);
	    if (curPercent > 100)
		curPercent = 100;
	    if (total != 0 &&
		percent < 100 &&
		percent < curPercent)
	    {
		// don't have My_Remove flush since we're about to
		stream << My_Remove(last_printed,false);
		percent = curPercent;
		last_printed = "[" +
		    (name.size() ? name + ": " : "") +
		    std::to_string(percent) + "% complete]";
		stream << last_printed << std::flush;

		return true;
	    }
	    return false;
	}
    Percent_Tracker& operator++()
	{
	    update(current+1);
	    return *this;
	}
    Percent_Tracker& operator+=(double value)
	{
	    update(current+value);
	    return *this;
	}
    void finish()
	{
	    stream << My_Remove(last_printed,flush_finish);
	    last_printed = "";
	}
};



#endif /* PROGRESS_UTILS */
