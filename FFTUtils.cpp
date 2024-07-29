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

static Vec<UI> withLength(Vec<Mod> const& x, UI len) {
    Vec<UI> result;
    result.reserve(len);
    for (Mod const& m : x) {
	result.push_back(static_cast<UI>(m));
    }
    result.resize(len, 0u);
    return result;
}

static Vec<UI> fntImplPow2(Vec<UI> x, UI exp2, Mod const& r) {
    // Required invariants:
    // x has length n = 2^exp2
    // r has order n:
    //   r^n = 1, and r^k ≠ 1 for 0 < k < n

    // https://en.wikipedia.org/wiki/Discrete_Fourier_transform_over_a_ring#Number-theoretic_transform
    // https://en.wikipedia.org/wiki/Talk%3ANumber-theoretic_transform
    // NTT = Number-theoretic transform
    // FNT = Fast Number Theoretic Transform
    
    // Calculates (treating x as a column vector)
    // y = M * x
    // where M is a n⨯n matrix of coefficients (0 ≤ i,j < n):
    // M_{i,j} = r^(bitrev_d(i) * j)
    // (d = exp2)
    // Note that this is slightly different from the normal DFT, where N_{i,j} = r^(i*j).
    // It would be easy to swap elements to get the DFT, but this step is unnecessary.
    // Notably, M has the same rows as N, but the order of rows is different.
    // This corresponds to a reordering of elements in y from the normal DFT.
    // But for convolution via DFT, you multiply element-wise after performing the DFT.
    // So the order of elements in the DFT doesn't matter, as long as it's consistent.

    // To show that this impl is correct, we define a sequence of functions f_e(a,b,j) for 0 ≤ e ≤ d, where
    // 0 ≤ a < 2^e
    // 0 ≤ b < 2^(d-e)
    // 0 ≤ j < n = 2^d
    // as:
    // f_e(a,b,j) = r^(b*j) if j ≡ a mod 2^e
    //              0       otherwise
    // and see that M_{i,j} = f_0(0,bitrev_d(i),j), as j ≡ 0 mod 2^0 is always true, so f_0(0,bitrev_d(i),j) = r^(bitrev_d(i) * j).
    // and see that I_n_{i,j} = f_d(i,0,j), as j ≡ i mod 2^d is true iff j = i, so f_d(i,0,j) = r^(0*j) = 1 when i = j, and 0 otherwise.
    // Iterating e from d to 0, we see this as a series of steps transforming I_n into M.
    // In reality, we're transforming I_n * x into M * x.
    // Row i = bitrev_{d-e}(b) * 2^e + a satisfies both endpoints above: e=0, a=0 -> i = bitrev_d(b) -> b = bitrev_d(i), and b = 0 -> i = a.
    // We show that f is satisfied by:
    // f_0    (0,        b,j) = r^(b*j)
    // f_{e>0}(a,        b,j) = [f_{e-1}(a,b,j) + r^(-a*2^(d-e)) * f_{e-1}(a,b+2^(d-e),j)] / 2
    // f_{e>0}(a+2^(e-1),b,j) = [f_{e-1}(a,b,j) - r^(-a*2^(d-e)) * f_{e-1}(a,b+2^(d-e),j)] / 2
    // Proof:
    // e=0, a=0 is satisfied by definition.
    // e>0 is proved by induction on e:
    //   f_e(a,b,j)
    //    = [f_{e-1}(a,b,j)                + r^(-a*2^(d-e)) * f_{e-1}(a,b+2^(d-e),j)] / 2
    //    = [r^(b*j) * (j ≡ a mod 2^(e-1)) + r^(-a*2^(d-e)) * r^((b+2^(d-e)) * j) * (j ≡ a mod 2^(e-1))] / 2
    //    = [r^(b*j)                       + r^(-a*2^(d-e)) * r^((b+2^(d-e)) * j)] * (j ≡ a mod 2^(e-1)) / 2
    //    = [r^(b*j)                       + r^(b*j) * r^(-a*2^(d-e)) * r^(2^(d-e) * j)] * (j ≡ a mod 2^(e-1)) / 2
    //    = [r^(b*j)                       + r^(b*j) * r^(2^(d-e) * (j-a))] * (j ≡ a mod 2^(e-1)) / 2
    //    = [1                             + r^(2^(d-e) * (j-a))] * r^(b*j) * (j ≡ a mod 2^(e-1)) / 2
    //      = 0 if j ≢ a mod 2^(e-1)
    //      = ? if j ≡ a + 2^(e-1) mod 2^e
    //        = [1 + r^(2^(d-e) * (j-a))] * r^(b*j) / 2
    //        = [1 + r^(2^(d-e) * (2^(e-1) + k*2^e))] * r^(b*j) / 2
    //        = [1 + r^(2^(d-1) + k*2^d)] * r^(b*j) / 2
    //        = [1 + r^(2^d)^k * r^(2^(d-1))] * r^(b*j) / 2
    //        = [1 + 1^k * r^(2^(d-1))] * r^(b*j) / 2
    //        = [1 + r^(2^(d-1))] * r^(b*j) / 2
    //        = [1 + r^(2^(d-1))] * [r^(2^(d-1)) - 1] * r^(b*j) / 2 / [r^(2^(d-1)) - 1] since r^(2^(d-1)) != 1
    //        = [r^(2^d) - 1] * r^(b*j) / 2 / [r^(2^(d-1)) - 1]
    //        = [1 - 1] * r^(b*j) / 2 / [r^(2^(d-1)) - 1]
    //        = 0
    //      = ? if j ≡ a mod 2^e
    //        = [1 + r^(2^(d-e) * (j-a))] * r^(b*j) / 2
    //        = [1 + r^(2^(d-e) * k*2^e)] * r^(b*j) / 2
    //        = [1 + r^(k*2^d)] * r^(b*j) / 2
    //        = [1 + r^(2^d)^k] * r^(b*j) / 2
    //        = [1 + 1^k] * r^(b*j) / 2
    //        = 2 * r^(b*j) / 2
    //        = r^(b*j)
    //  f_e(a+2^(e-1),b,j)
    //   = [f_{e-1}(a,b,j) - r^(-a*2^(d-e)) * f_{e-1}(a,b+2^(d-e),j)] / 2
    //   = f_{e-1}(a,b,j) - f_e(a,b,j)
    //   = r^(b*j) * (j ≡ a mod 2^(e-1)) - r^(b*j) * (j ≡ a mod 2^e)
    //   = r^(b*j) * ((j ≡ a mod 2^(e-1)) - (j ≡ a mod 2^e))
    //     = 0 if j ≢ a mod 2^(e-1)
    //     = 0 if j ≡ a mod 2^e
    //     = r^(b-j) if j ≡ a+2^(e-1) mod 2^e
    // Next, we see how this transformation plays out in terms of row indices i, instead of a and b:
    // i = bitrev_{d-e}(b) * 2^e + a = append(b_backwards,a), denoted as rev(b)#a
    // and the transformation has a with 1 less bit (e-1 bits):
    // f_0(0,b,j) = r^(b*j)
    // f_{e>0}(a,        b,j) = [f_{e-1}(a,b,j) + r^(-a*2^(d-e)) * f_{e-1}(a,b+2^(d-e),j)] / 2
    // f_{e>0}(a+2^(e-1),b,j) = [f_{e-1}(a,b,j) - r^(-a*2^(d-e)) * f_{e-1}(a,b+2^(d-e),j)] / 2
    // which can be written as:
    // f_{e>0}(rev(b)#(0#a),j)        = [f_{e-1}((rev(b)#0)#a,j) + r^(-a*2^(d-e)) * f_{e-1}(rev(b+2^(d-e))#a,j)] / 2
    // f_{e>0}(rev(b)#(a+2^(e-1)),j)  = [f_{e-1}((rev(b)#0)#a,j) - r^(-a*2^(d-e)) * f_{e-1}(rev(b+2^(d-e))#a,j)] / 2
    // and simplifying, we get:
    // f_{e>0}(rev(b)#0#a,j)  = [f_{e-1}(rev(b)#0#a,j) + r^(-a*2^(d-e)) * f_{e-1}(rev(b)#1#a,j)] / 2
    // f_{e>0}(rev(b)#1#a,j)  = [f_{e-1}(rev(b)#0#a,j) - r^(-a*2^(d-e)) * f_{e-1}(rev(b)#1#a,j)] / 2
    // so we're transforming 2 values of f_{e-1} to 2 values of f_e at the same indices: rev(b)#0#a and rev(b)#1#a.
    // But we want to start at f_d and end at f_0, so we recompute in terms of f_{e-1}:
    // f_{e-1}(rev(b)#0#a,j) =  f_e(rev(b)#0#a,j) + f_e(rev(b)#1#a,j)
    // f_{e-1}(rev(b)#1#a,j) = [f_e(rev(b)#0#a,j) - f_e(rev(b)#1#a,j)] * r^(a*2^(d-e))
    // And we rewrite to compute f_e in terms of f_{e+1}:
    // f_e(rev(b)#0#a,j) =  f_{e+1}(rev(b)#0#a,j) + f_{e+1}(rev(b)#1#a,j)
    // f_e(rev(b)#1#a,j) = [f_{e+1}(rev(b)#0#a,j) - f_{e+1}(rev(b)#1#a,j)] * r^(a*2^(d-e-1))
    // where we operate on pairs that differ only in the e'th bit from the right (0-indexed), and remember that a is the lower e bits of the row index i.
    // For example, we can see the tranformation apply to d=2 -> n=2^d=4:
    //    e=2    -> a     e=1     -> a      e=0         = permutation of rows (output) of DFT i <-> bitrev_d(i)
    // ┏       ┓      ┏         ┓      ┏          ┓        ┏          ┓   ┏                               ┓
    // ┃1 0 0 0┃    0 ┃1 0  1  0┃    0 ┃1  1  1  1┃        ┃1  1  1  1┃   ┃r^(0*0) r^(0*1) r^(0*2) r^(0*3)┃
    // ┃0 1 0 0┃    1 ┃0 1  0  1┃    0 ┃1 -1  1 -1┃        ┃1  r -1 -r┃   ┃r^(1*0) r^(1*1) r^(1*2) r^(1*3)┃
    // ┃0 0 1 0┃    0 ┃1 0 -1  0┃    0 ┃1  r -1 -r┃        ┃1 -1  1 -1┃   ┃r^(2*0) r^(2*1) r^(2*2) r^(2*3)┃
    // ┃0 0 0 1┃    1 ┃0 r  0 -r┃    0 ┃1 -r -1  r┃        ┃1 -r -1  r┃   ┃r^(3*0) r^(3*1) r^(3*2) r^(3*3)┃
    // ┗       ┛      ┗         ┛      ┗          ┛        ┗          ┛   ┗                               ┛
    //                2^(d-e-1)=1      2^(d-e-1)=2
    // And we can see the tranformation apply to d=3 -> n=2^d=8:
    //        e=3        -> a           e=2              -> a             e=1                -> a                 e=0
    // ┏               ┓      ┏                        ┓      ┏                            ┓      ┏                                 ┓
    // ┃1 0 0 0 0 0 0 0┃    0 ┃1 0  0  0  1  0   0   0 ┃    0 ┃1  0   1   0  1   0   1   0 ┃    0 ┃1   1    1    1   1   1   1    1 ┃
    // ┃0 1 0 0 0 0 0 0┃    1 ┃0 1  0  0  0  1   0   0 ┃    1 ┃0  1   0   1  0   1   0   1 ┃    0 ┃1  -1    1   -1   1  -1   1   -1 ┃
    // ┃0 0 1 0 0 0 0 0┃    2 ┃0 0  1  0  0  0   1   0 ┃    0 ┃1  0  -1   0  1   0  -1   0 ┃    0 ┃1  r^2  -1  -r^2  1  r^2 -1  -r^2┃
    // ┃0 0 0 1 0 0 0 0┃    3 ┃0 0  0  1  0  0   0   1 ┃    1 ┃0 r^2  0 -r^2 0  r^2  0 -r^2┃    0 ┃1 -r^2  -1   r^2  1 -r^2 -1   r^2┃
    // ┃0 0 0 0 1 0 0 0┃    0 ┃1 0  0  0 -1  0   0   0 ┃    0 ┃1  0  r^2  0 -1   0 -r^2  0 ┃    0 ┃1   r   r^2  r^3 -1  -r -r^2 -r^3┃
    // ┃0 0 0 0 0 1 0 0┃    1 ┃0 r  0  0  0 -r   0   0 ┃    1 ┃0  r   0  r^3 0  -r   0 -r^3┃    0 ┃1  -r   r^2 -r^3 -1   r -r^2  r^3┃
    // ┃0 0 0 0 0 0 1 0┃    2 ┃0 0 r^2 0  0  0 -r^2  0 ┃    0 ┃1  0 -r^2  0 -1   0  r^2  0 ┃    0 ┃1  r^3 -r^2 -r^5 -1 -r^3 r^2  r^5┃
    // ┃0 0 0 0 0 0 0 1┃    3 ┃0 0  0 r^3 0  0   0 -r^3┃    1 ┃0 r^3  0 -r^5 0 -r^3  0  r^5┃    0 ┃1 -r^3 -r^2  r^5 -1  r^3 r^2 -r^5┃
    // ┗               ┛      ┗                        ┛      ┗                            ┛      ┗                                 ┛
    //                               2^(d-e-1)=1                       2^(d-e-1)=2                            2^(d-e-1)=4
    // = permutation of rows (output) of DFT i <-> bitrev_d(i)
    // ┏                                 ┓  ┏                                                               ┓
    // ┃1   1    1    1   1   1   1    1 ┃  ┃r^(0*0) r^(0*1) r^(0*2) r^(0*3) r^(0*4) r^(0*5) r^(0*6) r^(0*7)┃
    // ┃1   r   r^2  r^3 -1  -r -r^2 -r^3┃  ┃r^(1*0) r^(1*1) r^(1*2) r^(1*3) r^(1*4) r^(1*5) r^(1*6) r^(1*7)┃
    // ┃1  r^2  -1  -r^2  1  r^2 -1  -r^2┃  ┃r^(2*0) r^(2*1) r^(2*2) r^(2*3) r^(2*4) r^(2*5) r^(2*6) r^(2*7)┃
    // ┃1  r^3 -r^2 -r^5 -1 -r^3 r^2  r^5┃  ┃r^(3*0) r^(3*1) r^(3*2) r^(3*3) r^(3*4) r^(3*5) r^(3*6) r^(3*7)┃
    // ┃1  -1    1   -1   1  -1   1   -1 ┃  ┃r^(4*0) r^(4*1) r^(4*2) r^(4*3) r^(4*4) r^(4*5) r^(4*6) r^(4*7)┃
    // ┃1  -r   r^2 -r^3 -1   r -r^2  r^3┃  ┃r^(5*0) r^(5*1) r^(5*2) r^(5*3) r^(5*4) r^(5*5) r^(5*6) r^(5*7)┃
    // ┃1 -r^2  -1   r^2  1 -r^2 -1   r^2┃  ┃r^(6*0) r^(6*1) r^(6*2) r^(6*3) r^(6*4) r^(6*5) r^(6*6) r^(6*7)┃
    // ┃1 -r^3 -r^2  r^5 -1  r^3 r^2 -r^5┃  ┃r^(7*0) r^(7*1) r^(7*2) r^(7*3) r^(7*4) r^(7*5) r^(7*6) r^(7*7)┃
    // ┗                                 ┛  ┗                                                               ┛

    UI n = 1U << exp2;
    UI m = r.get_mod();
    Vec<UI> rPow;
    rPow.reserve(n/2);
    Mod curRPow = r.create(1);
    for (UI i = 0; i < n/2; ++i) {
	rPow.push_back(static_cast<UI>(curRPow));
	curRPow *= r;
    }
    for (UI step = 0; step < exp2; ++step) {
	UI diff = 1U << (exp2 - 1U - step);
	for (UI i = 0; i < n; ++i) {
	    if (!(i & diff)) {
		UI j = i | diff;
		UI xi = x[i];
		UI xj = x[j];
		UL top = static_cast<UL>(xi) + xj;
		if (top >= m) {
		    top -= m;
		}
		UL bot = static_cast<UL>(xi) + m - xj;
		if (bot >= m) {
		    bot -= m;
		}
		bot = bot * rPow[(i & (diff - 1U)) << step] % m;
		x[i] = static_cast<UI>(top);
		x[j] = static_cast<UI>(bot);
	    }
	}
    }

    return x;
}

static Vec<UI> ifntImplPow2(Vec<UI> y, UI exp2, Mod const& r) {
    // Required invariants:
    // y has length n = 2^exp2
    // r has order n:
    //   r^n = 1, and r^k ≠ 1 for 0 < k < n
    // r is the same value passed to all ntt's that contributed to y

    // Calculates the inverse of fnt above.
    // Namely, computes
    // x such that y = M * x
    // or equivalently, x = M^(-1) y
    
    UI n = 1U << exp2;
    UI m = r.get_mod();
    Vec<UI> rInvPow;
    rInvPow.reserve(n/2);
    Mod rInv = 1 / r;
    Mod curRInvPow = r.create(1);
    for (UI i = 0; i < n/2; ++i) {
	rInvPow.push_back(static_cast<UI>(curRInvPow));
	curRInvPow *= rInv;
    }
    for (UI step = exp2; step--; ) {
	UI diff = 1U << (exp2 - 1U - step);
	for (UI i = 0; i < n; ++i) {
	    if (!(i & diff)) {
		UI j = i | diff;
		y[j] = static_cast<UI>(static_cast<UL>(y[j]) * rInvPow[(i & (diff - 1U)) << step] % m);
		
		UI yi = y[i];
		UI yj = y[j];
		UL top = UL(yi) + yj;
		if (top >= m) {
		    top -= m;
		}
		UL bot = UL(yi) + m - yj;
		if (bot >= m) {
		    bot -= m;
		}
		y[i] = static_cast<UI>(top);
		y[j] = static_cast<UI>(bot);
	    }
	}
    }

    auto nInv = static_cast<UI>(1 / r.create(n));
    for (UI i = 0; i < n; ++i) {
	y[i] = static_cast<UI>(static_cast<UL>(y[i]) * nInv % m);
    }

    return y;
}

template<SZ NumU32>
class BoundedBigInt {
  private:
    uint32_t fData[NumU32]{};

  public:
    BoundedBigInt() = default;
    BoundedBigInt(uint32_t x)
	: fData{x} {}

    friend B operator>=(BoundedBigInt const& x, BoundedBigInt const& y) {
	for (SZ i = NumU32; i--; ) {
	    if (x.fData[i] != y.fData[i]) {
		return x.fData[i] >= y.fData[i];
	    }
	}
	return true;
    }

    BoundedBigInt& operator+=(BoundedBigInt const& y) {
        uint64_t carry = 0;
	static_assert(sizeof(carry) > sizeof(fData[0]));
	for (SZ i = 0; i < NumU32; ++i) {
	    carry >>= 32;
	    carry += fData[i];
	    carry += y.fData[i];
	    fData[i] = static_cast<uint32_t>(carry);
	}
	return *this;
    }

    BoundedBigInt& operator-=(BoundedBigInt const& y) {
        B carry = false;
	for (SZ i = 0; i < NumU32; ++i) {
	    if (carry) {
		carry = fData[i]-- == 0;
	    }
	    carry |= fData[i] < y.fData[i];
	    fData[i] -= y.fData[i];
	}
	return *this;
    }

    friend BoundedBigInt operator*(BoundedBigInt const& x, BoundedBigInt const& y) {
	uint64_t current = 0;
	uint64_t carry = 0;
	BoundedBigInt res;
	for (SZ i = 0; i < NumU32; ++i) {
	    current = carry;
	    carry = 0;
	    for (SZ j = 0; j <= i; ++j) {
		current += static_cast<uint64_t>(x.fData[j]) * y.fData[i - j];
		carry += current >> 32;
		current %= static_cast<uint64_t>(1) << 32;
	    }
	    res.fData[i] = static_cast<uint32_t>(current);
	}
	return res;
    }

    uint32_t operator%(uint32_t y) {
	uint64_t res = 0;
	for (SZ i = NumU32; i--; ) {
	    res = ((res << 32) | fData[i]) % y;
	}
	return static_cast<uint32_t>(res);
    }
};

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
	static constexpr SZ NumPrimes = sizeof(FFT::ConvolutionConsts::biggest) / sizeof(FFT::ConvolutionConsts::biggest[0]);
	// Being careful to never overflow when performing + or *
	using MyBoundedBigInt = BoundedBigInt<NumPrimes + 1>;
	// In results, the MyBoundedBigInt is < max_uint_32^(NumPrimes-1)
	Vec<std::tuple<UI, MyBoundedBigInt, UI, Vec<UI>>> results;
	MyBoundedBigInt bigMod = 1;
	for (auto const& info : FFT::ConvolutionConsts::biggest) {
	    auto e = info.fExp2;
	    auto p = info.fPrime;
	    Mod dx = Mod(p, info.fRootPow2) ^ (1u << (e - d)); // element mod p with order 2^d
	    UI len = 1u << d;
	    bigMod = bigMod * p;
	    Vec<UI> fx = fntImplPow2(withLength(x, len), d, dx);
	    {
		Vec<UI> fy = fntImplPow2(withLength(y, len), d, dx);
		for (UI i = 0; i < len; ++i) {
		    fx[i] = static_cast<UI>(Mod(p, fx[i]) * fy[i]);
		}
	    }
	    auto convModP = ifntImplPow2(std::move(fx), d, dx);
	    for (UI i = static_cast<UI>(outputSize) + 1; i < len; ++i) {
		if (convModP[i] != 0) {
		    throw_exception<std::logic_error>("convolution computed a nonzero element too far into the array - the impl is wrong!");
		}
	    }
	    MyBoundedBigInt multFactor = 1;
	    Mod multFactorModP(p, 1);
	    for (auto const& info2 : FFT::ConvolutionConsts::biggest) {
		UI p2 = info2.fPrime;
		if (p != p2) {
		    multFactor = multFactor * p2;
		    multFactorModP /= p2;
		}
	    }
	    results.push_back({p, std::move(multFactor), static_cast<UI>(multFactorModP), std::move(convModP)});
	}
	Vec<Mod> result(outputSize, Mod(mod, 0));
	for (UI i = 0; i < outputSize; ++i) {
	    MyBoundedBigInt total;
	    for (auto const& [p, mult1, mult2modP, res] : results) {
		// the RHS here is < max_uint_32^NumPrimes
		// so the addition is safe when MyBoundedBigInt uses (NumPrimes+1) uint32_t-s
		total += mult1 * static_cast<UI>((static_cast<UL>(mult2modP) * res[i]) % p);
		if (total >= bigMod) {
		    // Since we're only adding a value < bigMod, and we started < bigMod, our total is < 2*bigMod.
		    // So to mod by bigMod, we need only subtract at most 1 of bigMod.
		    total -= bigMod;
		}
	    }
	    result[i] = total % mod;
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
