#include "View.hpp"
#include "../Now.hpp"
#include "../TypeUtils.hpp"
#include "../Wait.hpp"

#include <iostream>

static auto localScreen() {
    return Screen::capture().toLocal();
}

SI main(Argc const argc, Argv const argv) {
    if (argc < 2) {
	return 1;
    }
    int seconds = atoi(argv[1]);
    if (!seconds) {
	return 1;
    }
    waitFor(milliseconds(3000));
    std::cout << "start" << std::endl;
    auto start = now();
    auto lastScreen = localScreen();
    std::vector<double> timeDiffs;
    double timeFromStart;
    do {
	timeFromStart = static_cast<Time>(now() - start).count();
	auto currentScreen = localScreen();
	if (lastScreen != currentScreen) {
	    timeDiffs.push_back(timeFromStart);
	} else {
	    timeDiffs.push_back(-timeFromStart);
	}
	std::swap(lastScreen, currentScreen);
    } while (timeFromStart < seconds);

    for (double d : timeDiffs) {
	std::cout << d << std::endl;
    }
    
    return 0;
}
