/* Problem

f(0) = 4
f(1) = 7
Find f(3000).

*/



/* Solution



Answer: 



*/



#include "ProjectEuler.hpp"

static Optional<UL> f(UI n) {
    return Failure("Unimplemented");
}

SI main(Argc const argc, Argv const argv) {
    B ran_ok = PE::run_problem(
	argc,
	argv,
	f,
	{"f", {"n"}},
	{
	    {{0U}, 4}
	    {{1U}, 7}
	},
	{3'000U});
    return !ran_ok;
}

