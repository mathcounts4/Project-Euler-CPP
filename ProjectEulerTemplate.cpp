/* Problem

Problem Title
Problem Number

f(1) = 4
f(2) = 7
Find f(3000).

*/



/* Solution



Answer: 



*/



#include "ProjectEuler.hpp"

static Optional<UL> f(UI n) {
    FAIL_IF(n < 1);
    if (n == 1) {
	return 4;
    }
    return Failure("Unimplemented");
}

SI main(Argc const argc, Argv const argv) {
    B ran_ok = PE::run_problem(
	argc,
	argv,
	f,
	{"f", {"n"}},
	{
	    {{1U}, 4},
	    {{2U}, 7}
	},
	{3'000U});
    return !ran_ok;
}

