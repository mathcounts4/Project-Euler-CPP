#pragma once

template<class T>
struct Cleanup {
    T&& t;
    ~Cleanup() {
	try {
	    static_cast<T&&>(t)();
	} catch (...) {
	    // can't throw from destructor
	}
    }	
};
template<class T> Cleanup(T&&) -> Cleanup<T>;

