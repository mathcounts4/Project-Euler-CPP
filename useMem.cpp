#include <iostream>

int main() {
    auto n = 14000000000UL;
    volatile char* x = new char[n]();
    for (auto i = 0UL; i < n; i += 1000) {
        ++x[i ^ (i%1111 == 7)];
        if (i%100000000 == 0) std::cout << i << std::endl;
    }
    return *x > 1;
}
