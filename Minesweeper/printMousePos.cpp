#include "MouseController.hpp"
#include "../TypeUtils.hpp"
#include "../Wait.hpp"

#include <iostream>

SI main(Argc const, Argv const) {
    for (int i = 0; i < 10; ++i) {
	waitFor(std::chrono::milliseconds(500));
	auto [x, y] = Mouse::position();
	std::cout << x << " " << y << std::endl;
    }
    Mouse::realisticMove({2000, 2000});
    auto [x, y] = Mouse::position();
    std::cout << x << " " << y << std::endl;
    return 0;
}
