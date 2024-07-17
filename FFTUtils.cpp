#include "fftw.hpp"
#include <stdexcept>
#include <utility>

#include "ExitUtils.hpp"
#include "FFTData.hpp"
#include "FFTUtils.hpp"
#include "Function.hpp"
#include "PrimeUtils.hpp"
#include "TypeUtils.hpp"
#include "U_Map.hpp"
#include "Vec.hpp"

static Vec<Mod> withModAndLength(Vec<Mod> const& x, UI p, UI len) {
    Vec<Mod> result;
    result.reserve(len);
    for (Mod const& m : x) {
	result.push_back(Mod(p, static_cast<UI>(m)));
    }
    result.resize(len, Mod(p, 0));
    return result;
}

static Vec<Mod> nttImplPow2(Vec<Mod> x, UI exp2, Mod const& r) {
    // Required invariants:
    // x has length 2^exp2
    // r has order 2^exp2:
    //   r^(2^exp2) = 1, and r^k ≠ 1 for 0 < k < 2^exp2
    // r and all elements of x have the same modulus

    UI n = 1U << exp2;
    Vec<Mod> rPow;
    Mod curRPow = r.create(1);
    for (UI i = 0; i < n/2; ++i) {
	rPow.push_back(curRPow);
	curRPow *= r;
    }
    for (UI step = 0; step < exp2; ++step) {
	UI diff = 1U << (exp2 - 1U - step);
	for (UI i = 0; i < n; ++i) {
	    if (!(i & diff)) {
		UI j = i | diff;
		Mod top = x[i] + x[j];
		Mod bot = (x[i] - x[j]) * rPow[(i & (diff - 1U)) << step];
		x[i] = top;
		x[j] = bot;
	    }
	}
    }

    return x;
}

static Vec<Mod> inttImplPow2(Vec<Mod> x, UI exp2, Mod const& r) {
    // Required invariants:
    // x has length 2^exp2
    // r has order 2^exp2:
    //   r^(2^exp2) = 1, and r^k ≠ 1 for 0 < k < 2^exp2
    // r and all elements of x have the same modulus
    // r is the same value passed to all ffts that contributed to x

    
    UI n = 1U << exp2;
    Vec<Mod> rInvPow;
    Mod rInv = 1 / r;
    Mod curRInvPow = r.create(1);
    for (UI i = 0; i < n/2; ++i) {
	rInvPow.push_back(curRInvPow);
	curRInvPow *= rInv;
    }
    for (UI step = exp2; step--; ) {
	UI diff = 1U << (exp2 - 1U - step);
	for (UI i = 0; i < n; ++i) {
	    if (!(i & diff)) {
		UI j = i | diff;
		x[j] *= rInvPow[(i & (diff - 1U)) << step];
		Mod top = x[i] + x[j];
		Mod bot = x[i] - x[j];
		x[i] = top;
		x[j] = bot;
	    }
	}
    }

    auto nInv = 1 / r.create(n);
    for (UI i = 0; i < n; ++i) {
	x[i] *= nInv;
    }

    return x;
}

namespace FFT {
    
    template<class In, class Out> using Map_1D_Type = U_Map<SZ, FFT_1D_Container<In, Out>>;
    static Map_1D_Type<D, CD> r2c_plans;
    static Map_1D_Type<CD, D> c2r_plans;

    Vec<CD> fft(Vec<D> const& in, B save) {
	SZ const n = in.size();
	auto [it, isNew] = r2c_plans.try_emplace(n, n, fftw_plan_dft_r2c_1d, save);
	Vec<CD> result = it->second(in);
	if (isNew && !save) {
	    r2c_plans.erase(it);
	}
	return result;
    }
    Vec<D> ifft(SZ const n, Vec<CD> const& in, B save) {
        auto [it, isNew] = c2r_plans.try_emplace(n, n, fftw_plan_dft_c2r_1d, save);
	Vec<D> result = it->second(in);
	if (isNew && !save) {
	    c2r_plans.erase(it);
	}
	for (D& x : result) {
	    x /= n;
	}
	return result;
    }

    Vec<D> convolution(Vec<D> x, Vec<D> y, SZ goal, B save) {
	if (x.size() == 0 || y.size() == 0)
	    return {};
	SZ size = x.size() + y.size() - 1;
	if (goal == static_cast<SZ>(-1))
	    goal = size;
	x.resize(size, 0);
	y.resize(size, 0);
	Vec<CD> fft_x = fft(x, save);
	Vec<CD> fft_y = fft(y, save);
	SZ fft_size = fft_x.size();
	for (SZ i = 0; i < fft_size; ++i)
	    fft_x[i] *= fft_y[i];
	Vec<D> result = ifft(size, fft_x, save);
	result.resize(goal);
	return result;
    }

    Vec<BigInt> convolution(Vec<BigInt> const& x, Vec<BigInt> const& y) {
	// TODO: implement this
	(void)x;
	(void)y;
	throw_exception<std::logic_error>(std::string(__FUNCTION__) + " unimplemented");
    }
    
    Vec<Mod> convolution(Vec<Mod> const& x, Vec<Mod> const& y) {
	if (x.size() == 0 || y.size() == 0) {
	    return {};
	}
	UI mod = x[0].get_mod();
	if (mod == 0) {
	    Vec<BigInt> bx;
	    Vec<BigInt> by;
	    for (auto const& xx : x) {
		bx.push_back(static_cast<SL>(xx));
	    }
	    for (auto const& yy : y) {
		by.push_back(static_cast<SL>(yy));
	    }
	    auto bresult = convolution(bx, by);
	    Vec<Mod> result;
	    for (auto const& br : bresult) {
		result.push_back(Mod(mod, *br.convert<SL>()));
	    }
	    return result;
	}
	// pick prime p, int d, int r with:
	// s = 2^d
	// s | p-1
	// s ≥ length(x) + length(y)
	// r is a primitive root mod p (r^(p-1) is the smallest positive power of r that ≡ 1 mod p)
	// Then we can perform the Number Theoretic Tranform (NTT)
	
	// We (at first) ignore the original mod and compute the overall convolution
	//   as if the inputs were positive integers.
	// The only useful fact we take on the input is that these integers are < 2^32.
	// We compute this mod 3 large primes, to determine a result mod their product N = p_1 * p_2 * p_3,
	//   which is big enough that any possible integer in the convolution result is < N, so the results mod
	//   each of the 3 primes determines the result exactly (Chinese remainder theorem).
	// out ≡ a_1 mod p_1
	// out ≡ a_2 mod p_2
	// out ≡ a_3 mod p_3
	// out ≡ a_1 * p_2 * (p_2 ^ -1 mod p_1) * p_3 * (p_3 ^ -1 mod p_1) +
	//       a_2 * p_1 * (p_1 ^ -1 mod p_2) * p_3 * (p_3 ^ -1 mod p_2) +
	//       a_3 * p_1 * (p_1 ^ -1 mod p_3) * p_2 * (p_2 ^ -1 mod p_3) mod p_1 * p_2 * p_3
	

	auto outputSize = x.size() + y.size() - 1;
	UI d = static_cast<UI>(std::ceil(std::log2(outputSize)));
	if (d > ConvolutionConsts::maxSizeExp2) {
	    throw_exception<std::invalid_argument>("convolution for Mod supports max sum of lengths = 2^" + to_string(ConvolutionConsts::maxSizeExp2) + " = " + to_string(1u << ConvolutionConsts::maxSizeExp2) + " < " + to_string(outputSize) + " (the sum of your input sizes - 1)");
	}
	Vec<BigInt> bigResult(outputSize, 0);
	BigInt bigMod = 1;
	for (auto const& info : FFT::ConvolutionConsts::biggest) {
	    auto e = info.fExp2;
	    auto p = info.fPrime;
	    Mod dx = Mod(p, info.fRootPow2) ^ (1u << (e - d)); // element mod p with order 2^d
	    UI len = 1u << d;
	    bigMod *= p;
	    Vec<Mod> fx = nttImplPow2(withModAndLength(x, p, len), d, dx);
	    {
		Vec<Mod> fy = nttImplPow2(withModAndLength(y, p, len), d, dx);
		for (UI i = 0; i < len; ++i) {
		    fx[i] *= fy[i];
		}
	    }
	    auto convModP = inttImplPow2(fx, d, dx);
	    for (UI i = 0; i < outputSize; ++i) {
		BigInt forResult = static_cast<UI>(convModP[i]);
		for (auto const& info2 : FFT::ConvolutionConsts::biggest) {
		    UI p2 = info2.fPrime;
		    if (p != p2) {
			forResult *= p2;
			forResult *= static_cast<UI>(1 / Mod(p, p2));
		    }
		}
		bigResult[i] += forResult;
	    }
	    for (UI i = static_cast<UI>(outputSize) + 1; i < len; ++i) {
		if (convModP[i] != 0) {
		    throw_exception<std::logic_error>("convolution computed a nonzero element too far into the array - the impl is wrong!");
		}
	    }
	}
	Vec<Mod> result(outputSize, Mod(mod, 0));
	for (UI i = 0; i < outputSize; ++i) {
	    result[i] = *(((bigResult[i] % bigMod) % mod).convert<UI>());
	}
	return result;
    }

    namespace ConvolutionConsts {
	Vec<PrimeAndRootPow2> findPrimes(UI numToFind) {
	    Vec<PrimeAndRootPow2> result;
	    for (UI e = 31; ; --e) {
		// iterate mult big-to-small to get larger primes first
		for (UI mult = (1u << (32-e)) + 1; mult > 2; ) {
		    mult -= 2;
		    UI p = mult * (1u << e) + 1;
		    if (is_prime(p)) {
			UI r = primitiveRoot(p);
			UI rootOrderPow2 = static_cast<UI>(Mod(p, r) ^ mult);
			result.push_back(PrimeAndRootPow2(e, mult, r, rootOrderPow2));
			if (result.size() >= numToFind) {
			    return result;
			}
		    }
		}
	    }
	}
    }
}
