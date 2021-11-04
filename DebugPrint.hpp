#pragma once

#include <iostream>

namespace debug {
    
    extern const bool doPrint;
    
    struct Cout {
	template<class T>
	static void print(T&& t) {
	    if (debug::doPrint) {
		std::cout << std::forward<T>(t);
	    }
	}   
    };
    
    constexpr Cout cout;
    
    template<class T>
    Cout operator<<(Cout, T&&t) {
	cout.print(std::forward<T>(t));
	return cout;
    }
    inline Cout operator<<(Cout, std::ostream& (*pf)(std::ostream&)) {
	// for std::endl
	cout.print(pf);
	return cout;
    }
} /* namespace debug */
    
    
