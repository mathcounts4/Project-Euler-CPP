const unsigned long MAX_UNSIGNED_LONG = ~(0UL); // = 2^64 - 1
const unsigned long BIG_SHIFT = 31UL; // 31
const unsigned long BIG_MOD = 1UL << BIG_SHIFT; // 2^31
const unsigned long DO_BIG_MOD = BIG_MOD - 1UL; // 00...0011..11 (31 1's)
#define small_part(x) ((x) & DO_BIG_MOD) // x % 2^31
#define big_part(x) ((x) >> BIG_SHIFT) // x / 2^31
#define make_big(x) ((x) << BIG_SHIFT) // x * 2^31
// note that x = big_part(x) * BIG_MOD + small_part(x)
// note that x = make_big(big_part(x)) + small_part(x)

const unsigned long HUGE_SHIFT = BIG_SHIFT << 1UL; // 62
const unsigned long HUGE_MOD = 1UL << HUGE_SHIFT; // 2^62
const unsigned long DO_HUGE_MOD = HUGE_MOD - 1UL; // 00..0011..11 (62 1's)
#define tiny_part(x) ((x) & DO_HUGE_MOD) // x % 2^62
#define huge_part(x) ((x) >> HUGE_SHIFT) // x / 2^62
// note that x = huge_part(x) * HUGE_MOD + tiny_part(x)

#define digit(x,k) (((x) >> k) & 1UL)

// We require BIG_MOD that satisfies
// (1) N <= BIG_MOD^2
// (2) 2 * (BIG_MOD - 1)^2 < MAX_UNSIGNED_LONG
// We require A and B that satisfy
// (3) 0 <= A < N
// (4) 0 <= B < N
unsigned long multMod(unsigned long A, unsigned long B, unsigned long N) {
    if (A == 0UL) {
	return 0UL;
    }
    else if (MAX_UNSIGNED_LONG / A > B) {
	return (A * B) % N;
    }
    else
    {
        unsigned long a1 = big_part(A);
	unsigned long a2 = small_part(A);
	unsigned long b1 = big_part(B);
	unsigned long b2 = small_part(B);

	// A * B = (a1 * b1) BIG_MOD^2 + (a1 * b2 + a2 * b1) BIG_MOD + a2 * b2
	
	unsigned long C = a1 * b1;
	unsigned long D = a1 * b2 + a2 * b1;
	unsigned long E = a2 * b2;

	// A * B = C BIG_MOD^2 + D BIG_MOD + E
	// C,E <= (BIG_MOD - 1)^2
	// D <= 2 * (BIG_MOD - 1)^2

	unsigned long small = make_big(small_part(D)) + E;
	// small <= BIG_MOD * (BIG_MOD - 1) + (BIG_MOD - 1)^2
	// small < 2 * BIG_MOD^2
	unsigned long big = C + big_part(D) + huge_part(small);
	// huge_part(small) < 2
	// big_part(D) <= BIG_MOD - 2
	// big < BIG_MOD^2
	small = tiny_part(small);
	// small < BIG_MOD^2

	// big * HUGE_MOD + small =
	
	// C * BIG_MOD^2 + big_part(D) * BIG_MOD * BIG_MOD +
	// huge_part(small) * BIG_MOD + small =
	
	// C * BIG_MOD^2 + big_part(D) * BIG_MOD * BIG_MOD + small =

	// C * BIG_MOD^2 + big_part(D) * BIG_MOD * BIG_MOD +
	// small_part(D) * BIG_MOD + E =

	// C * BIG_MOD^2 + D * BIG_MOD + E =

	// A * B

	
	// so we've written A * B in base HUGE_MOD as (big)(small)
	// time to actuall do some modular arithmetic:
	// see https://en.wikipedia.org/wiki/Division_algorithm
	long shift;
	unsigned long r = 0;
	for (shift = HUGE_SHIFT - 1UL; shift >= 0; shift--)
	{
	    r = (r << 1UL) | digit(big,shift);
	    if (r >= N)
	    {
		r -= N;
	    }
	}
	for (shift = HUGE_SHIFT - 1UL; shift >= 0; shift--)
	{
	    r = (r << 1UL) | digit(small,shift);
	    if (r >= N)
	    {
		r -= N;
	    }
	}

	return r;
    }
}
