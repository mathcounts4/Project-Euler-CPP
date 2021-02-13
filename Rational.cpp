#include "Rational.hpp"

template<> void Fraction<BigInt>::reduce() {
    auto const g = my_gcd(n,d);
    if (g != 1) {
	n /= g;
	d /= g;
    }
    if (d.neg()) {
	n.negateMe();
	d.negateMe();
    }
}

template<> bool Fraction<BigInt>::operator<(Fraction const & o) const {
    int const l_comp = n.cmp();
    int const r_comp = o.n.cmp();
    if (l_comp != r_comp || !l_comp)
	return l_comp < r_comp;
    return n*o.d < o.n*d;
}

template<> Fraction<BigInt> Fraction<BigInt>::operator-() const {
    return Fraction(Fraction::NoReduce{}, -n, d);
}

template<> Fraction<BigInt>& Fraction<BigInt>::operator+=(Fraction const & o) {
    n = n*o.d + o.n*d;
    d *= o.d;
    reduce();
    return *this;
}

template<> Fraction<BigInt>& Fraction<BigInt>::operator-=(Fraction const & o) {
    n = n*o.d - o.n*d;
    d *= o.d;
    reduce();
    return *this;
}

template<> Fraction<BigInt>& Fraction<BigInt>::operator*=(Fraction const & o) {
    n *= o.n;
    d *= o.d;
    reduce();
    return *this;
}

template<> Fraction<BigInt>& Fraction<BigInt>::operator/=(Fraction const & o) {
    if (!o.n)
	throw_exception<std::domain_error>("Division by zero fraction.");
    n *= o.d;
    d *= o.n;
    reduce();
    return *this;
}

