#pragma once

#include <chrono>

decltype(auto) inline now() {
    return std::chrono::system_clock::now();
}
