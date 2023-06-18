#pragma once

#include "Class.hpp"
#include "Constants/One.hpp"
#include "Constants/Zero.hpp"
#include "ExitUtils.hpp"
#include "Math.hpp"

// AdjoinSqrt(k, {a, b}) represents a + b√k
// a, b, and k have type T - commonly an integer or Mod
template<SZ nthRoot, class T>
class Adjoin {
  private:
    T fK;
    std::array<T, nthRoot> fValues;

    void kMismatchCheck(Adjoin const& o) const {
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
    Adjoin(T k, std::array<T, nthRoot> values)
	: fK(std::move(k))
	, fValues(std::move(values)) {
	static_assert(nthRoot > 1, "Adjoin only makes sense when you have a square root, cube root, etc");
    }

    friend void swap(Adjoin& x, Adjoin& y) {
	std::swap(x.fK, y.fK);
	std::swap(x.fValues, y.fValues);
    }

    T const& getK() const {
	return fK;
    }
    auto const& getValues() const {
	return fValues;
    }

    // equality
    friend B operator==(Adjoin const& x, Adjoin const& y) {
	x.kMismatchCheck(y);
	return x.fValues == y.fValues;
    }
    friend B operator!=(Adjoin const& x, Adjoin const& y) {
	return !(x == y);
    }

    // binary
    Adjoin& operator+=(Adjoin const& y) {
	kMismatchCheck(y);
	for (SZ i = 0; i < nthRoot; ++i) {
	    fValues[i] += y.fValues[i];
	}
	return *this;
    }
    friend Adjoin operator+(Adjoin const& x, Adjoin const& y) {
	return Adjoin(x) += y;
    }
    Adjoin& operator-=(Adjoin const& y) {
	kMismatchCheck(y);
	for (SZ i = 0; i < nthRoot; ++i) {
	    fValues[i] -= y.fValues[i];
	}
	return *this;
    }
    friend Adjoin operator-(Adjoin const& x, Adjoin const& y) {
	return Adjoin(x) -= y;
    }
    Adjoin& operator*=(Adjoin const& y) {
	*this = *this * y;
	return *this;
    }
    friend Adjoin operator*(Adjoin const& x, Adjoin const& y) {
	x.kMismatchCheck(y);
	return {x.getK(), Adjoin::multiply(x.getK(), x.getValues(), y.getValues(), std::make_index_sequence<nthRoot>{})};
    }
    Adjoin operator^(UL pow) const {
	return my_pow(*this, pow);
    }

  private:
    template<SZ i, SZ... j>
    static T multiplyValue(T const& k, std::array<T, nthRoot> const& x, std::array<T, nthRoot> const& y, std::index_sequence<j...>) {
	return ((i >= j ? x[j] * y[i-j] : x[j] * y[i+sizeof...(j)-j] * k) + ...);
    }
    
    template<SZ... i>
    static std::array<T, nthRoot> multiply(T const& k, std::array<T, nthRoot> const& x, std::array<T, nthRoot> const& y, std::index_sequence<i...> s) {
	return {multiplyValue<i>(k, x, y, s)...};
    }
};

namespace detail {
    template<class T, SZ... i>
    std::array<T, sizeof...(i)> makeArray(T const& first, T const& rest, std::index_sequence<i...>) {
	return {i == 0 ? first : rest...};
    }
    template<class T, SZ n>
    std::array<T, n> makeArray(T const& first, T const& rest) {
	return makeArray(first, rest, std::make_index_sequence<n>{});
    }
}

template<SZ nthRoot, class T>
struct One<Adjoin<nthRoot, T>> {
    using X = Adjoin<nthRoot, T>;
    static inline X get() {
	static_assert(alwaysFalse<X>, "Cannot get 1 of type Adjoin since K is not known. Pass an example Adjoin value to one().");
    }
    
    static inline X get(X const& x) {
	auto const& v = x.getValues()[0];
	return {x.getK(), detail::makeArray<T, nthRoot>(one(v), zero(v))};
    }
};

template<SZ nthRoot, class T>
struct Zero<Adjoin<nthRoot, T>> {
    using X = Adjoin<nthRoot, T>;
    static inline X get() {
	static_assert(alwaysFalse<X>, "Cannot get 0 of type Adjoin since K is not known. Pass an example Adjoin value to zero().");
    }
    
    static inline X get(X const& x) {
	auto const& v = x.getValues()[0];
	return {x.getK(), detail::makeArray<T, nthRoot>(zero(v), zero(v))};
    }
};

template<SZ nthRoot, class T>
struct Class<Adjoin<nthRoot, T>> {
    static std::ostream& print(std::ostream& oss, Adjoin<nthRoot, T> const& x) {
	auto const& values = x.getValues();
	auto const& k = x.getK();
	if (nthRoot == 2) {
	    return oss << "(" << values[0] << ")+(" << values[1] << ")√(" << k << ")";
	} else {
	    oss << "(" << values[0] << ")";
	    for (SZ i = 1; i < nthRoot; ++i) {
		oss << "+(" << values[i] << ")*(" << k << ")^(" << i << "/" << nthRoot << ")";
	    }
	    return oss;
	}
    }
    static std::string name() {
	return "Adjoin<" + std::to_string(nthRoot) + "," + Class<T>::name() + ">";
    }
};

template<class T>
using AdjoinSqrt = Adjoin<2, T>;

