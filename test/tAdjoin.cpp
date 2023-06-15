#include "../Adjoin.hpp"
#include "../Mod.hpp"
#include "harness.hpp"

TEST(AdjoinSqrt, Construction) {
    AdjoinSqrt<SI> x{1, 3, 2};
    CHECK(x.getA(), equals(1));
    CHECK(x.getB(), equals(3));
    CHECK(x.getK(), equals(2));
}

TEST(AdjoinSqrt, Eq) {
    AdjoinSqrt<SI> x{1, 3, 2};
    AdjoinSqrt<SI> y{1, 3, 2};
    AdjoinSqrt<SI> z{1, 4, 2};
    AdjoinSqrt<SI> q{2, 3, 2};

    CHECK(x, equals(y));
    CHECK(y, equals(x));
    CHECK(x, isNotEqualTo(z));
    CHECK(z, isNotEqualTo(x));
    CHECK(x, isNotEqualTo(q));
    CHECK(q, isNotEqualTo(x));
}

TEST(AdjoinSqrt, PlusMinusMultiply) {
    AdjoinSqrt<SI> x{1, 3, 17};
    AdjoinSqrt<SI> y{2, 5, 17};
    
    AdjoinSqrt<SI> xplusy{1+2, 3+5, 17};
    AdjoinSqrt<SI> xminusy{1-2, 3-5, 17};
    AdjoinSqrt<SI> xtimesy{1*2+3*5*17, 1*5+3*2, 17};

    CHECK(x+y, equals(xplusy));
    CHECK(x-y, equals(xminusy));
    CHECK(x*y, equals(xtimesy));

    auto xpluseqy = x;
    auto xminuseqy = x;
    auto xtimeseqy = x;
    
    xpluseqy += y;
    xminuseqy -= y;
    xtimeseqy *= y;

    CHECK(xpluseqy, equals(xplusy));
    CHECK(xminuseqy, equals(xminusy));
    CHECK(xtimeseqy, equals(xtimesy));
}

TEST(AdjoinSqrt, Power) {
    AdjoinSqrt<SI> x{1, 3, 17};
    AdjoinSqrt<SI> one{1, 0, 17};

    CHECK(x ^ 0, equals(one));
    CHECK(x ^ 1, equals(x));
    CHECK(x ^ 2, equals(x * x));
    CHECK(x ^ 3, equals(x * x * x));
    CHECK(x ^ 4, equals(x * x * x * x));
    CHECK(x ^ 5, equals(x * x * x * x * x));
}

TEST(AdjoinSqrt, Constants) {
    AdjoinSqrt<SI> x{1, 3, 17};
    AdjoinSqrt<SI> zeroVal{0, 0, 17};
    AdjoinSqrt<SI> oneVal{1, 0, 17};

    CHECK(zero(x), equals(zeroVal));
    CHECK(one(x), equals(oneVal));
}

TEST(AdjoinSqrt, Mod) {
    Mod::GlobalModSentinel sentinel(23);
    
    AdjoinSqrt<Mod> x{1, 3, 5};
    AdjoinSqrt<Mod> xplusx{1 + 1, 3 + 3, 5};
    AdjoinSqrt<Mod> xx{1*1+3*3*5, 1*3*2, 5};

    CHECK(x + x, equals(xplusx));
    CHECK(x * x, equals(xx));
    CHECK(x ^ 2, equals(xx));
}
