#include <iostream>

#include "captureStdOut.hpp"
#include "harness.hpp"

// Dummy problem:

// f(x, y) = { 0 if x = 0 and y = 0
//             1 if x = 0
//             2 if y = 0
//             f(x, y-1) + f(x-1, y) otherwise

// You are given that:
// f(0, 5) = 1
// f(7, 0) = 2
// f(2, 2) = 9

// Find (f(800, 321) mod 10^9+7) / 10^4, rounded to 5 decimals.


// START OF DEMO SOLUTION

#include "../FormatUtils.hpp"
#include "../Mod.hpp"
#include "../ProjectEuler.hpp"
#include "../SafeMath.hpp"

using FormattedOutput = Formatted<D, 5>;

static Optional<FormattedOutput> f(UI x, UI y, UI mod) {
    SafeMath_failFast = false;
    auto xy = SafeMath(x) * y;
    FAIL_IF(!(xy).ok());
    FAIL_IF(xy >= 1e9); // too big

    Mod::GlobalModSentinel sentinel(mod);
    
    Vec<Vec<Mod>> f_memoized(x + 1, Vec<Mod>(y + 1, 0));
    for (UI i = 0; i <= x; ++i) {
	for (UI j = 0; j <= y; ++j) {
	    if (i == 0 && j == 0) {
		f_memoized[i][j] = 0;
	    } else if (i == 0) {
		f_memoized[i][j] = 1;
	    } else if (j == 0) {
		f_memoized[i][j] = 2;
	    } else {
		f_memoized[i][j] = f_memoized[i - 1][j] + f_memoized[i][j - 1];
	    }
	}
    }
    auto modResult = f_memoized[x][y];
    return FormattedOutput(D(modResult) / 1e4);
}

static int demoMain(Argc const argc, Argv const argv) {
    B ran_ok = PE::run_problem(
	argc,
	argv,
	f,
	{"f", {"x", "y", "mod"}},
	{
	    {{0U, 5U, 0U}, 1e-4},
	    {{7U, 0U, 0U}, 2e-4},
	    {{2U, 2U, 0U}, 9e-4}
	},
	{800U, 321U, static_cast<UI>(1e9 + 7)});
    return !ran_ok;
}

// END OF DEMO SOLUTION

TEST(ProjectEuler, DemoNoArgs) {
    // no runtime args -> run with the question's true inputs
    // ./run.me
    char const* argv[] = {"./run.me"};
    Argc argc = sizeof(argv) / sizeof(char const*);

    auto [mainStatus, output] = captureStdOutAndRun([&]{ return demoMain(argc, argv); });
    
    CHECK(mainStatus, equals(0));
    CHECK(output, matches(std::regex(regexEscape("f(800,321,1000000007)") + ".*" + regexEscape("83709.22570") + "\n")));
}

TEST(ProjectEuler, DemoTestAll) {
    // "test all" -> run all tests
    // ./run.me test all
    char const* argv[] = {"./run.me", "test", "all"};
    Argc argc = sizeof(argv) / sizeof(char const*);

    auto [mainStatus, output] = captureStdOutAndRun([&]{ return demoMain(argc, argv); });
    
    CHECK(mainStatus, equals(0));
    
    std::string someCrap = "(.|\n)*?"; // matches anything including newlines
    CHECK(output, matches(std::regex(regexEscape("f(0,5,0) [expected 0.00010]") + ".*" + regexEscape("0.00010") + someCrap)));
    CHECK(output, matches(std::regex(someCrap + regexEscape("f(7,0,0) [expected 0.00020]") + ".*" + regexEscape("0.00020") + someCrap)));
    CHECK(output, matches(std::regex(someCrap + regexEscape("f(2,2,0) [expected 0.00090]") + ".*" + regexEscape("0.00090") + "\n")));
}

TEST(ProjectEuler, DemoTestFirst) {
    // "test 1" -> run test 1 (1-based)
    // ./run.me test 1
    char const* argv[] = {"./run.me", "test", "1"};
    Argc argc = sizeof(argv) / sizeof(char const*);

    auto [mainStatus, output] = captureStdOutAndRun([&]{ return demoMain(argc, argv); });
    
    CHECK(mainStatus, equals(0));
    CHECK(output, matches(std::regex(regexEscape("f(0,5,0) [expected 0.00010]") + ".*" + regexEscape("0.00010") + "\n")));
}

TEST(ProjectEuler, DemoTestLast) {
    // "test 3" -> run test 3 (1-based)
    // ./run.me test 3
    char const* argv[] = {"./run.me", "test", "3"};
    Argc argc = sizeof(argv) / sizeof(char const*);

    auto [mainStatus, output] = captureStdOutAndRun([&]{ return demoMain(argc, argv); });
    
    CHECK(mainStatus, equals(0));
    CHECK(output, matches(std::regex(regexEscape("f(2,2,0) [expected 0.00090]") + ".*" + regexEscape("0.00090") + "\n")));
}

TEST(ProjectEuler, RuntimeInput) {
    // "{x, y, mod}" -> run with that input
    // ./run.me { 0, 1000, 0 }
    char const* argv[] = {"./run.me", "{", "0,", "1000,", "0", "}"};
    Argc argc = sizeof(argv) / sizeof(char const*);

    auto [mainStatus, output] = captureStdOutAndRun([&]{ return demoMain(argc, argv); });
    
    CHECK(mainStatus, equals(0));
    CHECK(output, matches(std::regex(regexEscape("f(0,1000,0)") + ".*" + regexEscape("0.00010") + "\n")));
}

TEST(ProjectEuler, RuntimeInputMultiple) {
    // "{x, y, mod}" -> run with that input
    // can provide multiple {x1, y1, mod1} {x2, y2, mod2}
    // ./run.me { 0, 1000, 0 } {5,5,17}
    // Also parsing doesn't care about whitespace
    char const* argv[] = {"./run.me", "{", "0,", "1001,", "0", "}", "{5,5,17}"};
    Argc argc = sizeof(argv) / sizeof(char const*);

    auto [mainStatus, output] = captureStdOutAndRun([&]{ return demoMain(argc, argv); });
    
    CHECK(mainStatus, equals(0));
    std::string someCrap = "(.|\n)*?"; // matches anything including newlines
    CHECK(output, matches(std::regex(regexEscape("f(0,1001,0)") + ".*" + regexEscape("0.00010") + someCrap)));
    CHECK(output, matches(std::regex(someCrap + regexEscape("f(5,5,17)") + ".*" + regexEscape("0.00040") + "\n")));
}

TEST(ProjectEuler, RuntimeInputFailMultiply) {
    // If the Optional<Output> function returns failure, the harness should print that
    
    char const* argv[] = {"./run.me", "{50000000,5000000,17}"};
    Argc argc = sizeof(argv) / sizeof(char const*);

    auto [mainStatus, output] = captureStdOutAndRun([&]{ return demoMain(argc, argv); });
    
    CHECK(mainStatus, equals(1));
    std::string someCrap = "(.|\n)*?"; // matches anything including newlines
    CHECK(output, matches(std::regex(someCrap + regexEscape("!(xy).ok()") + someCrap)));
}

TEST(ProjectEuler, RuntimeInputFailTooBig) {
    // If the Optional<Output> function returns failure, the harness should print that
    
    char const* argv[] = {"./run.me", "{10000,100000,99}"};
    Argc argc = sizeof(argv) / sizeof(char const*);

    auto [mainStatus, output] = captureStdOutAndRun([&]{ return demoMain(argc, argv); });
    
    CHECK(mainStatus, equals(1));
    std::string someCrap = "(.|\n)*?"; // matches anything including newlines
    CHECK(output, matches(std::regex(someCrap + regexEscape("xy >= 1e9") + someCrap)));
}

