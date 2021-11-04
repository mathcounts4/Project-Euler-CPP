#include <iostream>
#include "Mod.hpp"

using namespace std;

int main()
{
    const unsigned int mod = 753;

    typedef Mod<mod> T;

    T x(1);
    T y = x;
    T z = -x;
    z+x;
    cout << x << endl;
    cout << (z+(-x)) << endl;
    
    return 0;
}
