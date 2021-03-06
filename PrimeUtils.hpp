#pragma once
#ifndef PRIME_UTILS
#define PRIME_UTILS

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <random>
#include <vector>

#include "MathUtils.hpp"

std::vector<bool> isPrimeUpTo(unsigned long n) {
    std::vector<bool> prime(n+1,true);
    std::size_t maxI = sqrt(n);
    for (std::size_t i = 2; i <= maxI; ++i)
	if (prime[i])
	    for (std::size_t j = i*i; j<=n; j+=i)
		prime[j] = false;
    prime[0] = false;
    if (n >= 1)
	prime[1] = false;
    return prime;
}

template<class T>
std::vector<T> primesUpTo(T n) {
    std::vector<T> primes;
    std::vector<bool> isPrime = isPrimeUpTo(n);
    for (std::size_t i = 2; i <= n; ++i)
	if (isPrime[i])
	    primes.push_back(i);
    return primes;
}

template<class T>
std::vector<T> phiUpTo(T n) {
    std::vector<T> phi(n+1,0);
    std::size_t maxI = sqrt(n);
    // at first, only store prime factors
    for (std::size_t i = 2; i <= maxI; ++i)
	if (!phi[i])
	    for (std::size_t j = i; (j+=i)<=n; )
		phi[j] = i;
    // then calculate phi
    for (std::size_t i = 2; i <= n; ++i) {
	T p = phi[i];
	phi[i] = p ? phi[i/p] * (i/p%p ? p-1 : p) : i-1;
    }
    return phi;
}

// returns if n is likely prime using the Miller-Rabin primality test
bool is_likely_prime(unsigned long n)
{
    static std::vector<bool> const low_results(isPrimeUpTo(1e6));
    if (n < low_results.size())
	return low_results[n];
    
    unsigned int pow2 = 0;
    unsigned long lowPow = n-1;
    for (; lowPow%2 == 0; lowPow /= 2)
	++pow2;

    // passes when n < 3,317,044,064,679,887,385,961,981
    // https://en.wikipedia.org/wiki/Miller%E2%80%93Rabin_primality_test
    for (unsigned long const base : {2,3,5,7,11,13,17,19,23,29,31,37,41})
    {
	if (n == base)
	    return true;
	if (n % base == 0)
	    return false;
	unsigned long start = 1;
	unsigned long mult = base;
	__uint128_t m = 1;
	for (unsigned long pow = lowPow; pow > 0; pow /= 2)
	{
	    if (pow%2)
		start = m*start*mult % n;
	    mult = m*mult*mult % n;
	}
	if (start != 1)
	{
	    for (unsigned long local_pow2 = pow2; local_pow2 > 0; --local_pow2)
	    {
		if (start == n-1)
		{
		    start = 1;
		    break;
		}
		// something != +-1 squared to 1 -> not prime
		if (start == 1)
		    return false;
		start = m*start*start % n;
	    }
	    // something^(n-1) > 1 -> not prime
	    // a^(p-1) = 0 or 1 mod p
	    if (start > 1)
		return false;
	}
    }

    return true;
}

// attempts to find a NOT NECESSARILY PRIME factor > 1 of the input number n
// Uses Pollard's Rho Algorithm:
// https://www.geeksforgeeks.org/pollards-rho-algorithm-prime-factorization/
// does not check if n is prime (if so, runs forever)
// If n < 2, returns 1
unsigned long find_factor_sure(unsigned long n)
{
    if (n < 2)
	return 1;
    if (n % 2 == 0)
	return 2;
    auto current_int = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng(current_int);
    std::uniform_int_distribution<unsigned long> dist(1,n-1);
    while (true)
    {
        __uint128_t x = dist(rng);
	unsigned long c = dist(rng);
        __uint128_t y = x;
	unsigned long factor = 1;
	while (factor == 1 && (x > 0 || y > 0 || c > 0))
	{
	    x = (x*x+c)%n;
	    y = (y*y+c)%n;
	    y = (y*y+c)%n;
	    factor = my_gcd<unsigned long>(x>y?x-y:y-x,n);
	}
	if (factor < n)
	    return factor;
    }
}

// attempts to find a factor of the input number n
// after checking if n is prime or not, in which case 1 is returned.
unsigned long find_factor_unsure(unsigned long n)
{
    if (is_likely_prime(n) || n < 2)
	return 1;
    return find_factor_sure(n);
}

// finds a prime factor of the input number n
// if n < 2, returns 1
unsigned long find_prime_factor(unsigned long n)
{
    if (n < 2)
	return 1;
    while (!is_likely_prime(n))
	n = find_factor_sure(n);
    return n;
}

namespace
{
    void add_prime_factors_to_list(std::vector<unsigned long>& list,
				   unsigned long n)
    {
	if (is_likely_prime(n))
	{
	    list.push_back(n);
	}
	else if (n > 1)
	{
	    unsigned long f = find_factor_sure(n);
	    add_prime_factors_to_list(list,f);
	    add_prime_factors_to_list(list,n/f);
	}
    }
}

// returns an unsorted list of prime factors on n, with duplicates
// for 0 or 1, returns an empty list
std::vector<unsigned long> prime_factor_list(unsigned long n)
{
    std::vector<unsigned long> list;
    add_prime_factors_to_list(list,n);
    return list;
}

using Prime_Fac = std::vector<std::array<unsigned long,2> >;

// returns a vector of pairs (prime, exponent) that make up n
Prime_Fac prime_factor_pairs(unsigned long n)
{
    std::vector<unsigned long> factors = prime_factor_list(n);
    std::sort(factors.begin(),factors.end());
    
    Prime_Fac pair_list;
    unsigned long prev = 0;
    for (unsigned long f : factors)
    {
	if (f != prev || pair_list.size() == 0)
	{
	    pair_list.push_back({{f,1}});
	    prev = f;
	}
	else
	{
	    ++pair_list.back()[1];
	}
    }

    return pair_list;
}

// returns the product of all the specified prime factors
// note that this cannot return 0
unsigned long to_number(Prime_Fac const & p)
{
    unsigned long result = 1;
    for (auto const & pair : p)
	for (unsigned long f = pair[0], pow = pair[1]; pow > 0; f*=f, pow/=2)
	    if (pow & 1)
		result *= f;
    return result;
}

namespace
{
    void list_factors(Prime_Fac const & list,
		      std::size_t const index,
		      unsigned long value,
		      std::vector<unsigned long> & result)
    {
	if (index < list.size())
	{
	    unsigned long const & prime = list[index][0];
	    unsigned long const & pow = list[index][1];
	    for (unsigned long count = 0; count <= pow; ++count)
	    {
		list_factors(list,index+1,value,result);
		value *= prime;
	    }
	}
	else
	{
	    result.push_back(value);
	}
    }
}

// returns a sorted vector of factors of the input given as prime factorization
std::vector<unsigned long> factors(Prime_Fac const & prime_factors)
{
    std::vector<unsigned long> result;
    list_factors(prime_factors,0,1,result);
    std::sort(result.begin(),result.end());
    return result;
}

// returns a sorted vector of factors of n
std::vector<unsigned long> factors(unsigned long n)
{
    return factors(prime_factor_pairs(n));
}

namespace
{
    void list_factors_prime_fac(Prime_Fac const & list,
				std::size_t const index,
				Prime_Fac & modifiable,
				std::vector<Prime_Fac> & result)
    {
	if (index < list.size())
	{
	    list_factors_prime_fac(list,
				   index+1,
				   modifiable,
				   result);
	    unsigned long const & prime = list[index][0];
	    unsigned long const & pow = list[index][1];
	    std::size_t modifiable_index = modifiable.size();
	    modifiable.push_back({{prime,0}});
	    while (++modifiable[modifiable_index][1] <= pow)
		list_factors_prime_fac(list,
				       index+1,
				       modifiable,
				       result);
	    modifiable.pop_back();
	}
	else
	{
	    result.push_back(modifiable);
	}
    }
}

// returns a somewhat-sorted list of the factors of the input given as prime factorization
// somewhat-sorted: if a|b|n and a<b, the prime factorization of a preceeds the prime factorization of b in the list.
std::vector<Prime_Fac> factors_prime_fac(Prime_Fac const & prime_factors)
{
    Prime_Fac factor_prime_fac;
    std::vector<Prime_Fac> result;
    list_factors_prime_fac(prime_factors,0,factor_prime_fac,result);
    return result;
}

std::vector<Prime_Fac> factors_prime_fac(unsigned long n)
{
    return factors_prime_fac(prime_factor_pairs(n));
}


#endif /* PRIME_UTILS */
