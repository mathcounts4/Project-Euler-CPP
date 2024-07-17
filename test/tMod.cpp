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

TEST(Mod, BigModMult) {
    UI max = std::numeric_limits<UI>::max();
    {
	Mod::GlobalModSentinel s(max);
	CHECK(Mod(-1) * Mod(-1), equals(Mod(1)));
	CHECK(Mod(-2) * Mod(-3), equals(Mod(6)));
    }
    {
	Mod::GlobalModSentinel s(max - 7);
	CHECK(Mod(-1) * Mod(-1), equals(Mod(1)));
	CHECK(Mod(-2) * Mod(-3), equals(Mod(6)));
    }
}

TEST(Mod, ChineseRemainderTheorem) {
    Mod x(3, 2); // 2 mod 3
    Mod y(7, 3); // 3 mod 7
    CHECK(*Mod::uniqueMerge({x, y}), equals(Mod(21, 17)));

    Mod a(6, 2); // 2 mod 6 -> 2 mod 3
    Mod b(45, 19); // 19 mod 45 -> 1 mod 3
    CHECK(static_cast<B>(Mod::uniqueMerge({a, b})), isFalse());

    Mod c(1U << 28, 1); // 2^28 * 3 * 5 * 7 ≥ 2^32
    Mod d(3U, 0);
    Mod e(5U, 0);
    Mod f(7U, 0);
    CHECK(static_cast<B>(Mod::uniqueMerge({c, d, e, f})), isFalse());

    // 1 mod 2
    // 1 mod 7
    // 3 mod 5
    // -> 43 mod 70
    Mod g(2, 1);
    Mod h(7, 1);
    Mod i(5, 3);
    CHECK(*Mod::uniqueMerge({g, h, i}), equals(Mod(70, 43)));
    Mod j(14, 1);
    Mod k(10, 3);
    Mod l(35, 8);
    CHECK(*Mod::uniqueMerge({j, k, l}), equals(Mod(70, 43)));
    Mod m(70, 43);
    CHECK(*Mod::uniqueMerge({j, k, l, m}), equals(Mod(70, 43)));
    Mod n(0, 113);
    CHECK(*Mod::uniqueMerge({j, k, l, n}), equals(Mod(0, 113)));

    // Degenerate case - no inputs -> 0 mod 1, since all integers are solutions
    CHECK(*Mod::uniqueMerge({}), equals(Mod(1, 0)));

    // 50 and 51 -> failure
    Mod o(0, 50);
    Mod p(0, 51);
    CHECK(static_cast<B>(Mod::uniqueMerge({o, p})), isFalse());

    // 50 and 7 mod 10 -> failure
    Mod q(0, 50);
    Mod r(10, 7);
    CHECK(static_cast<B>(Mod::uniqueMerge({q, r})), isFalse());
}
