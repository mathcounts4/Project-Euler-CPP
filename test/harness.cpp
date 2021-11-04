#include "harness.hpp"

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
    
    B operator<(Test const & other) const {
	if (file != other.file)
	    return file < other.file;
	if (line != other.line)
	    return line < other.line;
	if (name != other.name)
	    return name < other.name;
	return function < other.function;
    }
};

static auto& testList() {
    // This returns a reference to be safely used during program init.
    // Regular global variables have init order rules to worry about
    static Vec<Test> list;
    return list;
}
SI registerTest(void(*function)(), C const * test, C const * file, UI line) {
    testList().push_back({function,test,file,line});
    return 0;
}

inline Test::Result runTest(Test const & test) noexcept {
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
    
    std::sort(testList().begin(),testList().end());
    for (Test const & test : testList()) {
	++total;
	std::cout << test.name << " starting..." << std::endl;
	switch (runTest(test)) {
	case Test::Result::Terminated:
	    failed.push_back(test.name + " (unfinished)");
	    std::cout << test.name << " terminated." << std::endl;
	    break;
	case Test::Result::Failed:
	    ++completed;
	    failed.push_back(test.name);
	    std::cout << test.name << " failed." << std::endl;
	    break;
	case Test::Result::Passed:
	    ++completed;
	    ++passed;
	    std::cout << test.name << " passed." << std::endl;
	    break;
	}
    }
    
    std::cout << std::endl;
    std::cout << completed << "/" << total << " tests completed." << std::endl;
    std::cout << passed << "/" << total << " tests passed." << std::endl;
    std::cout << std::endl;
    if (failed.size()) {
	std::cout << "Failed tests:" << std::endl;
	for (auto const & name : failed)
	    std::cout << name << std::endl;
	std::cout << std::endl;
    }
    return passed != total;
}
    
