#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    auto cur = fs::current_path();
    std::cout << cur << std::endl;
    return 0;
}
