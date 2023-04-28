#include <chrono>
#include <cmath>
#include <utility>

#include "Class.hpp"
#include "ExitUtils.hpp"
#include "FormatUtils.hpp"
#include "ProgressUtils.hpp"

My_Remove::My_Remove(std::size_t length, bool flush)
    : fLength(length)
    , fFlush(flush) {
}

My_Remove::My_Remove(std::string const& s, bool flush)
    : My_Remove(s.size(),flush) {
}

template<> std::ostream& Class<My_Remove>::print(std::ostream& o, My_Remove const& r) {
    for (std::size_t i = r.fLength; i > 0; --i)
	// move back, write space, move back
	o << "\b \b";
    if (r.fFlush) {
	o << std::flush;
    }
    return o;
}




Percent_Tracker::Percent_Tracker(
    double total,
    std::string const& name,
    double thresholdPercent,
    double timeEstThreshold,
    std::ostream& o,
    bool cleanUp,
    bool flushFinish)
    : fDisableEcho{}
    , fStart{std::chrono::high_resolution_clock::now()}
    , fPercent(0)
    , fThresholdPercent(thresholdPercent)
    , fTimeEstThreshold(timeEstThreshold)
    , fTotal(total)
    , fCurrent(0)
    , fPrefix(name.size() ? name + ": " : "")
    , fLastPrinted()
    , fStream(o)
    , fCleanUp(cleanUp)
    , fFlushFinish(flushFinish) {
}

Percent_Tracker::~Percent_Tracker() {
    try {
	if (fCleanUp) {
	    fStream << My_Remove(fLastPrinted,fFlushFinish);
	}
    } catch (...) {
	// never throw from destructor
    }
}

bool Percent_Tracker::update(double value) {
    fCurrent = value;
    if (fTotal <= 0) {
	return false;
    }
    double newPercent = fCurrent / fTotal*100;
    if (newPercent > 100) {
	newPercent = 100;
    }
    auto time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = time-fStart;
    if (diff.count() > 0.1) { // for very fast results, don't bother displaying
	if (fPercent + (newPercent >= 100 ? 0 : fThresholdPercent) < newPercent) {
	    fPercent = newPercent;
	    // don't have My_Remove flush since we're about to
	    fStream << My_Remove(fLastPrinted,false);
	    using Percent_Format = Formatted<D,5,Adjust_Format::Right,Float_Format::Fixed,2>;
	    using Time_Format = Formatted<D,3,Adjust_Format::Right,Float_Format::Fixed,0>;
	    std::string time_left;
	    if (fPercent < 100 && diff.count() >= fTimeEstThreshold) {
		time_left = ", " + to_string(Time_Format((100./fPercent-1)*diff.count())) + " s remaining" + " (" + to_string(Time_Format(100./fPercent*diff.count())) + " s total)";
	    }
	    fLastPrinted =
		"[" + fPrefix +
		to_string(Percent_Format(fPercent)) + "%" +
		" in " + to_string(Time_Format(diff.count())) + " s" +
		time_left + "]";
	    fStream << fLastPrinted << std::flush;
	
	    return true;
	}
    }
    return false;
}

Percent_Tracker& Percent_Tracker::operator++() {
    return (*this) += 1;
}

Percent_Tracker& Percent_Tracker::operator+=(double value) {
    update(fCurrent + value);
    return *this;
}
