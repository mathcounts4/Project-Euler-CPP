#pragma once

#include "BigInt.hpp"
#include "Fraction.hpp"

using Rational = Fraction<BigInt>;

template<> extern void Rational::reduce();
template<> extern Rational& Rational::operator+=(Rational const& o);
template<> extern Rational& Rational::operator-=(Rational const& o);
template<> extern Rational& Rational::operator*=(Rational const& o);
template<> extern Rational& Rational::operator/=(Rational const& o);

