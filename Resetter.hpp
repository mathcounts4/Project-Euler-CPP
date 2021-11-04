#pragma once

#include "State.hpp"

// a Resetter stores a T's state on creation and resets it on destruction.
// Calling reset() will reset the state early, but it will only reset once.
template<class T> class Resetter {
  private:
    T& f_t;
    State<T> f_state;
    bool f_reset;
  public:
    Resetter(T& t);
    ~Resetter();
    void ignore();
    bool reset();
};
template<class T> Resetter(T&) -> Resetter<T>;

template<class T> Resetter<T>::Resetter(T& t)
    : f_t(t)
    , f_state{t}
    , f_reset{true}
{}

template<class T> Resetter<T>::~Resetter() {
    try {
	if (f_reset)
	    reset();
    } catch (...) {}
}

template<class T> void Resetter<T>::ignore() {
    f_reset = false;
}

template<class T> bool Resetter<T>::reset() {
    if (!f_reset)
	return false;
    bool const success = f_state.apply(f_t);
    if (success)
	f_reset = false;
    return success;
}

