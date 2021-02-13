#pragma once

#include "BigInt.hpp"
#include "Fraction.hpp"

using Rational = Fraction<BigInt>;

template<> extern void Fraction<BigInt>::reduce();
template<> extern bool Fraction<BigInt>::operator<(Fraction const & o) const;
template<> extern Fraction<BigInt> Fraction<BigInt>::operator-() const;
template<> extern Fraction<BigInt>& Fraction<BigInt>::operator+=(Fraction const & o);
template<> extern Fraction<BigInt>& Fraction<BigInt>::operator-=(Fraction const & o);
template<> extern Fraction<BigInt>& Fraction<BigInt>::operator*=(Fraction const & o);
template<> extern Fraction<BigInt>& Fraction<BigInt>::operator/=(Fraction const & o);

