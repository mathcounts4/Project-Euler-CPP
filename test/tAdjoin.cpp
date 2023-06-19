#include "../Adjoin.hpp"
#include "../Mod.hpp"
#include "harness.hpp"

TEST(AdjoinSqrt, Construction) {
    AdjoinSqrt<SI> x{2, {1, 3}};
    CHECK(x.getValues()[0], equals(1));
    CHECK(x.getValues()[1], equals(3));
    CHECK(x.getK(), equals(2));
}

TEST(AdjoinSqrt, Eq) {
    AdjoinSqrt<SI> x{2, {1, 3}};
    AdjoinSqrt<SI> y{2, {1, 3}};
    AdjoinSqrt<SI> z{2, {1, 4}};
    AdjoinSqrt<SI> q{2, {2, 3}};

    CHECK(x, equals(y));
    CHECK(y, equals(x));
    CHECK(x, isNotEqualTo(z));
    CHECK(z, isNotEqualTo(x));
    CHECK(x, isNotEqualTo(q));
    CHECK(q, isNotEqualTo(x));
}

TEST(AdjoinSqrt, PlusMinusMultiply) {
    AdjoinSqrt<SI> x{17, {1, 3}};
    AdjoinSqrt<SI> y{17, {2, 5}};
    
    AdjoinSqrt<SI> xplusy{17, {1+2, 3+5}};
    AdjoinSqrt<SI> xminusy{17, {1-2, 3-5}};
    AdjoinSqrt<SI> xtimesy{17, {1*2+3*5*17, 1*5+3*2}};

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

TEST(Adjoin, Divide) {
    Adjoin<2, SI> x{3, {5, 6}};
    Adjoin<2, SI> y{3, {2, 1}}; // 1/(2+√3) = 2-√3
    Adjoin<2, SI> xdivy{3, {5*2-6*1*3, 5*-1+6*2}};
    CHECK(x/y, equals(xdivy));

    Adjoin<2, SI> a{3, {4, 2}};
    Adjoin<2, SI> b{3, {5, 3}}; // 1/(5+3√3) = (5-3√3)/-2
    Adjoin<2, SI> adivb{3, {(4*5+2*-3*3)/-2, (4*-3+2*5)/-2}};
    CHECK(a/b, equals(adivb));

    Adjoin<2, UI> u{3, {14, 35}};
    Adjoin<2, UI> u7{3, {14/7, 35/7}};
    CHECK(u / 7, equals(u7));

    Adjoin<3, UI> u3{3, {14, 35, 70}};
    Adjoin<3, UI> u37{3, {14/7, 35/7, 70/7}};
    CHECK(u3 / 7, equals(u37));
}

TEST(AdjoinSqrt, Power) {
    AdjoinSqrt<SI> x{17, {1, 3}};
    AdjoinSqrt<SI> one{17, {1, 0}};

    CHECK(x ^ 0, equals(one));
    CHECK(x ^ 1, equals(x));
    CHECK(x ^ 2, equals(x * x));
    CHECK(x ^ 3, equals(x * x * x));
    CHECK(x ^ 4, equals(x * x * x * x));
    CHECK(x ^ 5, equals(x * x * x * x * x));
}

TEST(AdjoinSqrt, Constants) {
    AdjoinSqrt<SI> x{17, {1, 3}};
    AdjoinSqrt<SI> zeroVal{17, {0, 0}};
    AdjoinSqrt<SI> oneVal{17, {1, 0}};

    CHECK(zero(x), equals(zeroVal));
    CHECK(one(x), equals(oneVal));
}

TEST(AdjoinSqrt, Mod) {
    Mod::GlobalModSentinel sentinel(23);
    
    AdjoinSqrt<Mod> x{5, {1, 3}};
    AdjoinSqrt<Mod> xplusx{5, {1 + 1, 3 + 3}};
    AdjoinSqrt<Mod> xx{5, {1*1+3*3*5, 1*3*2}};

    CHECK(x + x, equals(xplusx));
    CHECK(x * x, equals(xx));
    CHECK(x ^ 2, equals(xx));
}

TEST(AdjoinCubeRt, Multiply) {
    Adjoin<3, SI> x{17, {1, 3, 4}};
    Adjoin<3, SI> y{17, {2, 5, 7}};
    
    Adjoin<3, SI> xtimesy{17, {1*2+3*7*17+4*5*17, 1*5+3*2+4*7*17, 1*7+3*5+4*2}};
    
    CHECK(x*y, equals(xtimesy));
}
