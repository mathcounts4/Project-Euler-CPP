#pragma once

#include <algorithm>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

#include "MyHashUtil.hpp"
#include "MyHashVector.hpp"
#include "StringUtils.hpp"

const unsigned int Default_BigInt_Mod = 1 << 30;

template<unsigned int mod = Default_BigInt_Mod>
class BigInt {
private:
    enum { Finite, Infinite } finite;
    enum { Positive, Negative } sign;
    std::vector<unsigned long> data;
    friend std::size_t std::hash<BigInt>::operator()(const BigInt & x) const;
    void reduce() {
	while (data.size() && data.back() == 0)
	    data.pop_back();
	if (data.size() == 0)
	    sign = Positive;
    }
public:
    BigInt() : finite(Finite), sign(Positive) {}
    BigInt(long val) : finite(Finite), sign(val>=0?Positive:Negative) {
	for (val=abs(val);val;val/=mod)
	    data.push_back(val % mod);
    }
    static BigInt Inf() {
	BigInt ans;
	ans.finite = Infinite;
	return ans;
    }
    static BigInt NegInf() {
	BigInt ans = Inf();
	ans.sign = Negative;
	return ans;
    }
    void swap(BigInt & other) {
	std::swap(finite,other.finite);
	std::swap(sign,other.sign);
	std::swap(data,other.data);
    }
    explicit operator bool() const {
	return data.size() || finite == Infinite;
    }
    explicit operator char() const {
	if (finite == Infinite)
	    throw std::domain_error("Cannot convert infinite value to char");
	int ans = 0;
	for (int i = data.size(); i--; ) {
	    if (std::numeric_limits<char>::max()/mod-1 <= ans)
		throw std::overflow_error("Too large for char");
	    ans = ans * mod + data[i];
	}
	return sign == Positive ? ans : -ans;
    }
    explicit operator int() const {
	if (finite == Infinite)
	    throw std::domain_error("Cannot convert infinite value to int");
	int ans = 0;
	for (int i = data.size(); i--; ) {
	    if (std::numeric_limits<int>::max()/mod-1 <= ans)
		throw std::overflow_error("Too large for int");
	    ans = ans * mod + data[i];
	}
	return sign == Positive ? ans : -ans;
    }
    explicit operator long() const {
	if (finite == Infinite)
	    throw std::domain_error("Cannot convert infinite value to long");
	long ans = 0;
	for (int i = data.size(); i--; ) {
	    if (std::numeric_limits<long>::max()/mod-1 <= ans)
		throw std::overflow_error("Too large for long");
	    ans = ans * mod + data[i];
	}
	return sign == Positive ? ans : -ans;
    }
    explicit operator double() const {
	if (finite == Infinite)
	    return sign == Positive ? std::numeric_limits<double>::infinity() : -std::numeric_limits<double>::infinity();
	double ans = 0;
	for (int i = data.size(); i--; ) {
	    if (std::numeric_limits<double>::max()/mod-1 <= ans)
		throw std::overflow_error("Too large for double");
	    ans = ans * mod + data[i];
	}
	return sign == Positive ? ans : -ans;
    }
    explicit operator std::string() const {
	if (finite == Infinite)
	    return sign == Positive ? "INF" : "-INF";
	if (*this == 0)
	    return "0";
	std::string ans;
	BigInt copy(*this);
	copy.sign = Positive;
	while (bool(copy)) {
	    ans += '0' + char(copy % 10);
	    copy /= 10;
	}
	if (sign == Negative)
	    ans += "-";
	std::reverse(ans.begin(),ans.end());
	return ans;
    }
    bool operator>(BigInt const & other) const {
	if (finite == Infinite && other.finite == Infinite)
	    return sign == Positive && other.sign == Negative;
	if (finite == Infinite)
	    return sign == Positive;
	if (other.finite == Infinite)
	    return other.sign == Negative;
	if (data.size() == 0 && other.data.size() == 0)
	    return false;
	if (sign != other.sign)
	    return sign == Positive;
	if (data.size() != other.data.size())
	    return data.size() > other.data.size() ^ sign == Negative;
	for (int i = data.size(); --i >= 0; )
	    if (data[i] != other.data[i])
		return data[i] > other.data[i] ^ sign == Negative;
	return false;
    }
    bool operator<(BigInt const & other) const {
	return other > *this;
    }
    bool operator>=(BigInt const & other) const {
	return !(other > *this);
    }
    bool operator<=(BigInt const & other) const {
	return !(*this > other);
    }
    bool operator!=(BigInt const & other) const {
	return *this > other || other > *this;
    }
    bool operator==(BigInt const & other) const {
	
	return !(*this != other);
    }
    BigInt operator-() const {
	BigInt negative(*this);
	negative.sign = sign == Positive ? Negative : Positive;
	return negative;
    }
    BigInt & operator+=(BigInt const & other) {
	return *this -= -other;
    }
    BigInt operator+(BigInt const & other) const {
	BigInt ans(*this);
	ans += other;
	return ans;
    }
    template<class T>
    friend BigInt operator+(T const & lhs, BigInt const & rhs) {
	return BigInt(lhs) + rhs;
    }
    BigInt & operator-=(BigInt const & other) {
	if (finite == Infinite && other.finite == Infinite)
	    return sign == other.sign ? *this = 0 : *this;
	if (finite == Infinite)
	    return *this;
	if (other.finite == Infinite)
	    return *this = -other;
	auto outSign = *this >= other ? Positive : Negative;
	int carry = 0;
	for (int i = 0; i < other.data.size() || i < data.size() || carry; ++i) {
	    long cur = carry;
	    carry = 0;
	    if (i < other.data.size())
		cur += other.sign == outSign ? -other.data[i] : other.data[i];
	    if (i < data.size())
		cur += sign == outSign ? data[i] : -data[i];
	    if (cur < 0) {
		cur += mod;
		carry = -1;
	    } else if (cur >= mod) {
		cur -= mod;
		carry = 1;
	    }
	    if (i == data.size()) {
		data.push_back(cur);
	    } else {
		data[i] = cur;
	    }
	}
	reduce();
	sign = outSign;
	return *this;
    }
    BigInt operator-(BigInt const & other) const {
	BigInt ans(*this);
	ans -= other;
	return ans;
    }
    template<class T>
    friend BigInt operator-(T const & lhs, BigInt const & rhs) {
	return BigInt(lhs) - rhs;
    }
    BigInt & operator*=(BigInt const & other) {
	if (*this == 0 || other == 0)
	    return *this = 0;
	if (finite == Infinite || other.finite == Infinite) {
	    sign = sign == other.sign ? Positive : Negative;
	    return *this;
	}
	BigInt ans;
	ans.sign = sign == other.sign ? Positive : Negative;
	for (int i = 0; i < data.size(); ++i) {
	    for (int j = 0; j < other.data.size(); ++j) {
		if (i+j == ans.data.size())
		    ans.data.push_back(0);
		ans.data[i+j] += data[i]*other.data[j];
		for (int k = i+j; ans.data[k] >= mod; ++k) {
		    if (k+1 == ans.data.size())
			ans.data.push_back(0);
		    ans.data[k+1] += ans.data[k]/mod;
		    ans.data[k] %= mod;
		}
	    }
	}
	swap(ans);
	reduce();
	return *this;
    }
    BigInt operator*(BigInt const & other) const {
	BigInt ans(*this);
	ans *= other;
	return ans;
    }
    template<class T>
    friend BigInt operator*(T const & lhs, BigInt const & rhs) {
	return BigInt(lhs) * rhs;
    }
    BigInt & operator/=(BigInt const & other) {
	if (other == 0)
	    throw std::domain_error("Division by zero");
	if (finite == Infinite && other.finite == Infinite)
	    return *this = sign == other.sign ? 1 : -1;
	if (finite == Infinite) {
	    sign = sign == other.sign ? Positive : Negative;
	    return *this;
	}
	if (other.finite == Infinite)
	    return *this = 0;
	std::vector<BigInt> mult{1}, times{other};
	times[0].sign = Positive;
	BigInt ans;
	auto ans_sign = sign == other.sign ? Positive : Negative;
	sign = Positive;
	while (times.back() <= *this) {
	    mult.push_back(mult.back() * 2);
	    times.push_back(times.back() * 2);
	}
	while (bool(*this) && times.size()) {
	    if (*this >= times.back()) {
		*this -= times.back();
		ans += mult.back();
	    }
	    times.pop_back();
	    mult.pop_back();
	}
	ans.sign = ans_sign;
	swap(ans);
	return *this;
    }
    BigInt operator/(BigInt const & other) const {
	BigInt ans(*this);
	ans /= other;
	return ans;
    }
    template<class T>
    friend BigInt operator/(T const & lhs, BigInt const & rhs) {
	return BigInt(lhs) / rhs;
    }
    BigInt & operator%=(BigInt const & other) {
	return *this -= *this / other * other;
    }
    BigInt operator%(BigInt const & other) const {
	BigInt ans(*this);
	ans %= other;
	return ans;
    }
    template<class T>
    friend BigInt operator%(T const & lhs, BigInt const & rhs) {
	return BigInt(lhs) % rhs;
    }
    friend std::ostream& operator<<(std::ostream& s, BigInt const & x) {
	return s << std::string(x);
    }
};

MAKE_HASHABLE_TEMPLATE(<unsigned int mod>,BigInt<mod>,x,x.finite,x.sign,x.data)

template<unsigned int mod>
class std::numeric_limits<BigInt<mod> > {
public:
    static constexpr bool is_specialized = false;
    static constexpr BigInt<mod> min() noexcept { return BigInt<mod>::Inf(); }
    static constexpr BigInt<mod> max() noexcept { return BigInt<mod>::Inf(); }
    static constexpr BigInt<mod> lowest() noexcept { return -BigInt<mod>::Inf(); }
    static constexpr int  digits = 0;
    static constexpr int  digits10 = 0;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = false;
    static constexpr int radix = 0;
    static constexpr BigInt<mod> epsilon() noexcept { return 1; }
    static constexpr BigInt<mod> round_error() noexcept { return 1; }
    
    static constexpr int  min_exponent = 0;
    static constexpr int  min_exponent10 = 0;
    static constexpr int  max_exponent = 0;
    static constexpr int  max_exponent10 = 0;
    
    static constexpr bool has_infinity = true;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr float_denorm_style has_denorm = denorm_absent;
    static constexpr bool has_denorm_loss = false;
    static constexpr BigInt<mod> infinity() noexcept { return BigInt<mod>::Inf(); }
    static constexpr BigInt<mod> quiet_NaN() noexcept { return BigInt<mod>(); }
    static constexpr BigInt<mod> signaling_NaN() noexcept { return BigInt<mod>(); }
    static constexpr BigInt<mod> denorm_min() noexcept { return BigInt<mod>(); }

    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = false;
    static constexpr bool is_modulo = false;

    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;
    static constexpr float_round_style round_style = round_toward_zero;
};

template<unsigned int mod>
struct to_string_helper<BigInt<mod> >
{
    static inline std::string to_string(BigInt<mod> const & b)
	{
	    return std::string(b);
	}
};
