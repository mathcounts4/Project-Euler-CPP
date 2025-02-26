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
    CHECK(BigInt("-5"), equals(BigInt(-5)));
    CHECK(BigInt("5"), equals(BigInt(5)));
    CHECK(BigInt("0"), equals(BigInt(0)));
    CHECK(BigInt("-0"), equals(BigInt(0)));
    CHECK(BigInt("0xFF"), equals(BigInt(255)));
    CHECK(BigInt("0Xa0A"), equals(BigInt(2570)));
    CHECK(BigInt("0o77"), equals(BigInt(63)));
    CHECK(BigInt("-0O00012"), equals(BigInt(-10)));
    CHECK(BigInt("0b00101"), equals(BigInt(5)));
    CHECK(BigInt("-0B0000"), equals(BigInt(0)));
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
    CHECK(BigInt("12345678900000000000000000000") / (BigInt(10) ^ 20), equals(123456789U));
    CHECK(BigInt("12345678999999999999999999999") / (BigInt(10) ^ 20), equals(123456789U));
    CHECK(BigInt("-12345678900000000000000000000") / (BigInt(10) ^ 20), equals(-123456789L));
    CHECK(BigInt("-12345678999999999999999999999") / (BigInt(10) ^ 20), equals(-123456789L));
    CHECK(BigInt("12345678900000000000000000000") / -(BigInt(10) ^ 20), equals(-123456789L));
    CHECK(BigInt("12345678999999999999999999999") / -(BigInt(10) ^ 20), equals(-123456789L));
    CHECK(BigInt("-12345678900000000000000000000") / -(BigInt(10) ^ 20), equals(123456789L));
    CHECK(BigInt("-12345678999999999999999999999") / -(BigInt(10) ^ 20), equals(123456789L));
    CHECK(((BigInt(2) ^ 39) - 1) / (BigInt(2) ^ 22), equals((BigInt(2) ^ 17) - 1));
    CHECK((BigInt(2) ^ 39) / (BigInt(2) ^ 22), equals(BigInt(2) ^ 17));
    CHECK(((BigInt(2) ^ 39) + 1) / (BigInt(2) ^ 22), equals(BigInt(2) ^ 17));
    CHECK(inf / three, equals(inf));
    CHECK(inf / negativeFive, equals(negInf));
    CHECK(negInf / three, equals(negInf));
    CHECK(negInf / negativeFive, equals(inf));

    // divideRoundAwayFrom0
    CHECK(divideRoundAwayFrom0(three, negativeFive), equals(-1));
    CHECK(divideRoundAwayFrom0(negativeFive, three), equals(-2));
    CHECK(divideRoundAwayFrom0(negativeFive, 2), equals(-3));
    CHECK(divideRoundAwayFrom0(big, 7), equals(bigUL/7 + (bigUL%7 ? 1 : 0)));
    CHECK(divideRoundAwayFrom0(BigInt("12345678900000000000000000000"), BigInt(10) ^ 20), equals(123456789U));
    CHECK(divideRoundAwayFrom0(BigInt("12345678999999999999999999999"), BigInt(10) ^ 20), equals(123456790U));
    CHECK(divideRoundAwayFrom0(BigInt("-12345678900000000000000000000"), BigInt(10) ^ 20), equals(-123456789L));
    CHECK(divideRoundAwayFrom0(BigInt("-12345678999999999999999999999"), BigInt(10) ^ 20), equals(-123456790L));
    CHECK(divideRoundAwayFrom0(BigInt("12345678900000000000000000000"), -(BigInt(10) ^ 20)), equals(-123456789L));
    CHECK(divideRoundAwayFrom0(BigInt("12345678999999999999999999999"), -(BigInt(10) ^ 20)), equals(-123456790L));
    CHECK(divideRoundAwayFrom0(BigInt("-12345678900000000000000000000"), -(BigInt(10) ^ 20)), equals(123456789L));
    CHECK(divideRoundAwayFrom0(BigInt("-12345678999999999999999999999"), -(BigInt(10) ^ 20)), equals(123456790L));
    CHECK(divideRoundAwayFrom0((BigInt(2) ^ 39) - 1, BigInt(2) ^ 22), equals(BigInt(2) ^ 17));
    CHECK(divideRoundAwayFrom0(BigInt(2) ^ 39, BigInt(2) ^ 22), equals(BigInt(2) ^ 17));
    CHECK(divideRoundAwayFrom0((BigInt(2) ^ 39) + 1, BigInt(2) ^ 22), equals((BigInt(2) ^ 17) + 1));
    CHECK(divideRoundAwayFrom0(inf, three), equals(inf));
    CHECK(divideRoundAwayFrom0(inf, negativeFive), equals(negInf));
    CHECK(divideRoundAwayFrom0(negInf, three), equals(negInf));
    CHECK(divideRoundAwayFrom0(negInf, negativeFive), equals(inf));

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

TEST(BigInt, gcd) {
    CHECK(my_gcd(BigInt(2)*2*3*7*733*929*13723*4956871, BigInt(3)*3*3*5*929*74597*4956871), equals(BigInt(3)*929*4956871));

    // Brilliant (app) > Number Theory > Fermat's Little Theorem > Primality Testing > Additional Problems
    // https://brilliant.org/wiki/prime-testing/
    // You've stumbled onto a significant vulnerability in a commonly used cryptographic library. It turns out that the random number generator it uses frequently produces the same primes when it is generating keys.
    // Exploit this knowledge to factor the (hexadecimal) keys below, and enter your answer as the last six digits of the largest factor you find (in decimal).
    /*
Key 1:
1c7bb1ae67670f7e6769b515c174414278e16c27e95b43a789099a1c7d55c717b2f0a0442a7d49503ee09552588ed9bb6eda4af738a02fb31576d78ff72b2499b347e49fef1028182f158182a0ba504902996ea161311fe62b86e6ccb02a9307d932f7fa94cde410619927677f94c571ea39c7f4105fae00415dd7d
Key 2:
2710e45014ed7d2550aac9887cc18b6858b978c2409e86f80bad4b59ebcbd90ed18790fc56f53ffabc0e4a021da2e906072404a8b3c5555f64f279a21ebb60655e4d61f4a18be9ad389d8ff05b994bb4c194d8803537ac6cd9f708e0dd12d1857554e41c9cbef98f61c5751b796e5b37d338f5d9b3ec3202b37a32f
     */
    BigInt assumedPQ("0x1c7bb1ae67670f7e6769b515c174414278e16c27e95b43a789099a1c7d55c717b2f0a0442a7d49503ee09552588ed9bb6eda4af738a02fb31576d78ff72b2499b347e49fef1028182f158182a0ba504902996ea161311fe62b86e6ccb02a9307d932f7fa94cde410619927677f94c571ea39c7f4105fae00415dd7d");
    BigInt assumedPR("0x2710e45014ed7d2550aac9887cc18b6858b978c2409e86f80bad4b59ebcbd90ed18790fc56f53ffabc0e4a021da2e906072404a8b3c5555f64f279a21ebb60655e4d61f4a18be9ad389d8ff05b994bb4c194d8803537ac6cd9f708e0dd12d1857554e41c9cbef98f61c5751b796e5b37d338f5d9b3ec3202b37a32f");
    BigInt assumedP("84712455329458472210475092572678509526272708993404574231933262963274962364443789510742328496333782156398316832447160740326193165821341070265858616467");
    BigInt assumedQ("3435871829708679014447596389440344584309955989618078613506398153304765923748971812293610680963305820321694192278092116849920331574779248450640734127");
    BigInt assumedR("4712455329458472210475092572678509526272708993404574231933262963274962364443789510742328496333782156398316832447160740326193165821341070265858616437");
    auto computedGCD = my_gcd(assumedPQ, assumedPR);
    ASSERT(computedGCD, equals(assumedP)); // answer: 616467
    CHECK(assumedPQ / assumedP, equals(assumedQ));
    CHECK(assumedPR / assumedP, equals(assumedR));
}
