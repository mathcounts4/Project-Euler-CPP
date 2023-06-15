#pragma once

#include "Class.hpp"
#include "Constants/One.hpp"
#include "Constants/Zero.hpp"
#include "ExitUtils.hpp"
#include "Math.hpp"

// AdjoinSqrt(a, b, k) represents a + b√k
// a, b, and k have type T - commonly an integer or Mod
template<class T>
class AdjoinSqrt {
  private:
    T fA;
    T fB;
    T fK;

    void kMismatchCheck(AdjoinSqrt const& o) const {
	if (fK != o.fK) {
	    throw_exception<std::invalid_argument>(
		"Adjoin values " +
		to_string(fK) +
		" and " +
	        to_string(o.fK) +
		" do not agree");
	}
    }

  public:
    AdjoinSqrt(T a, T b, T k)
	: fA(a)
	, fB(b)
	, fK(k) {
    }

    friend void swap(AdjoinSqrt& x, AdjoinSqrt& y) {
	std::swap(x.fA, y.fA);
	std::swap(x.fB, y.fB);
	std::swap(x.fK, y.fK);
    }

    T const& getA() const {
	return fA;
    }
    T const& getB() const {
	return fB;
    }
    T const& getK() const {
	return fK;
    }

    // equality
    friend B operator==(AdjoinSqrt const& x, AdjoinSqrt const& y) {
	x.kMismatchCheck(y);
	return x.fA == y.fA && x.fB == y.fB;
    }
    friend B operator!=(AdjoinSqrt const& x, AdjoinSqrt const& y) {
	return !(x == y);
    }

    // binary
    AdjoinSqrt& operator+=(AdjoinSqrt const& y) {
	kMismatchCheck(y);
	fA += y.fA;
	fB += y.fB;
	return *this;
    }
    friend AdjoinSqrt operator+(AdjoinSqrt const& x, AdjoinSqrt const& y) {
	return AdjoinSqrt(x) += y;
    }
    AdjoinSqrt& operator-=(AdjoinSqrt const& y) {
	kMismatchCheck(y);
	fA -= y.fA;
	fB -= y.fB;
	return *this;
    }
    friend AdjoinSqrt operator-(AdjoinSqrt const& x, AdjoinSqrt const& y) {
	return AdjoinSqrt(x) -= y;
    }
    AdjoinSqrt& operator*=(AdjoinSqrt const& y) {
	*this = *this * y;
	return *this;
    }
    friend AdjoinSqrt operator*(AdjoinSqrt const& x, AdjoinSqrt const& y) {
	x.kMismatchCheck(y);
	return {
	    x.fA * y.fA + x.fB * y.fB * x.fK,
	    x.fA * y.fB + x.fB * y.fA,
	    x.fK
	};
    }
    AdjoinSqrt operator^(UL pow) const {
        return my_pow(*this, pow);
    }
};

template<class T>
struct One<AdjoinSqrt<T>> {
    using X = AdjoinSqrt<T>;
    static inline X get() {
	static_assert(alwaysFalse<X>, "Cannot get 1 of type AdjoinSqrt since K is not known. Pass an example AdjoinSqrt value to one().");
    }
    
    static inline X get(X const& x) {
	return {one(x.getA()), zero(x.getB()), x.getK()};
    }
};

template<class T>
struct Zero<AdjoinSqrt<T>> {
    using X = AdjoinSqrt<T>;
    static inline X get() {
	static_assert(alwaysFalse<X>, "Cannot get 0 of type AdjoinSqrt since K is not known. Pass an example AdjoinSqrt value to zero().");
    }
    
    static inline X get(X const& x) {
	return {zero(x.getA()), zero(x.getB()), x.getK()};
    }
};

template<class T>
struct Class<AdjoinSqrt<T>> {
    static std::ostream& print(std::ostream& oss, AdjoinSqrt<T> const& x) {
	return oss << x.getA() << "+" << x.getB() << "√" << x.getK();
    }
    static std::string name() {
	return "AdjoinSqrt<" + Class<T>::name() + ">";
    }
};

