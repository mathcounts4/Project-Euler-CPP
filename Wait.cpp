#include "Wait.hpp"

#include "Now.hpp"
#include "Rand.hpp"

#include <chrono> // std::chrono::microseconds
#include <thread> // std::this_thread::sleep_for

static thread_local double timeMultiplyByFactor = 1;
static thread_local bool neverWaitFlag = false;

double NormalDuration::toDouble() const {
    auto result = randNormal(fMean.count(), fStdDev.count()) * timeMultiplyByFactor;
    if (result < 0) {
	result = 0;
    }
    return result;
}

Time timeFrom(std::chrono::time_point<std::chrono::system_clock> const& start) {
    return now() - start;
}

void waitFor(NormalDuration const& duration) {
    auto microseconds = 1e6 * duration.toDouble();
    auto maxMicroseconds = std::chrono::microseconds::max().count() / 2;
    if (microseconds > maxMicroseconds) {
	microseconds = maxMicroseconds;
    }
    if (microseconds > 0 && !neverWaitFlag) {
	std::this_thread::sleep_for(std::chrono::microseconds(static_cast<unsigned long>(round(microseconds))));
    }
}

ScopedSetValue<bool> neverWait() {
    return ScopedSetValue(neverWaitFlag, true);
}

ScopedSetValue<double> NormalDuration::slowDownBy(double delayFactor) {
    return ScopedSetValue(timeMultiplyByFactor, timeMultiplyByFactor * delayFactor);
}

ScopedSetValue<double> NormalDuration::speedUpBy(double speedFactor) {
    return ScopedSetValue(timeMultiplyByFactor, timeMultiplyByFactor / speedFactor);
}

ScopedSetValue<double> NormalDuration::setSlowDownFactor(double delayFactor) {
    return ScopedSetValue(timeMultiplyByFactor, delayFactor);
}

