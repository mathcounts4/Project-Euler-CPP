#pragma once

// uses fftw3 from http://www.fftw.org/ to expose fft and convolution functions
// make sure to link against the library using -l

#include <complex>

#include "BigInt.hpp"
#include "Class.hpp"
#include "Math.hpp"
#include "Mod.hpp"
#include "TypeUtils.hpp"
#include "Vec.hpp"

namespace FFT {
    
    using CD = std::complex<D>;
    
    Vec<CD> fft(Vec<D> const& in, B save = false);
    Vec<D> ifft(SZ const n, Vec<CD> const& in, B save = false);

    Vec<D> convolution(Vec<D> x, Vec<D> y, SZ goal = SZ(-1), B save = false);
    Vec<BigInt> convolution(Vec<BigInt> const& x, Vec<BigInt> const& y);
    Vec<Mod> convolution(Vec<Mod> const& x, Vec<Mod> const& y);

    namespace ConvolutionConsts {
	struct PrimeAndRootPow2 {
	    UI fExp2;       // e
	    UI fMultFactor; // m
	    UI fPrime;      // p
	    UI fPrimRoot;   // r
	    UI fRootPow2;   // x
	    // p = m * 2^e + 1
	    // p is a large prime < 2^32, ideally close to 2^32
	    // r is a primitive root mod p:
	    //   the order of r mod p is p-1:
	    //   r^(p-1) ≡ 1, and r^k ≢ 1 for 0 < k < p-1
	    // x ≡ r^m mod p, so that the order of x mod p is 2^e

	    constexpr PrimeAndRootPow2(UI e, UI m, UI r, UI x)
		: fExp2(e)
		, fMultFactor(m)
		, fPrime(m * (1u << e) + 1u)
		, fPrimRoot(r)
		, fRootPow2(x) {}
	};

	Vec<PrimeAndRootPow2> findPrimes(UI numToFind);

	constexpr PrimeAndRootPow2 biggest[3] = {
	    PrimeAndRootPow2(30,  3, 5,        125),
	    PrimeAndRootPow2(28, 13, 3,    1594323),
	    PrimeAndRootPow2(27, 29, 3, 1094924467),
	};
	constexpr UI maxSizeExp2 = std::min({
		biggest[0].fExp2,
		biggest[1].fExp2,
		biggest[2].fExp2
	    });
    }
}

template<>
struct Class<FFT::ConvolutionConsts::PrimeAndRootPow2> {
    using T = FFT::ConvolutionConsts::PrimeAndRootPow2;
    static std::ostream& print(std::ostream& oss, T const& t) {
	return oss << t.fMultFactor << " * 2^" << t.fExp2 << " + 1 = " << t.fPrime << " with primRoot = " << t.fPrimRoot << " and " << t.fRootPow2 << " with order 2^" << t.fExp2;
    }
    static Optional<T> parse(std::istream& /* is */) {
	return Failure("unimplemented");
    }
    static std::string name() {
	return "PrimeAndRootPow2";
    }
    static std::string format() {
	return "<custom>";
    }
};
