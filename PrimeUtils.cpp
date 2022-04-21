#include "Class.hpp"
#include "ExitUtils.hpp"
#include "Math.hpp"
#include "Mod.hpp"
#include "PrimeUtils.hpp"
#include "Str.hpp"
#include "TypeUtils.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <random>

Vec<UL> const tiny_primes{2,3,5,7,11,13,17,19,23,29,31,37,41};

Vec<B> isPrimeUpTo(UI const n) {
    Vec<B> prime(n+1,true);
    SZ maxI = static_cast<SZ>(sqrt(n));
    for (UI i = 2; i <= maxI; ++i)
	if (prime[i])
	    for (UI j = i*i-i; j <= n-i; )
		prime[j+=i] = false;
    prime[0] = false;
    if (n >= 1)
	prime[1] = false;
    return prime;
}

Vec<UI> primesUpTo(UI const n) {
    Vec<UI> primes;
    auto isPrime = isPrimeUpTo(n);
    for (UI i = 2; i <= n; ++i)
	if (isPrime[i])
	    primes.push_back(i);
    return primes;
}

Vec<UI> phiUpTo(UI const n) {
    auto result = somePrimeFactorUpTo(n);
    if (n >= 1) {
	result[1] = 1;
    }
    for (UI i = 2; i <= n; ++i) {
	UI p = result[i];
	result[i] = result[i/p] * (i/p%p ? p-1 : p);
    }
    return result;
}

Vec<UI> somePrimeFactorUpTo(UI const n) {
    Vec<UI> result(n+1,0);
    UI maxI = static_cast<UI>(std::sqrt(n));
    for (UI i = 2; i <= maxI; ++i) {
	if (!result[i]) {
	    for (UI j = i; j <= n; j += i) {
		result[j] = i;
	    }
	}
    }
    for (UI i = maxI+1; i <= n; ++i) {
	if (!result[i]) {
	    result[i] = i;
	}
    }
    return result;
}

Vec<Vec<UI>> allFactorsUpTo(UI const n) {
    auto pFac = somePrimeFactorUpTo(n);
    Vec<Vec<UI>> factors(n+1);
    factors[1] = {1};
    for (UI i = 2; i <= n; ++i) {
	auto p = pFac[i];
	UI e = 0;
	UI j = i;
	for ( ; j % p == 0; j /= p) {
	    ++e;
	}
	UI mult = 1;
	// faster with the loops in this order, for some reason
	for (UI exp = 0; exp <= e; ++exp) {
	    for (UI f : factors[j]) {
		factors[i].push_back(f * mult);
	    }
	    mult *= p;
	}
    }
    return factors;
}

// Miller-Rabin primality test
B is_prime(UL const n) {
    static auto const low_results = isPrimeUpTo(1e6);
    if (n < low_results.size())
	return low_results[n];
    
    UI pow2 = 0;
    UL lowPow = n-1;
    for (; (lowPow & 1UL) == 0; lowPow >>= 1)
	++pow2;

    // passes when n < 3,317,044,064,679,887,385,961,981
    // https://en.wikipedia.org/wiki/Miller%E2%80%93Rabin_primality_test
    for (UL const base : tiny_primes) {
	if (n == base)
	    return true;
	if (n % base == 0)
	    return false;
        UL start = 1;
	UL mult = base;
        U128 m = 1; // used in oder to perform 128-bit calculations
	for (UL pow = lowPow; pow; pow >>= 1) {
	    if (pow & 1)
		start = m*start*mult % n;
	    mult = m*mult*mult % n;
	}
	if (start != 1) {
	    B hit_n_1 = false; // n-1
	    for (UL local_pow2 = pow2; local_pow2 > 0; --local_pow2) {
		if (start == n-1) {
		    hit_n_1 = true;
		    break;
		}
		// something != +-1 squared to 1 -> not prime
		if (start == 1) {
		    return false;
		}
		start = m*start*start % n;
	    }
	    // something^(n-1) > 1 -> not prime
	    // a^(p-1) = 0 or 1 mod p
	    if (!hit_n_1) {
		return false;
	    }
	}
    }

    return true;
}

B is_composite(UL const n) {
    return n > 1 && !is_prime(n);
}

// Pollard's Rho Algorithm:
// https://www.geeksforgeeks.org/pollards-rho-algorithm-prime-factorization/
UL find_factor_of_composite(UL const n) {
    for (UL const p : tiny_primes)
	if (n%p == 0)
	    return p;
    static auto rand_int = std::chrono::system_clock::now().time_since_epoch().count();
    static std::mt19937 rng(static_cast<UI>(rand_int));
    std::uniform_int_distribution<UL> dist(1,n-1);
    while (true) {
        UL x = dist(rng);
	UL c = dist(rng);
        UL y = x;
	UL factor = 1;
	while (factor == 1 && (x || y || c)) {
	    x = (static_cast<U128>(x)*x+c)%n;
	    y = (static_cast<U128>(y)*y+c)%n;
	    y = (static_cast<U128>(y)*y+c)%n;
	    factor = my_gcd<UL>(x>y?x-y:y-x,n);
	}
	if (factor < n)
	    return factor;
    }
}

UL find_factor(UL const n) {
    if (!is_composite(n))
	return n;
    return find_factor_of_composite(n);
}

UL find_prime_factor(UL n) {
    while (is_composite(n))
	n = find_factor_of_composite(n);
    return n;
}

Vec<UL> prime_factor_list(UL const n) {
    if (n == 0)
	throw_exception<std::invalid_argument>("Cannot factor 0");
    Vec<UL> result;
    if (n == 1)
	return result;
    UL processing[64];
    SZ p_size = 1;
    processing[0] = n;
    while (p_size) {
	UL x = processing[--p_size];
	if (is_prime(x)) {
	    result.push_back(x);
	} else {
	    UL f = find_factor_of_composite(x);
	    processing[p_size++] = f;
	    processing[p_size++] = x/f;
	}
    }
    std::sort(result.begin(), result.end());
    return result;
}

Prime_Power::Prime_Power(UL const prime, UL const exponent)
    : fPrime(prime)
    , fExponent(exponent) {
}

Prime_Factorization::Prime_Factorization(UL const n) {
    UL prev = 0;
    B started = false;
    for (UL const f : prime_factor_list(n)) {
	if (f != prev || !started) {
	    fPowers.emplace_back(f,1);
	    prev = f;
	    started = true;
	} else {
	    ++fPowers.back().fExponent;
	}
    }
}

Prime_Factorization::Prime_Factorization(Vec<Prime_Power> powers)
    : fPowers(std::move(powers)) {
}

Prime_Factorization::Prime_Factorization()
    : Prime_Factorization(1) {
}

Prime_Factorization::operator UL() const {
    UL result = 1;
    for (Prime_Power const& pp : fPowers)
	for (UL f = pp.fPrime, pow = pp.fExponent; pow > 0; f*=f, pow >>= 1)
	    if (pow & 1)
		result *= f;
    return result;
}

Prime_Factorization::operator BigInt() const {
    BigInt result = 1;
    for (Prime_Power const& pp : fPowers) {
	BigInt f = pp.fPrime;
	for (UL pow = pp.fExponent; pow > 0; f*=f, pow >>= 1)
	    if (pow & 1)
		result *= f;
    }
    return result;
}

void Prime_Factorization::list_factors(Vec<UL>& result, SZ const index, UL value) const {
    if (index == fPowers.size()) {
	result.push_back(value);
	return;
    }

    // exponent = 0;
    list_factors(result, index+1, value);

    // exponent = 1 ... max
    UL prime = fPowers[index].fPrime;
    UL exp = fPowers[index].fExponent;
    for (UL count = 1; count <= exp; ++count) {
	value *= prime;
	list_factors(result, index+1, value);
    }
}

Vec<UL> Prime_Factorization::factors() const {
    Vec<UL> result;
    list_factors(result);
    return result;
}

void Prime_Factorization::list_factors_prime_factorizations(
    Vec<Prime_Factorization>& result,
    Prime_Factorization& new_pf,
    SZ const index) const {
    if (index == fPowers.size()) {
	result.push_back(new_pf);
	return;
    }

    // exponent = 0
    list_factors_prime_factorizations(result,new_pf,index+1);

    // exponent = 1 ... max
    new_pf.fPowers.emplace_back(fPowers[index].fPrime,0);
    auto const max_exp = fPowers[index].fExponent;
    for (UL exp = 1; exp <= max_exp; ++exp) {
	++new_pf.fPowers.back().fExponent;
	list_factors_prime_factorizations(result,new_pf,index+1);
    }

    // reset
    new_pf.fPowers.pop_back();
}

Vec<Prime_Power> const& Prime_Factorization::primes_and_exponents() const {
    return fPowers;
}

Optional<Prime_Power> Prime_Factorization::max_prime_and_power() const {
    if (fPowers.size() == 0)
	return Failure("max_prime not available for 1");
    return fPowers.back();
}

Prime_Factorization Prime_Factorization::square() const {
    auto sqPE = primes_and_exponents();
    for (auto&& [p, e] : sqPE) {
	e *= 2;
    }
    return sqPE;
}

// {x, y} where n = x^2 * y and y is square-free
std::pair<Prime_Factorization, Prime_Factorization> Prime_Factorization::splitSquareFree() const {
    Vec<Prime_Power> x;
    Vec<Prime_Power> y;
    for (auto const& [p, e] : primes_and_exponents()) {
	if (e > 1) {
	    x.emplace_back(p, e / 2);
	}
	if (e % 2) {
	    y.emplace_back(p, 1);
	}
    }
    return {x, y};
}

Vec<Prime_Factorization> Prime_Factorization::factors_prime_factorizations() const {
    Vec<Prime_Factorization> result;
    Prime_Factorization empty;
    list_factors_prime_factorizations(result, empty);
    return result;
}

Vec<UL> factors(UL const n) {
    return Prime_Factorization(n).factors();
}

Vec<UL> sorted_factors(UL const n) {
    auto list = factors(n);
    std::sort(list.begin(), list.end());
    return list;
}

Optional<UI> sqrtModPrime(SL value, UI p) {
    value %= p;
    if (value < 0) {
	value += p;
    }

    auto x = static_cast<UI>(value);
    {
	auto rtX = static_cast<UI>(std::sqrt(x));
	if (rtX * rtX == x) {
	    return rtX;
	}
    }

    // faster than factorization to determine legendre via quadratic reciprocity
    auto legendre = Mod(p, x) ^ ((p-1) / 2);

    FAIL_IF(legendre != 1);
    
    if (p % 4 == 3) {
	return static_cast<UI>(static_cast<SL>((Mod(p, x) ^ ((p+1) / 4))));
    }

    // https://en.wikipedia.org/wiki/Tonelli%E2%80%93Shanks_algorithm
    auto Q = p - 1;
    UI S = 0;
    for ( ; Q % 2 == 0; Q /= 2) {
	++S;
    }
    UI nonresidue = 2;
    for ( ; (Mod(p, nonresidue) ^ ((p-1) / 2)) == 1; ++nonresidue) {}
    auto n = x;
    auto z = nonresidue;
    auto M = S;
    auto c = Mod(p, z) ^ Q;
    auto t = Mod(p, n) ^ Q;
    auto R = Mod(p, n) ^ ((Q+1)/2);
    while (t != 1) {
	UI i = 0;
	for (auto T = t; T != 1; T *= T) {
	    ++i;
	}
	auto b = c ^ (1U << (M-i-1));
	t *= b^2;
	R *= b;
    }
    return static_cast<UI>(static_cast<SL>(R));
}

template<> std::ostream& Class<Prime_Power>::print(std::ostream& os, Prime_Power const& pp) {
    return os << pp.fPrime << "^" << pp.fExponent;
}
template<> Optional<Prime_Power> Class<Prime_Power>::parse(std::istream& is) {
    Resetter resetter{is};
    auto pair = Class<std::pair<UL,UL>>::parse(is);
    if (!pair)
	return Failure(pair.cause());
    resetter.ignore();
    return {pair->first,pair->second};
}
template<> Str Class<Prime_Power>::name() {
    return "Prime_Power";
}
template<> Str Class<Prime_Power>::format() {
    return Class<std::pair<UL,UL> >::format();
}

template<> std::ostream& Class<Prime_Factorization>::print(std::ostream& os, Prime_Factorization const& pf) {
    os << static_cast<BigInt>(pf);
    if (pf.fPowers.size()) {
	os << " (";
	bool started = false;
	for (auto const& pp : pf.fPowers) {
	    if (started)
		os << " * ";
	    os << pp;
	    started = true;
	}
	os << ")";
    }
    return os;
}

template<> Optional<Prime_Factorization> Class<Prime_Factorization>::parse(std::istream& is) {
    Resetter resetter{is};
    auto unsignedLong = Class<UL>::parse(is);
    if (unsignedLong) {
	resetter.ignore();
	return *unsignedLong;
    }
    auto primePowerList = Class<Vec<Prime_Power> >::parse(is);
    if (primePowerList) {
	resetter.ignore();
	return *primePowerList;
    }
    return Failure("Failed to parse as unsigned long: " + unsignedLong.cause() + " and failed to parse as prime power list: " + primePowerList.cause());
}

template<> Str Class<Prime_Factorization>::name() {
    return "Prime_Factorization";
}

template<> Str Class<Prime_Factorization>::format() {
    return Class<UL>::format() + " | " + Class<Vec<Prime_Power> >::format();
}
