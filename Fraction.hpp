#pragma once

#include <functional>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>

#include "MyHashUtil.hpp"
#include "StringUtils.hpp"

template<class IntType = long>
class Fraction {
private:
    IntType n,d;
    friend std::size_t std::hash<Fraction>::operator()(const Fraction & x) const;
    // returns the gcd of x and y, with the sign of y
    static IntType gcd(IntType x, IntType y) {
	if (x == 0)
	    return y?:1;
	IntType mult = y<0?-1:1;
	x = x<0?-x:x;
	y = y<0?-y:y;
	for (IntType z; bool(z=y%x); ) {
	    y = x;
	    x = z;
	}
	return x * mult;
    }
public:
    Fraction(IntType N, IntType D) {
	IntType g = Fraction::gcd(N,D);
	n = N/g;
	d = D/g;
    }
    Fraction(IntType x) : Fraction(x,1) {}
    Fraction() : Fraction(IntType(0)) {}
    void swap(Fraction & o) {
	std::swap(n,o.n);
	std::swap(d,o.d);
    }
    explicit operator bool() const {
	return bool(n);
    }
    explicit operator double() const {
	return double(n)/double(d);
    }
    explicit operator IntType() const {
	return n/d;
    }
    explicit operator std::string() const {
	return to_std_string(n) + "/" + to_std_string(d);
    }
    bool operator<(Fraction const & o) const {
	return n*o.d < d*o.n;
    }
    bool operator>(Fraction const & o) const {
	return o < *this;
    }
    bool operator<=(Fraction const & o) const {
	return !(o < *this);
    }
    bool operator>=(Fraction const & o) const {
	return !(*this < o);
    }
    bool operator==(Fraction const & o) const {
	return n==o.n && d==o.d;
    }
    bool operator!=(Fraction const & o) const {
	return !(*this == o);
    }
    Fraction & operator+=(Fraction const & o) {
	IntType g = Fraction::gcd(d,o.d);
        if (o.d/g > std::numeric_limits<IntType>::max()/d)
	    throw std::overflow_error("Numbers too large.");
	n = o.d/g*n+d/g*o.n;
	d = o.d/g*d;
	IntType newGCD = gcd(n,d);
	n /= newGCD;
	d /= newGCD;
	return *this;
    }
    Fraction & operator+=(IntType const & o) {
	return *this += Fraction(o);
    }
    Fraction operator+(Fraction const & o) const {
	Fraction ans(*this);
	ans += o;
	return ans;
    }
    Fraction operator+(IntType const & o) const {
	return *this + Fraction(o);
    }
    template<class T>
    friend Fraction operator+(T const & x, Fraction const & o) {
	return Fraction(x) + o;
    }
    Fraction operator-() const {
	return {-n,d};
    }
    Fraction & operator-=(Fraction const & o) {
	return *this += -o;
    }
    Fraction & operator-=(IntType const & o) {
	return *this -= Fraction(o);
    }
    Fraction operator-(Fraction const & o) const {
	Fraction ans(*this);
	ans -= o;
	return ans;
    }
    Fraction & operator-(IntType const & o) const {
	return *this - Fraction(o);
    }
    template<class T>
    friend Fraction operator-(T const & x, Fraction const & o) {
	return Fraction(x) - o;
    }
    Fraction & operator*=(Fraction const & o) {
	IntType g1 = Fraction::gcd(n,o.d);
	IntType g2 = Fraction::gcd(o.n,d);
	if (n != 0 && (o.n<0?-o.n:o.n)/g2>std::numeric_limits<IntType>::max()/((n<0?-n:n)/g1))
	    throw std::overflow_error("Numbers too large.");
	if (d != 0 && o.d/g1>std::numeric_limits<IntType>::max()/(d/g2))
	    throw std::overflow_error("Numbers too large.");
	n /= g1;
	n *= o.n/g2;
	d /= g2;
	d *= o.d/g1;
	return *this;
    }
    Fraction & operator*=(IntType const & o) {
	return *this *= Fraction(o);
    }
    Fraction operator*(Fraction const & o) const {
	Fraction ans(*this);
	ans *= o;
	return ans;
    }
    Fraction operator*(IntType const & o) const {
	return *this * Fraction(o);
    }
    template<class T>
    friend Fraction operator*(T const & x, Fraction const & o) {
	return Fraction(x) * o;
    }
    Fraction & operator/=(Fraction const & o) {
	if (o.n == 0)
	    throw std::domain_error("Division by zero fraction.");
	return *this *= Fraction(o.d,o.n);
    }
    Fraction & operator/=(IntType const & o) {
	return *this /= Fraction(o);
    }
    Fraction operator/(Fraction const & o) const {
	Fraction ans(*this);
	ans /= o;
	return ans;
    }
    Fraction operator/(IntType const & o) const {
	return *this / Fraction(o);
    }
    template<class T>
    friend Fraction operator/(T const & x, Fraction const & o) {
	return Fraction(x) / o;
    }
    friend std::ostream& operator<<(std::ostream& o, Fraction const & f) {
	return o << f.n << "/" << f.d;
    }
};

template<class IntType>
struct to_string_helper<Fraction<IntType> >
{
    static inline std::string to_string(Fraction<IntType> const & f)
	{
	    return std::string(f);
	}
};

MAKE_HASHABLE_TEMPLATE(<class intType>,Fraction<intType>,x,x.n,x.d)
