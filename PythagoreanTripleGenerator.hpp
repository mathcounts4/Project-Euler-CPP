#pragma once

#include "Class.hpp"
#include "TypeUtils.hpp"
#include "Vec.hpp"

struct PythagoreanTriple {
    // a^2 + b^2 = c^2
    UI a;
    UI b;
    UI c;
    friend bool operator==(PythagoreanTriple const& x, PythagoreanTriple const& y) {
	return x.a == y.a && x.b == y.b && x.c == y.c;
    }
};
DEFINE_SIMPLE_AGGREGATE_CLASS(PythagoreanTriple, a, b, c);

// unsorted list
Vec<PythagoreanTriple> primitivePythagoreanTriplesUpToPerimeter(UI maxPerimeter);

