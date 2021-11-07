#pragma once

#include <iostream>
#include <sstream>
#include <tuple>

template<class Func>
auto captureStdOutAndRun(Func&& func) {
    struct Redirect {
	Redirect()
	    : fOldBuf(std::cout.rdbuf(fStringStream.rdbuf())) {
	}
	~Redirect() {
	    try {
		std::cout.rdbuf(fOldBuf);
	    } catch (...) {
	    }
	}
	std::string output() {
	    return fStringStream.str();
	}

	std::stringstream fStringStream;
	std::streambuf* fOldBuf;
    };

    Redirect redirect;
    auto result = func();
    return std::make_tuple(std::move(result), redirect.output());
}

