#pragma once

#include "Class.hpp"

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
    Mod operator^(SL y) const;
    // ^=
    Mod& operator^=(SL pow);

    friend B operator<(Mod const& x, Mod const& y);

#undef MOD_CAN_CREATE_TYPE

    struct GlobalModSentinel {
      public:
	UI const prev;
	GlobalModSentinel(UI newMod);
	~GlobalModSentinel();
    };
};
