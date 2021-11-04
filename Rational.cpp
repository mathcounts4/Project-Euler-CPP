#include "Rational.hpp"

template<> void Rational::reduce() {
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

template<> Rational& Rational::operator+=(Rational const& o) {
    n = n*o.d + o.n*d;
    d *= o.d;
    reduce();
    return *this;
}

template<> Rational& Rational::operator-=(Rational const& o) {
    n = n*o.d - o.n*d;
    d *= o.d;
    reduce();
    return *this;
}

template<> Rational& Rational::operator*=(Rational const& o) {
    n *= o.n;
    d *= o.d;
    reduce();
    return *this;
}

template<> Rational& Rational::operator/=(Rational const& o) {
    if (!o.n)
	throw_exception<std::domain_error>("Division by zero fraction.");
    n *= o.d;
    d *= o.n;
    reduce();
    return *this;
}

