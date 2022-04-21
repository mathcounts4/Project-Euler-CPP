#pragma once

#include "BigInt.hpp"
#include "Class.hpp"
#include "Optional.hpp"
#include "Str.hpp"
#include "TypeUtils.hpp"
#include "Vec.hpp"

Vec<B> isPrimeUpTo(UI const n);
Vec<UI> primesUpTo(UI const n);
Vec<UI> phiUpTo(UI const n);
Vec<UI> somePrimeFactorUpTo(UI const n);
Vec<Vec<UI>> allFactorsUpTo(UI const n);

B is_prime(UL const n);
B is_composite(UL const n);

// Given a composite number, returns a proper factor
UL find_factor_of_composite(UL const n);

// If n is composite, returns a proper factor, otherwise returns n.
UL find_factor(UL const n);
UL find_prime_factor(UL n);

// sorted list of prime factors, including duplicates
Vec<UL> prime_factor_list(UL const n);

struct Prime_Power {
    UL fPrime;
    UL fExponent;

    Prime_Power(UL const prime, UL const exponent);
};

class Prime_Factorization {
  private:
    Vec<Prime_Power> fPowers;
    friend struct Class<Prime_Factorization>;

    void list_factors(Vec<UL>& result, SZ const index = 0, UL value = 1) const;

    void list_factors_prime_factorizations(
	Vec<Prime_Factorization>& result,
	Prime_Factorization& new_pf,
	SZ const index = 0) const;
  public:
    // Do not construct with 0
    Prime_Factorization(UL const n);

    // Does not validate Prime_Power list
    Prime_Factorization(Vec<Prime_Power> powers);

    // Default: 1
    Prime_Factorization();

    // Multiplies out to the value
    explicit operator UL() const;
    explicit operator BigInt() const;
    
    // sorted: if a|b|n, a preceeds b in the list
    Vec<UL> factors() const;
    Vec<Prime_Factorization> factors_prime_factorizations() const;

    // returns the prime powers of this factorization
    Vec<Prime_Power> const& primes_and_exponents() const;
    
    // returns the biggest prime and its power, if one exists
    Optional<Prime_Power> max_prime_and_power() const;

    Prime_Factorization square() const;

    // {x, y} where n = x^2 * y and y is square-free
    std::pair<Prime_Factorization, Prime_Factorization> splitSquareFree() const;
};

// sorted: if a|b|n, a preceeds b in the list
Vec<UL> factors(UL const n);

// sorted: if a<b, a preceeds b in the list
Vec<UL> sorted_factors(UL const n);

// returns one of the square roots of value mod p, if 1 or 2 exist
// undefined behavior if p is not prime
Optional<UI> sqrtModPrime(SL value, UI p);

/* Class<Prime_Power> */
template<> std::ostream& Class<Prime_Power>::print(std::ostream& os, Prime_Power const& pp);
template<> Optional<Prime_Power> Class<Prime_Power>::parse(std::istream& is);
template<> Str Class<Prime_Power>::name();
template<> Str Class<Prime_Power>::format();

/* Class<Prime_Factorization> */
template<> std::ostream& Class<Prime_Factorization>::print(std::ostream& os, Prime_Factorization const& pf);
template<> Optional<Prime_Factorization> Class<Prime_Factorization>::parse(std::istream& is);
template<> Str Class<Prime_Factorization>::name();
template<> Str Class<Prime_Factorization>::format();
