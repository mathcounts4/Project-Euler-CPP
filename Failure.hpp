#pragma once

#include "TypeUtils.hpp"

#include <string>

#define FAIL_IF(X)				\
    if (X)					\
	return Failure(#X)

struct Failure {
    std::string reason;
    template<class... T> Failure(T&&... t)
	: reason(fwd<T>(t)...) {}
};
