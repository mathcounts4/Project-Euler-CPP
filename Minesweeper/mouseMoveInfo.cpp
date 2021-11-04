#include "MouseController.hpp"
#include "../Now.hpp"
#include "../TypeUtils.hpp"

#include <iostream>
#include <chrono>
#include <thread> // std::this_thread::sleep_for

SI main(Argc const, Argv const) {
    std::cout << "starting" << std::endl;
    auto start = now();
    constexpr int n = 1000;
    double times[n];
    UI xs[n];
    UI ys[n];
    for (int i = 0; i < n; ++i) {
	auto [x, y] = Mouse::position();
	auto current = now();
	std::chrono::duration<double> diff = current - start;
	times[i] = diff.count();
	xs[i] = x;
	ys[i] = y;
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    std::cout << "[";
    for (int i = 0; i < n; ++i) {
	if (i > 0) {
	    std::cout << ";";
	}
	std::cout << times[i] << "," << xs[i] << "," << ys[i];
    }
    std::cout << "]" << std::endl;
	
    return 0;
}
