#pragma once

#include "ScopedSetValue.hpp"

#include <chrono>

using Time = std::chrono::duration<double>;

class NormalDuration {
  public:
    template<class T1,
	     class T2,
	     class = std::enable_if_t<std::is_constructible_v<Time, T1 const&>>,
	     class = std::enable_if_t<std::is_constructible_v<Time, T2 const&>>>
    constexpr NormalDuration(T1 const& meanTime,
			     T2 const& standardDeviationTime)
	: fMean(meanTime)
	, fStdDev(standardDeviationTime) {
    }
    template<class T,
	     class = std::enable_if_t<std::is_constructible_v<Time, T const&>>>
    constexpr NormalDuration(T const& time)
	: NormalDuration(time, Time(0)) {
    }
    double toDouble() const;
    friend void waitFor(NormalDuration const& duration);

  public:
    [[nodiscard]] static ScopedSetValue<double> slowDownBy(double delayFactor);
    [[nodiscard]] static ScopedSetValue<double> speedUpBy(double speedFactor);
    [[nodiscard]] static ScopedSetValue<double> setSlowDownFactor(double delayFactor);
    
  private:
    Time fMean;
    Time fStdDev;
};


static constexpr NormalDuration Immediately(std::chrono::milliseconds(0),
					    std::chrono::milliseconds(0));

Time timeFrom(std::chrono::time_point<std::chrono::system_clock> const& start);
void waitFor(NormalDuration const& duration);
ScopedSetValue<bool> neverWait();
