#include "harness.hpp"

#include "../Now.hpp"
#include "../Str.hpp"
#include "../TypeUtils.hpp"
#include "../Vec.hpp"

#include <iostream>

B global_success = false;

struct Test {
    enum class Result { Terminated, Failed, Passed };
    
    void (*function)();
    Str name;
    Str file;
    UI line;
    
    friend B operator<(Test const& x, Test const& y) {
	if (x.name != y.name)
	    return x.name < y.name;
	if (x.file != y.file)
	    return x.file < y.file;
	if (x.line != y.line)
	    return x.line < y.line;
	return x.function < y.function;
    }
};

static auto& testList() {
    // This returns a reference to be safely used during program init.
    // Regular global variables have init order rules to worry about
    static Vec<Test> list;
    return list;
}
SI registerTest(void(*function)(), C const* test, C const* file, UI line) {
    testList().push_back({function,test, file, line});
    return 0;
}

static Test::Result runTest(Test const& test) noexcept {
    try {
	global_success = true;
	test.function();
    } catch (std::exception const & ex) {
	std::cout << "ERROR: " << test.name << " terminated due to exception: " << ex.what() << std::endl;
	return Test::Result::Terminated;
    } catch (...) {
	std::cout << "ERROR: " << test.name << "] terminated due to unknown exception" << std::endl;
	return Test::Result::Terminated;
    }
    if (global_success)
	return Test::Result::Passed;
    return Test::Result::Failed;
}

static auto runTestWithTiming(Test const& test) noexcept {
    auto start = now();
    auto result = runTest(test);
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(now() - start).count() / 1e9 * 1e3; // -> ms
    return std::make_pair(result, duration);
}

TrueFunctor isTrue() {
    return {};
}

FalseFunctor isFalse() {
    return {};
}

SI main() {
    UI completed = 0;
    UI passed = 0;
    UI total = 0;
    Vec<Str> failed;
    Vec<std::pair<Str, D>> slow;
    
    std::sort(testList().begin(), testList().end());
    for (Test const& test : testList()) {
	++total;
	std::cout << test.name << " starting..." << std::endl;
	auto [result, time] = runTestWithTiming(test);
	switch (result) {
	case Test::Result::Terminated:
	    failed.push_back(test.name + " (unfinished)");
	    std::cout << test.name << " terminated." << "[" << time << " milliseconds]" << std::endl;
	    break;
	case Test::Result::Failed:
	    ++completed;
	    failed.push_back(test.name);
	    std::cout << test.name << " failed." << "[" << time << " milliseconds]" << std::endl;
	    break;
	case Test::Result::Passed:
	    ++completed;
	    ++passed;
	    std::cout << test.name << " passed." << "[" << time << " milliseconds]" << std::endl;
	    break;
	}
	if (time > 100) {
	    slow.push_back({test.name, time});
	}
    }
    
    std::cout << std::endl;
    std::cout << completed << "/" << total << " tests completed." << std::endl;
    std::cout << passed << "/" << total << " tests passed." << std::endl;
    std::cout << std::endl;
    if (!slow.empty()) {
	std::cout << "Slow tests:" << std::endl;
	for (auto const& [name, ms] : slow)
	    std::cout << name << ": " << ms << " milliseconds" << std::endl;
	std::cout << std::endl;
    }
    if (!failed.empty()) {
	std::cout << "Failed tests:" << std::endl;
	for (auto const& name : failed)
	    std::cout << name << std::endl;
	std::cout << std::endl;
    }
    return passed != total;
}
    
