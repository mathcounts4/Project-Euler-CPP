// compile: g++ -std=c++11 -O3 Testing.cpp

// try calling the executable in various ways:
// ./a.out
// ./a.out test all
// ./a.out test 5
// ./a.out [3,4,5] 1
// ./a.out [3,4,5] 1 1.2
// ./a.out [3,4,9] 3
// ./a.out 3 4 5 1.2



// example problem:
// Given three integer sides that form a triangle and the triangle's density, compute the mass of the triangle

// The triangle sides are strictly bounded above by 1^15

// Sample test cases:
// 1. ({3,4,5},1) -> 6
// 2. ({5,5,6},2.5) -> 30
// 3. ({6,7,12},0) -> 0

// Case to submit:
// ({50,79,90},3.465)



#include <cmath>

#include "ProgressUtils.hpp"
#include "ProjectEuler.hpp"
#include "TypeUtils.hpp"

// define the method that computes the solution:
Optional<D> compute_mass(V<UL> triangle, D density) {
    // triangle must have 3 sides
    FAIL_IF(triangle.size() != 3);
    
    // verify upper bound on sides
    for (std::size_t i = 0; i < 3; ++i)
        FAIL_IF(triangle[i] >= 1e15);

    // check if the longest side is too long
    FAIL_IF(triangle[0] + triangle[1] + triangle[2] <=
	    2*std::max({triangle[0],triangle[1],triangle[2]}));

    // density < 0 doesn't make sense
    FAIL_IF(density < 0);
    
    UL x = triangle[0];
    UL y = triangle[1];
    UL z = triangle[2];
    
    D s = (x+y+z)/2.;

    SL limit = 3e9;
    {
	Percent_Tracker tracker(limit,"Useless loop");
	for (SL i = 0; i < limit; ++i)
	    ++tracker;
    }

    return density * sqrt(s*(s-x)*(s-y)*(s-z));
}

// Sample test cases:
// 1. ({3,4,5},1) -> 6
// 2. ({5,5,6},2.5) -> 30
// 3. ({6,7,12},0) -> 0

// Case to submit:
// ({50,79,90},3.465)
SI main(Argc const argc, Argv const argv) {
    B ran_ok = PE::run_problem(
	argc,
	argv,
	compute_mass,
	{"compute_mass",{"triangle","density"}},
	{
	    {{{3,4,5},1},6},
	    {{{5,5,6},2.5},30},
	    {{{6,7,12},0},0}
	},
	{{50,79,90},3.465});
    return !ran_ok;
}
