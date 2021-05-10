#include "OSX_ApplicationServices.hpp"
#include "View.hpp"
#include "../TypeUtils.hpp"
#include "../Wait.hpp"

#include <iostream>
#include <unistd.h>

static C code(Pixel const& pixel) {
    auto r = pixel.r;
    auto g = pixel.g;
    auto b = pixel.b;

    auto strongest = std::max({r, g, b});

    UC threshold = 72;
    B visible = strongest >= threshold;
    auto visibleRatio = 1.4;
    
    B isR = visible && r >= strongest / visibleRatio;
    B isG = visible && g >= strongest / visibleRatio;
    B isB = visible && b >= strongest / visibleRatio;
    
    return '0' + isR + 2 * isG + 4 * isB;
}

SI main(Argc const, Argv const) {
    if (!isatty(fileno(stdout))) {
	std::cout << "This program only prints to a terminal." << std::endl;
	return 1;
    }
    std::cout << "display " << CGMainDisplayID() << std::endl;
    std::cout << "Waiting for 5 seconds" << std::endl;
    waitFor(Time(5));
    std::cout << "display " << CGMainDisplayID() << std::endl;
    std::cout << "Snap!" << std::endl;
    auto screenshot = Screen::capture();
    for (SZ i = 0; i < 100; ++i) {
	for (SZ j = 0; j < 200; ++j) {
	    auto pixel = screenshot.at(j, i);
	    char colorCode = code(pixel);
	    // changes terminal text color and background color then prints a space
	    std::cout << "\x1b[3" << colorCode << ";4" << colorCode << "m";
	    std::cout << " ";
	}
	std::cout << "\x1b[0m" << std::endl;
    }
    return 0;
}

