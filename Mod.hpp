#pragma once

#include "Class.hpp"
#include "Optional.hpp"
#include "Math.hpp"

#include <vector>

class Mod;
template<> std::ostream& Class<Mod>::print(std::ostream& oss, Mod const& mod);
template<> std::string Class<Mod>::name();

// Mod an UI
class Mod {
  private:
    UI mod;
    SL value;
    static UI globalMod;
    friend class GlobalModSentinel;
    void mod_mismatch_check(Mod const& o) const;
  public:
    Mod();
    Mod(UL value);
    Mod(SL value);
    Mod(UI value);
    Mod(SI value);
    Mod(UI mod, UL value);
    Mod(UI mod, SL value);
    Mod(UI mod, UI value);
    Mod(UI mod, SI value);
    Mod(Mod const& m);
    
#define MOD_CAN_CREATE_TYPE(T) decltype(Mod(0U,declval<T const&>()))
    
    template<class T, class = MOD_CAN_CREATE_TYPE(T)>
    Mod create(T const& val) const {
	return Mod(mod,val);
    }

    Mod& operator=(Mod const& m);
    template<class T, class = MOD_CAN_CREATE_TYPE(T)>
    Mod& operator=(T const& val) {
	return *this = create(val);
    }
    
    friend void swap(Mod& x, Mod& y);
    Mod operator-() const;
    Mod& operator++();
    Mod& operator--();
    explicit operator B() const;
    explicit operator UI() const;
    explicit operator SL() const;
    explicit operator D() const;
    UI get_mod() const;
    friend Class<Mod>;

    // comparison operators
    // ==
    B operator ==(Mod const& m) const;
    template<class T, class = MOD_CAN_CREATE_TYPE(T)>
    friend B operator==(Mod const& m, T const& t) {
	return m == m.create(t);
    }
    template<class T, class = MOD_CAN_CREATE_TYPE(T)>
    friend B operator==(T const& t, Mod const& m) {
	return m.create(t) == m;
    }
    // !=
    B operator !=(Mod const& m) const;
    template<class T, class = MOD_CAN_CREATE_TYPE(T)>
    friend B operator!=(Mod const& m, T const& t) {
	return m != m.create(t);
    }
    template<class T, class = MOD_CAN_CREATE_TYPE(T)>
    friend B operator!=(T const& t, Mod const& m) {
	return m.create(t) != m;
    }
    
    // binary operators
    // +
    Mod operator+(Mod const& m) const;
    template<class T, class = MOD_CAN_CREATE_TYPE(T)> friend
    Mod operator+(Mod const& m, T const& t) {
	return m + m.create(t);
    }
    template<class T, class = MOD_CAN_CREATE_TYPE(T)> friend
    Mod operator+(T const& t, Mod const& m) {
	return m.create(t) + m;
    }
    // +=
    Mod& operator+=(Mod const& m);
    template<class T, class = MOD_CAN_CREATE_TYPE(T)> friend
    Mod& operator+=(Mod& m, T const& t) {
	return m += m.create(t);
    }
    // -
    Mod operator-(Mod const& m) const;
    template<class T, class = MOD_CAN_CREATE_TYPE(T)> friend
    Mod operator-(Mod const& m, T const& t) {
	return m - m.create(t);
    }
    template<class T, class = MOD_CAN_CREATE_TYPE(T)> friend
    Mod operator-(T const& t, Mod const& m) {
	return m.create(t) - m;
    }
    // -=
    Mod& operator-=(Mod const& m);
    template<class T, class = MOD_CAN_CREATE_TYPE(T)> friend
    Mod& operator-=(Mod& m, T const& t) {
	return m -= m.create(t);
    }
    // *
    Mod operator*(Mod const& m) const;
    template<class T, class = MOD_CAN_CREATE_TYPE(T)> friend
    Mod operator*(Mod const& m, T const& t) {
	return m * m.create(t);
    }
    template<class T, class = MOD_CAN_CREATE_TYPE(T)> friend
    Mod operator*(T const& t, Mod const& m) {
	return m.create(t) * m;
    }
    // *=
    Mod& operator*=(Mod const& m);
    template<class T, class = MOD_CAN_CREATE_TYPE(T)> friend
    Mod& operator*=(Mod& m, T const& t) {
	return m *= m.create(t);
    }
    // /
    Mod operator/(Mod const& m) const;
    template<class T, class = MOD_CAN_CREATE_TYPE(T)> friend
    Mod operator/(Mod const& m, T const& t) {
	return m / m.create(t);
    }
    template<class T, class = MOD_CAN_CREATE_TYPE(T)> friend
    Mod operator/(T const& t, Mod const& m) {
	return m.create(t) / m;
    }
    // /=
    Mod& operator/=(Mod const& m);
    template<class T, class = MOD_CAN_CREATE_TYPE(T)> friend
    Mod& operator/=(Mod& m, T const& t) {
	return m /= m.create(t);
    }
    // ^
    template<class Integral>
    Mod operator^(Integral y) const {
	return Mod(*this) ^= y;
    }
    // ^=
    template<class Integral>
    Mod& operator^=(Integral pow) {
	static_assert(std::is_integral_v<Integral>, "Exponent must be integral");
	if constexpr (!std::is_unsigned_v<Integral>) {
	    if (pow < Integral(0)) {
		value = static_cast<SL>(mod_inv(value, mod));
	    }
	}

	Mod base = create(1);
	swap(*this, base);
	for (auto abs_pow = my_abs(pow); abs_pow; abs_pow >>= 1) {
	    if (abs_pow % 2)
		(*this) *= base;
	    base *= base;
	}
	return *this;
    }

    friend B operator<(Mod const& x, Mod const& y);

    // CRT = Chinese Remainder Theorem
    // Inputs: a mod A, b mod B, ...
    // Returns: x mod lcm(A,B,...) with x≡a mod A, x≡b mod B, ...
    static Optional<Mod> uniqueMerge(std::vector<Mod> const& mods);

#undef MOD_CAN_CREATE_TYPE

    struct GlobalModSentinel {
      public:
	UI const prev;
	GlobalModSentinel(UI newMod);
	~GlobalModSentinel();
    };
};
