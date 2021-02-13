#include "printPixels.hpp"

#include <iostream>
#include <unistd.h>

static char code(Pixel const& pixel) {
    auto r = pixel.r;
    auto g = pixel.g;
    auto b = pixel.b;

    auto strongest = std::max({r, g, b});

    unsigned char threshold = 72;
    bool visible = strongest >= threshold;
    auto visibleRatio = 1.4;
    
    bool isR = visible && r >= strongest / visibleRatio;
    bool isG = visible && g >= strongest / visibleRatio;
    bool isB = visible && b >= strongest / visibleRatio;
    
    return '0' + isR + 2 * isG + 4 * isB;
}

void printPixels(std::vector<std::vector<Pixel>> const& pixels) {
    if (!isatty(fileno(stdout))) {
	std::cout << "This program only prints to a terminal." << std::endl;
        exit(1);
    }

    for (auto const& row : pixels) {
	for (auto const& pixel : row) {
	    char colorCode = code(pixel);
	    // changes terminal color then prints a space
	    std::cout << "\x1b[3" << colorCode << ";4" << colorCode << "m";
	    std::cout << " ";
	}
	std::cout << "\x1b[0m" << std::endl;
    }
}
