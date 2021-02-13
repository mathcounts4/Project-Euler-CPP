#include "Mod.hpp"

#include "Class.hpp"
#include "ExitUtils.hpp"
#include "Math.hpp"

#include <string>

UI Mod::globalMod = 0;

template<> std::ostream& Class<Mod>::print(std::ostream& oss, Mod const & mod) {
    oss << mod.value;
    if (mod.mod > 0)
	oss << " (mod " << mod.mod << ")";
    return oss;
}
template<> std::string Class<Mod>::name() {
    return "Mod";
}

void Mod::mod_mismatch_check(Mod const & o) const {
    if(mod != o.mod)
	throw_exception<std::invalid_argument>(
	    "Mod values " +
	    std::to_string(mod) +
	    " and " +
	    std::to_string(o.mod) +
	    " do not agree");
}

Mod::Mod()
    : Mod(globalMod,0) {}
Mod::Mod(UL v)
    : Mod(globalMod,v) {}
Mod::Mod(SL v)
    : Mod(globalMod,v) {}
Mod::Mod(UI v)
    : Mod(globalMod,v) {}
Mod::Mod(SI v)
    : Mod(globalMod,v) {}

Mod::Mod(UI m, UL v)
    : mod(m) {
    if (mod == 0 && v > std::numeric_limits<SL>::max())
	throw_exception<std::domain_error>(
	    "invalid Mod constructor values " +
	    VAR_STR(m) +
	    " and " +
	    VAR_STR(v));
    if (mod > 0)
	v %= mod;
    value = static_cast<SL>(v);
}
Mod::Mod(UI m, SL v)
    : mod(m) {
    if (mod > 0) {
	v %= mod;
	if (v < 0)
	    v += mod;
    }
    value = v;
}
Mod::Mod(UI m, UI v)
    : Mod(m,static_cast<UL>(v)) {
}
Mod::Mod(UI m, SI v)
    : Mod(m,static_cast<SL>(v)) {
}
Mod::Mod(Mod const & m)
    : mod(m.mod)
    , value(m.value) {
}

Mod& Mod::operator=(Mod const & m) = default;

void swap(Mod& x, Mod& y) {
    std::swap(x.mod,y.mod);
    std::swap(x.value,y.value);
}

Mod Mod::operator-() const {
    return create(-value);
}

Mod& Mod::operator++() {
    if (++value == mod)
	value = 0;
    return *this;
}

Mod& Mod::operator--() {
    if (value == 0) {
	value = mod;
    }
    --value;
    return *this;
}

Mod::operator bool() const {
    return value > 0;
}

Mod::operator SL() const {
    return value;
}

Mod::operator double() const {
    return value;
}

UI Mod::get_mod() const {
    return mod;
}

bool Mod::operator==(Mod const & m) const {
    mod_mismatch_check(m);
    return value == m.value;
}

bool Mod::operator!=(Mod const & m) const {
    return !(*this == m);
}

Mod Mod::operator+(Mod const & y) const {
    return Mod(*this) += y;
}

Mod& Mod::operator+=(Mod const & o){
    mod_mismatch_check(o);
    value += o.value;
    if (value >= mod)
	value -= mod;
    return *this;
}

Mod Mod::operator-(Mod const & y) const {
    return Mod(*this) -= y;
}

Mod& Mod::operator-=(Mod const & o) {
    mod_mismatch_check(o);
    if (value < o.value)
	value += mod;
    value -= o.value;
    return *this;
}

Mod Mod::operator*(Mod const & y) const {
    return Mod(*this) *= y;
}

Mod& Mod::operator*=(Mod const & o) {
    mod_mismatch_check(o);
    value *= o.value;
    if (mod)
	value %= mod;
    return *this;
}

Mod Mod::operator/(Mod const & y) const {
    return Mod(*this) /= y;
}

Mod& Mod::operator/=(Mod const & o) {
    mod_mismatch_check(o);
    if(o.value == 0)
	throw_exception<std::overflow_error>("Division by zero in Mod");
	    
    UI gcd = static_cast<UI>(my_gcd(o.value,mod));
	    
    if(value % gcd != 0)
	throw_exception<std::range_error>(
	    "Mod division requires " +
	    std::to_string(value) +
	    " by " +
	    std::to_string(o.value) +
	    " mod " +
	    std::to_string(mod));
	    
    value /= gcd;
    if (mod) {
	mod /= gcd;
	value *= mod_inv(o.value/gcd,mod);
	value %= mod;
    }
    return *this;
}

Mod Mod::operator^(SL const y) const {
    return Mod(*this) ^= y;
}

Mod& Mod::operator^=(SL const pow) {
    UL real_pow = my_abs(pow);
    if (pow < 0)
	value = static_cast<SL>(mod_inv(value,mod));
    Mod base = create(1);
    swap(*this,base);
    for (; real_pow; real_pow>>=1) {
	if (real_pow&1)
	    (*this) *= base;
	base *= base;
    }
    return *this;
}

Mod::GlobalModSentinel::GlobalModSentinel(UI newMod)
    : prev(Mod::globalMod) {
    Mod::globalMod = newMod;
}

Mod::GlobalModSentinel::~GlobalModSentinel() {
    Mod::globalMod = prev;
}

