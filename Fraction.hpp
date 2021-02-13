#pragma once

#include <functional>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>

#include "Class.hpp"
#include "ExitUtils.hpp"
#include "MyHashUtil.hpp"
#include "SafeMath.hpp"
#include "StringUtils.hpp"

template<class IntType>
class Fraction {
  private:
    IntType n,d;
    friend std::size_t std::hash<Fraction>::operator()(const Fraction & x) const;
    friend struct Class<Fraction>;
    void reduce();
    struct NoReduce {};
    Fraction(NoReduce, IntType N, IntType D);
  public:
    Fraction(IntType N, IntType D);
    Fraction(IntType x);
    Fraction();
    void swap(Fraction & o);
    IntType const & getN() const;
    IntType const & getD() const;
    explicit operator bool() const;
    explicit operator double() const;
    
    bool operator <(Fraction const & o) const;
    bool operator >(Fraction const & o) const;
    bool operator <=(Fraction const & o) const;
    bool operator >=(Fraction const & o) const;
    bool operator ==(Fraction const & o) const;
    bool operator !=(Fraction const & o) const;
    
    Fraction operator-() const;

    Fraction& operator +=(Fraction const & o);
    Fraction operator +(Fraction const & o) const;
    Fraction& operator -=(Fraction const & o);
    Fraction operator -(Fraction const & o) const;
    Fraction& operator *=(Fraction const & o);
    Fraction operator *(Fraction const & o) const;
    Fraction& operator /=(Fraction const & o);
    Fraction operator /(Fraction const & o) const;
};
template<class IntType> Fraction(IntType) -> Fraction<IntType>;
template<class IntType> Fraction(IntType, IntType) -> Fraction<IntType>;
MAKE_HASHABLE_TEMPLATE(<class intType>,Fraction<intType>,x,x.n,x.d);

template<class IntType>
struct Class<Fraction<IntType> > {
    using T = Fraction<IntType>;
    static std::ostream& print(std::ostream& oss, T const & t);
    static Optional<T> parse(std::istream& is);
    static std::string name();
    static std::string format();
};

/* template Fraction impl */

template<class IntType> void Fraction<IntType>::reduce() {
    auto const g = my_gcd(n,d); // if lowest, g is negative
    n = SafeMath<IntType>(n) / IntType(g);
    d = SafeMath<IntType>(d) / IntType(g);
    if (d < IntType(0)) {
	IntType negOne(-1);
	n = SafeMath<IntType>(n) * negOne;
	d = SafeMath<IntType>(d) * negOne;
    }
}

template<class IntType> Fraction<IntType>::Fraction(NoReduce, IntType N, IntType D)
    : n(std::move(N))
    , d(std::move(D)) {
}

template<class IntType> Fraction<IntType>::Fraction(IntType N, IntType D)
    : n(std::move(N))
    , d(std::move(D)) {
    if (!d)
	throw_exception<std::domain_error>("Zero denominzator passed to " + Class<Fraction>::name());
    reduce();
}
template<class IntType> Fraction<IntType>::Fraction(IntType x) : n{std::move(x)}, d{1} {}
template<class IntType> Fraction<IntType>::Fraction() : n{0}, d{1} {}

template<class IntType> void Fraction<IntType>::swap(Fraction & o) {
    std::swap(n,o.n);
    std::swap(d,o.d);
}

template<class IntType> IntType const & Fraction<IntType>::getN() const {
    return n;
}

template<class IntType> IntType const & Fraction<IntType>::getD() const {
    return d;
}

template<class IntType> Fraction<IntType>::operator bool() const {
    return bool(n);
}
template<class IntType> Fraction<IntType>::operator double() const {
    return double(n)/double(d);
}
template<class IntType> bool Fraction<IntType>::operator<(Fraction const & o) const {
    int const l_comp = (n > 0) - (n < 0);
    int const r_comp = (o.n > 0) - (o.n < 0);
    if (l_comp != r_comp || !l_comp)
	return l_comp < r_comp;
    auto const l_val = SafeMath<IntType>(n) * o.d;
    auto const r_val = SafeMath<IntType>(o.n) * d;
    if (l_val && r_val)
	return *l_val < *r_val;
    Fraction const result = *this/o; // hopefully some gcd cancels to allow multiplication
    return result.n < result.d;
}
template<class IntType> bool operator<(IntType const & x, Fraction<IntType> const & y) {
    return Fraction{x} < y;
}
template<class IntType> bool Fraction<IntType>::operator>(Fraction const & o) const {
    return o < *this;
}
template<class IntType> bool operator>(IntType const & x, Fraction<IntType> const & y) {
    return Fraction{x} > y;
}
template<class IntType> bool Fraction<IntType>::operator<=(Fraction const & o) const {
    return !(o < *this);
}
template<class IntType> bool operator<=(IntType const & x, Fraction<IntType> const & y) {
    return Fraction{x} <= y;
}
template<class IntType> bool Fraction<IntType>::operator>=(Fraction const & o) const {
    return !(*this < o);
}
template<class IntType> bool operator>=(IntType const & x, Fraction<IntType> const & y) {
    return Fraction{x} >= y;
}
template<class IntType> bool Fraction<IntType>::operator==(Fraction const & o) const {
    return n==o.n && d==o.d;
}
template<class IntType> bool operator==(IntType const & x, Fraction<IntType> const & y) {
    return Fraction{x} == y;
}
template<class IntType> bool Fraction<IntType>::operator!=(Fraction const & o) const {
    return !(*this == o);
}
template<class IntType> bool operator!=(IntType const & x, Fraction<IntType> const & y) {
    return Fraction{x} != y;
}

template<class IntType> Fraction<IntType> Fraction<IntType>::operator-() const {
    static_assert(std::is_signed_v<IntType>,"Negating an unsigned fraction - perhaps you meant to use a signed type?");
    if (n == -n)
	throw_exception<std::domain_error>("Attempted to negate " + to_string(*this) + " failed due to numerator");
    return Fraction(NoReduce{},-n,d);
}
template<class IntType> Fraction<IntType>& Fraction<IntType>::operator+=(Fraction const & o) {
    // d's are pos -> g is pos
    IntType const g = IntType(my_gcd(d,o.d));
    IntType const m_me = o.d/g;
    IntType const m_o = d/g;
    n = SafeMath<IntType>(n)*m_me + SafeMath<IntType>(o.n)*m_o;
    d = SafeMath<IntType>(d) * m_me;
    reduce();
    return *this;
}
template<class IntType> Fraction<IntType> Fraction<IntType>::operator+(Fraction const & o) const {
    return Fraction(*this) += o;
}
template<class IntType> Fraction<IntType> operator+(IntType const & x, Fraction<IntType> const & y) {
    return Fraction<IntType>(x) += y;
}
template<class IntType> Fraction<IntType>& Fraction<IntType>::operator-=(Fraction const & o) {
    // d's are pos -> g is pos
    IntType const g = IntType(my_gcd(d,o.d));
    IntType const m_me = o.d/g;
    IntType const m_o = d/g;
    n = SafeMath<IntType>(n)*m_me - SafeMath<IntType>(o.n)*m_o;
    d = SafeMath<IntType>(d) * m_me;
    reduce();
    return *this;
}
template<class IntType> Fraction<IntType> Fraction<IntType>::operator-(Fraction const & o) const {
    return Fraction(*this) -= o;
}
template<class IntType> Fraction<IntType> operator-(IntType const & x, Fraction<IntType> const & y) {
    return Fraction<IntType>(x) -= y;
}
template<class IntType> Fraction<IntType>& Fraction<IntType>::operator*=(Fraction const & o) {
    // d's are pos -> g's are pos
    IntType const g1 = IntType(my_gcd(n,o.d));
    IntType const g2 = IntType(my_gcd(o.n,d));
    n = SafeMath<IntType>(n/g1)*(o.n/g2);
    d = SafeMath<IntType>(d/g2)*(o.d/g1);
    return *this;
}
template<class IntType> Fraction<IntType> Fraction<IntType>::operator*(Fraction const & o) const {
    return Fraction(*this) *= o;
}
template<class IntType> Fraction<IntType> operator*(IntType const & x, Fraction<IntType> const & y) {
    return Fraction<IntType>(x) *= y;
}
template<class IntType> Fraction<IntType>& Fraction<IntType>::operator/=(Fraction const & o) {
    if (!o.n)
	throw_exception<std::domain_error>("Division by zero " + class_name<Fraction<IntType> >());
    return *this *= Fraction(o.d,o.n);
}
template<class IntType> Fraction<IntType> Fraction<IntType>::operator/(Fraction const & o) const {
    return Fraction(*this) /= o;
}
template<class IntType> Fraction<IntType> operator/(IntType const & x, Fraction<IntType> const & y) {
    return Fraction<IntType>(x) /= y;
}



/* template Class<Fraction> impl */

template<class IntType>
std::ostream& Class<Fraction<IntType> >::print(std::ostream& oss, Fraction<IntType> const & f) {
    if (f.d == IntType(1)) {
	return oss << f.n;
    } else {
	return oss << '(' << f.n << '/' << f.d << ')';
    }
}

template<class IntType>
Optional<Fraction<IntType> > Class<Fraction<IntType> >::parse(std::istream& is) {
    Resetter resetter{is};
    bool const inParens = consume(is,'(');
    auto n = Class<IntType>::parse(is);
    if (!n)
	return Failure("Failed to parse numerator for " + name() + ": " + n.cause());
    if (!consume(is,'/')) {
	resetter.ignore();
	return {*n,IntType(1)};
    }
    auto d = Class<IntType>::parse(is);
    if (!d)
	return Failure("Failed to parse denominator for " + name() + ": " + d.cause());
	
    if (inParens && !consume(is,')'))
	return Failure("Parsing " + name() + " failure: no ')' found");

    resetter.ignore();
    return {*n,*d};
}

template<class IntType>
std::string Class<Fraction<IntType> >::name() {
    return "Fraction<" + Class<IntType>::name() + ">";
}

template<class IntType>
std::string Class<Fraction<IntType> >::format() {
    return Class<IntType>::format() + "/" + Class<IntType>::format();
}

