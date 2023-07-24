#include "Mod.hpp"

#include "Class.hpp"
#include "ExitUtils.hpp"
#include "Math.hpp"
#include "PrimeUtils.hpp"

#include <string>

UI Mod::globalMod = 0;

template<> std::ostream& Class<Mod>::print(std::ostream& oss, Mod const& mod) {
    oss << mod.value;
    if (mod.mod > 0)
	oss << " (mod " << mod.mod << ")";
    return oss;
}
template<> std::string Class<Mod>::name() {
    return "Mod";
}

void Mod::mod_mismatch_check(Mod const& o) const {
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
Mod::Mod(Mod const& m)
    : mod(m.mod)
    , value(m.value) {
}

Mod& Mod::operator=(Mod const& m) = default;

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

Mod::operator B() const {
    return value > 0;
}

Mod::operator UI() const {
    if (mod == 0) {
	throw_exception<std::logic_error>("If mod can be 0, cast to SL instead of UI");
    }
    return static_cast<UI>(value);
}

Mod::operator SL() const {
    return value;
}

Mod::operator D() const {
    return value;
}

UI Mod::get_mod() const {
    return mod;
}

bool Mod::operator==(Mod const& m) const {
    mod_mismatch_check(m);
    return value == m.value;
}

bool Mod::operator!=(Mod const& m) const {
    return !(*this == m);
}

Mod Mod::operator+(Mod const& y) const {
    return Mod(*this) += y;
}

Mod& Mod::operator+=(Mod const& o){
    mod_mismatch_check(o);
    value += o.value;
    if (value >= mod)
	value -= mod;
    return *this;
}

Mod Mod::operator-(Mod const& y) const {
    return Mod(*this) -= y;
}

Mod& Mod::operator-=(Mod const& o) {
    mod_mismatch_check(o);
    if (value < o.value)
	value += mod;
    value -= o.value;
    return *this;
}

Mod Mod::operator*(Mod const& y) const {
    return Mod(*this) *= y;
}

Mod& Mod::operator*=(Mod const& o) {
    mod_mismatch_check(o);
    value *= o.value;
    if (mod)
	value %= mod;
    return *this;
}

Mod Mod::operator/(Mod const& y) const {
    return Mod(*this) /= y;
}

Mod& Mod::operator/=(Mod const& o) {
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

B operator<(Mod const& x, Mod const& y) {
    x.mod_mismatch_check(y);
    return x.value < y.value;
}

// CRT = Chinese Remainder Theorem
Optional<Mod> Mod::uniqueMerge(std::vector<Mod> const& mods) {
    auto n = mods.size();
    // verify all inputs are compatible
    for (UI i = 0; i < n; ++i) {
	for (UI j = i + 1; j < n; ++j) {
	    Mod const& x = mods[i];
	    Mod const& y = mods[j];
	    auto g = my_gcd(x.get_mod(), y.get_mod());
	    Mod xg(g, static_cast<SL>(x));
	    Mod yg(g, static_cast<SL>(y));
	    if (xg != yg) {
		return Failure(to_string(x) + " and " + to_string(y) + " cannot be merged: " + to_string(xg) + " vs " + to_string(yg));
	    }
	}
    }
    // Handle lcm = 0
    for (Mod const& x : mods) {
	if (x.get_mod() == 0) {
	    return x;
	}
    }
    
    std::map<UI, Mod> primeToBiggestPowerMod;
    for (Mod const& x : mods) {
	Prime_Factorization modFac(x.get_mod());
	for (auto [p_ul, e] : modFac.primes_and_exponents()) {
	    auto p = static_cast<UI>(p_ul); // safe since we're factoring a UI
	    auto pe = my_pow(p, e);
	    auto it = primeToBiggestPowerMod.find(p);
	    if (it == primeToBiggestPowerMod.end() || it->second.get_mod() < pe) {
		primeToBiggestPowerMod.insert_or_assign(p, Mod(pe, static_cast<SL>(x)));
	    }
	}
    }
    UI lcm = 1;
    for (auto const& [p, m] : primeToBiggestPowerMod) {
	auto constexpr iUB = std::numeric_limits<UI>::max();
	if (iUB / lcm < m.get_mod()) {
	    return Failure("Combined Mod too big for unsigned int: " + to_string(primeToBiggestPowerMod));
	}
	lcm *= m.get_mod();
    }
    
    // a mod A
    // b mod B
    // c mod C
    // where A, B, and C are coprime
    // aBC*((BC)^(-1) mod A) + bAC*((AC)^(-1) mod B) + cAB*((AB)^(-1) mod C)
    // Letting N = ABC, the solution is:
    // ∑a(N/A)((N/A)^(-1) mod A) for each (a,A) pair
    Mod result(lcm, 0);
    for (auto const& [p, m] : primeToBiggestPowerMod) {
	auto a = static_cast<UI>(m);
	auto A = m.get_mod();
	result += Mod(lcm, a) * (lcm / A) * static_cast<UI>(Mod(A, lcm/A)^(-1));
    }
    return result;
}

Mod::GlobalModSentinel::GlobalModSentinel(UI newMod)
    : prev(Mod::globalMod) {
    Mod::globalMod = newMod;
}

Mod::GlobalModSentinel::~GlobalModSentinel() {
    Mod::globalMod = prev;
}

