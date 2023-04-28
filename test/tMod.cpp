#include "../Mod.hpp"
#include "harness.hpp"

TEST(Mod, Order) {
    {
	Mod::GlobalModSentinel s(7);
	CHECK(Mod(3) < Mod(4), isTrue());
	CHECK(Mod(4) < Mod(3), isFalse());
    
	CHECK(Mod(3) < Mod(3), isFalse());
    
	CHECK(Mod(0) < Mod(3), isTrue());
	CHECK(Mod(3) < Mod(0), isFalse());
    
	CHECK(Mod(5) < Mod(8), isFalse());
	CHECK(Mod(8) < Mod(5), isTrue());
    }
    {
	CHECK(Mod(3) < Mod(4), isTrue());
	CHECK(Mod(4) < Mod(3), isFalse());
    
	CHECK(Mod(3) < Mod(3), isFalse());
    
	CHECK(Mod(0) < Mod(3), isTrue());
	CHECK(Mod(3) < Mod(0), isFalse());
    
	CHECK(Mod(5) < Mod(8), isTrue());
	CHECK(Mod(8) < Mod(5), isFalse());
    }
}
