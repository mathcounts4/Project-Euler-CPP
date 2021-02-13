#include "../Zip2.hpp"
#include "harness.hpp"

#include "../Compiles.hpp"
#include "../Vec.hpp"

#include <utility>

/* filtered pending https://bugs.llvm.org/show_bug.cgi?id=42220
TEST(zip,BindTemp) {
    struct IncrementerSentinel : public Vec<int> {
	int& the_int;
	IncrementerSentinel(int& x)
	    : Vec<int>{1,2,3}
	    , the_int(x) {
	    ++the_int;
	}
	IncrementerSentinel(IncrementerSentinel const &) = delete;
	~IncrementerSentinel() {
	    --the_int;
	}
    };
    int alive_temporaries = 0;
    bool loop_hit = false;
    for (auto&& [x,y] : zip(IncrementerSentinel(alive_temporaries),
			    IncrementerSentinel(alive_temporaries))) {
	(void) x;
	(void) y;
	loop_hit = true;
	CHECK(alive_temporaries, equals(2));
    }
    CHECK(loop_hit, isTrue());
    CHECK(alive_temporaries, equals(0));
}
*/

TEST(zip,Modify) {
    Vec<int> xList{3,4,5};
    Vec<int> yList{6,7,8};
    for (auto&& [x,y] : zip(xList,yList))
	std::swap(x,y);
    CHECK(xList, equals(Vec<int>{6,7,8}));
    CHECK(yList, equals(Vec<int>{3,4,5}));
}

TEST(zip,Const) {
    struct Bomb {
	bool exploded;
	Bomb() : exploded(false) {}
	Bomb(Bomb const &) = delete;
	void ignite() {
	    exploded = true;
	}
	void ignite() const {}
    };
    Vec<Bomb> xList(2);
    Vec<Bomb> yList(2);
    for (auto const & [x,y] : zip(xList,yList)) {
	x.ignite();
	y.ignite();
    }
    for (Bomb const & b : xList)
	CHECK(b.exploded, isFalse());
    for (Bomb const & b : yList)
	CHECK(b.exploded, isFalse());

    {
	// some code to ensure all Bomb's members functions are called
	// to make clang happy
	Bomb a;
	a.ignite();
	Bomb const b;
	b.ignite();
    }
}

TEST(zip,Many) {
    Vec<int> v{3,4,5};
    for (auto& [a,b,c,d,e] : zip(v,v,v,v,v)) {
	++a;
	++b;
	++c;
	++d;
	++e;
    }
    CHECK(v, equals(Vec<int>{8,9,10}));
}

