#include "../BigInt.hpp"
#include "harness.hpp"

constexpr UL bigUL = 12345678912345UL;

BigInt const zero;
BigInt const three(3LL);
BigInt const negativeFive(-5);
BigInt const big(bigUL);
BigInt const super("000111222333444555666777888999");
BigInt const inf = BigInt::Inf();
BigInt const negInf = BigInt::NegInf();

TEST(BigInt,Construction) {
    CHECK(zero, equals(0));
    CHECK(negativeFive, equals(-5L));
    CHECK(big, equals(bigUL));
}

TEST(BigInt,Printing) {
    CHECK(to_string(zero), equals("0"));
    CHECK(to_string(negativeFive), equals("-5"));
    CHECK(to_string(big), equals(to_string(bigUL)));
    CHECK(to_string(super), equals("111222333444555666777888999"));
    CHECK(to_string(inf), equals(BigInt::INF_STR));
    CHECK(to_string(negInf), equals(BigInt::NEG_INF_STR));
}

TEST(BigInt,Properties) {
    CHECK(zero.neg(), isFalse());
    CHECK(zero.inf(), isFalse());
    CHECK(three.neg(), isFalse());
    CHECK(three.inf(), isFalse());
    CHECK(negativeFive.neg(), isTrue());
    CHECK(negativeFive.inf(), isFalse());
    CHECK(big.neg(), isFalse());
    CHECK(big.inf(), isFalse());
    CHECK(inf.neg(), isFalse());
    CHECK(inf.inf(), isTrue());
    CHECK(negInf.neg(), isTrue());
    CHECK(negInf.inf(), isTrue());
}

TEST(BigInt,Conversion) {
    CHECK(*zero.convert<char>(), equals('\0'));
    CHECK(*zero.convert<long>(), equals(0L));
    CHECK(*negativeFive.convert<int>(), equals(-5));
    CHECK(*big.convert<UL>(), equals(bigUL));
    CHECK(*BigInt{-128}.convert<char>(), equals(-128));
    CHECK(BigInt{-129}.convert<char>(), isFalse());
    CHECK(BigInt{128}.convert<char>(), isFalse());
    CHECK(big.convert<int>(), isFalse());
    CHECK(to_string(*inf.convert<double>()), equals("inf"));
    CHECK(to_string(*negInf.convert<double>()), equals("-inf"));
}

TEST(BigInt,Comparison) {
    CHECK(zero <= zero, isTrue());
    CHECK(zero < negativeFive, isFalse());
    CHECK(zero > negInf, isTrue());
    CHECK(three < inf, isTrue());
    CHECK(inf <= negInf, isFalse());
    CHECK(inf > negInf, isTrue());
    CHECK(zero == three, isFalse());
    CHECK(inf == negInf, isFalse());
    CHECK(inf == BigInt::Inf(), isTrue());
    CHECK(three != three, isFalse());
    CHECK(big > three, isTrue());
    CHECK(-big < negativeFive, isTrue());
    CHECK(-5 == negativeFive, isTrue());
    CHECK(BigInt("34879468572945867495687439586") > big, isTrue());
    CHECK(BigInt("-9843349435609874564454565587") < big, isTrue());
    CHECK(BigInt("4358762345876324589763485762485764587645876342857346328457634587456874365") < inf, isTrue());
    CHECK(BigInt("-4358762345876324589763485762485764587645876342857346328457634587456874365") > -inf, isTrue());
}

TEST(BigInt,Arithmetic) {
    // these call into the modifying versions,
    // so we need only test one set.
    
    // unary -
    CHECK(-three, equals(-3));
    CHECK(-negativeFive, equals(5));
    CHECK(-negInf, equals(inf));
    CHECK(-inf, equals(negInf));

    // +
    CHECK(zero + big, equals(bigUL));
    CHECK(three + negativeFive, equals(-2));
    CHECK(inf + big, equals(inf));
    CHECK(negInf + big, equals(negInf));
    CHECK(BigInt(3652107375UL)+BigInt(2400000073UL), equals(3652107375UL+2400000073UL));

    // -
    CHECK(three - negativeFive, equals(8));
    CHECK(negativeFive - three, equals(-8));
    CHECK(negInf - big, equals(negInf));

    // *
    CHECK(three * three, equals(9));
    CHECK(negativeFive * three, equals(-15));
    CHECK(three * negativeFive, equals(-15));
    CHECK(negativeFive * negativeFive, equals(25));
    CHECK(negInf * three, equals(negInf));
    CHECK(negInf * negativeFive, equals(inf));
    CHECK(inf * negativeFive, equals(negInf));

    // /
    CHECK(three / negativeFive, equals(0));
    CHECK(negativeFive / three, equals(-1));
    CHECK(negativeFive / 2, equals(-2));
    CHECK(big / 7, equals(bigUL/7));
    CHECK(inf / three, equals(inf));
    CHECK(inf / negativeFive, equals(negInf));
    CHECK(negInf / three, equals(negInf));
    CHECK(negInf / negativeFive, equals(inf));

    // %
    CHECK(zero % negativeFive, equals(0));
    CHECK(zero % three, equals(0));
    CHECK(zero % negInf, equals(0));
    CHECK(three % negativeFive, equals(3));
    CHECK(negativeFive % three, equals(-2));
    CHECK(negativeFive % 2, equals(-1));
    CHECK(big % 7, equals(bigUL%7));
    CHECK(big % -7, equals(static_cast<long>(bigUL)%-7));
    CHECK(negativeFive % inf, equals(-5));
    CHECK(negativeFive % negInf, equals(-5));

    // <<
    CHECK(zero << 3, equals(0));
    CHECK(zero << -3, equals(0));
    CHECK(negativeFive << 2, equals(-20));
    CHECK(negativeFive << -2, equals(-1));
    CHECK(big << -8, equals(bigUL >> 8));
    CHECK(inf << 2253, equals(inf));
    CHECK(negInf << -345876, equals(negInf));

    // >>
    CHECK(zero >> 3, equals(0));
    CHECK(zero >> -3, equals(0));
    CHECK(negativeFive >> 1, equals(-2));
    CHECK(negativeFive >> 3, equals(0));
    CHECK(negativeFive >> 4357896, equals(0));
    CHECK(big >> 5, equals(bigUL >> 5));
    CHECK(inf >> 2253, equals(inf));
    CHECK(negInf >> -345876, equals(negInf));
}

TEST(BigInt,ModifyingArithmeticArgSelf) {
    // +=
    BigInt addPos(3);
    addPos += addPos;
    CHECK(addPos, equals(6));
    BigInt addNeg(-3);
    addNeg += addNeg;
    CHECK(addNeg, equals(-6));

    // -=
    BigInt subPos(3);
    subPos -= subPos;
    CHECK(subPos, equals(0));
    BigInt subNeg(-3);
    subNeg -= subNeg;
    CHECK(subNeg, equals(0));

    // *=
    BigInt multPos(3);
    multPos *= multPos;
    CHECK(multPos, equals(9));
    BigInt multNeg(-3);
    multNeg *= multNeg;
    CHECK(multNeg, equals(9));

    // /=
    BigInt divPos(3);
    divPos /= divPos;
    CHECK(divPos, equals(1));
    BigInt divNeg(-3);
    divNeg /= divNeg;
    CHECK(divNeg, equals(1));

    // %=
    BigInt modPos(3);
    modPos %= modPos;
    CHECK(modPos, equals(0));
    BigInt modNeg(-3);
    modNeg %= modNeg;
    CHECK(modNeg, equals(0));
}
