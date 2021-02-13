#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "MultMod.c"

// returns 0 if x fails the Miller-Rabin compositeness test (iter) times or x is a small prime
// returns the witness a (2 <= a <= x-2) if it passes a compositeness test
// If x has a small prime factor, 1 is returned
unsigned long primeWithIter(unsigned long x, unsigned int iter) {
    if (x == 2 ||
	x == 3 ||
	x == 5 ||
	x == 7) {
	return 0UL;
    } else if (x % 2 == 0 ||
	       x % 3 == 0 ||
	       x % 5 == 0 ||
	       x % 7 == 0) {
	return 1UL; // special cases, return 1
    } else
    {
	int i;
	// (iter) iterations of Miller-Rabin compositeness test
	for (i = 0; i < iter; i++)
	{
	    // avoid 0 and 1,-1 because they'll always pass the test
	    unsigned long a = (rand() % (x-3)) + 2;
	    unsigned long powA = a;
	    int pow2 = 0;
	    unsigned long powLeft = x-1UL;
	    while (powLeft % 2 == 0)
	    {
		powLeft /= 2;
		pow2++;
	    }
	    unsigned long product = 1;
	    while (powLeft > 0)
	    {
		if (powLeft % 2 == 1)
		{
		    product = multMod(product,powA,x);
		}
		powA = multMod(powA,powA,x);
		powLeft /= 2;
	    }
	    if (product != 1UL && product + 1UL != x) // if +-1, compositenes test fails
	    {
		// if not +-1, we must obtain -1
		int pass = 0;
		while (pow2 > 0)
		{
		    product = multMod(product,product,x);
		    if (product + 1UL == x)
		    {
			// got -1, test passes (fails compositeness)
			// exit this test iteration with success
			pow2 = 1;
			pass = 1;
		    }
		    pow2--;
		}
		if (!pass)
		{
		    return a;
		}
	    }
	}
	// failed the Miller-Rabin compositeness test (iter) times, assume prime
	return 0;
    }
}

// default 10 iterations
unsigned long prime(unsigned long x) {
    return primeWithIter(x,10);
}

int isPrimeWithIter(unsigned long x, unsigned int iter) {
    return (primeWithIter(x,iter) == 0);
}

int isPrime(unsigned long x) {
    return (prime(x) == 0);
}

void initializeMillerRabin() {
    srand(time(NULL));
}
