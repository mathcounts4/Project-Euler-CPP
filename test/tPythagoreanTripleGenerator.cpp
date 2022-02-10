#include "../PythagoreanTripleGenerator.hpp"
#include "harness.hpp"

TEST(PythagoreanTripleGenerator, small) {
    // Wow! See
    // https://r-knott.surrey.ac.uk/Pythag/pythag.html
    auto smallTriples = primitivePythagoreanTriplesUpToPerimeter(300);
    
    std::sort(smallTriples.begin(), smallTriples.end(), [](PythagoreanTriple const& x, PythagoreanTriple const& y) {
	auto const& [ax, bx, cx] = x;
	auto const& [ay, by, cy] = y;
	if (ax != ay) {
	    return ax < ay;
	}
	return bx < by;
    });
    
    decltype(smallTriples) expected{
	{3, 4, 5},
	{5, 12, 13},
	{7, 24, 25},
	{8, 15, 17},
	{9, 40, 41},
	{11, 60, 61},
	{12, 35, 37},
	{13, 84, 85},
	{15, 112, 113},
	{16, 63, 65},
	{20, 21, 29},
	{20, 99, 101},
	{28, 45, 53},
	{33, 56, 65},
	{36, 77, 85},
	{39, 80, 89},
	{44, 117, 125},
	{48, 55, 73},
	{60, 91, 109},
	{65, 72, 97}
    };
    
    CHECK(smallTriples, equals(expected));
}

TEST(PythagoreanTripleGenerator, big) {
    CHECK(primitivePythagoreanTriplesUpToPerimeter(100'000'000).size(), equals(7023027U));
}
