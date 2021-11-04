#include <stdio.h>
#include "MillerRabinPrime.c"

int main() {
    unsigned long huge = 100000000UL;
    unsigned long i;
    for (i = 0; i < 1000; i++) {
	if (isPrime(huge + i))
	{
	    printf("%lu\n",huge + i);
	}
    }
    return 0;
}
