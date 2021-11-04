#include "BigInt.hpp"
#include <iostream>

using namespace std;

int main() {
	BigInt<> x;
	BigInt<> y(7003);
	cout << (x-=7) << endl;
	cout << x*5 << endl;
	cout << x+y << endl;
	cout << x+-7L << endl;
	cout << 8.0+y << endl;
	cout << (x*=-y) << endl;
}
