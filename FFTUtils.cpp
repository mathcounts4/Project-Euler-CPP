#include "fftw.hpp"
#include <stdexcept>
#include <utility>

#include "ExitUtils.hpp"
#include "FFTData.hpp"
#include "FFTUtils.hpp"
#include "Function.hpp"
#include "TypeUtils.hpp"
#include "U_Map.hpp"
#include "Vec.hpp"

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
	x.resize(size,0);
	y.resize(size,0);
	Vec<CD> fft_x = fft(x,save);
	Vec<CD> fft_y = fft(y,save);
	SZ fft_size = fft_x.size();
	for (SZ i = 0; i < fft_size; ++i)
	    fft_x[i] *= fft_y[i];
	Vec<D> result = ifft(size,fft_x,save);
	result.resize(goal);
	return result;
    }
    Vec<Mod> convolution(Vec<Mod> const& x, Vec<Mod> const& y, SZ goal) {
	// [    x    ] * [    y    ] =
	// [  A   B  ] * [  C   D  ] =
	// [A*C]+[B*D]
	//      +
	//    [B*C]
	//      +
	//    [A*D]
	//      =
	//    M + N
	//      +
	// [A+B]*[C+D]-M-N

	if (x.size() == 0 || y.size() == 0)
	    throw_exception<std::invalid_argument>("vector passed to convolution has 0 size");
	if (goal == 0)
	    throw_exception<std::invalid_argument>("goal length passed to convolution was 0");
	if (goal == static_cast<SZ>(-1))
	    goal = x.size() + y.size() - 1;
	UI mod = x[0].get_mod();
	Vec<Mod> result(goal,Mod(mod,0));

	Vec<Mod> const& shorter = x.size()<y.size() ? x : y;
	Vec<Mod> const& longer = x.size()<y.size() ? y : x;
    
	SZ small = std::min(shorter.size(),goal);
	SZ big = std::min(longer.size(),goal);
    
	SZ bottom = 0;
	SZ top = small;
	for (; top < big; bottom += small, top += small) {
	    
	}
	throw_exception<std::invalid_argument>("convolution for Mod not fully implemented");
	/*
	return double_to_Mod(
	    convolution(
		Mod_to_double(x),
		Mod_to_double(y),
		goal,
		save),
	    mod);
	*/
    }

    Vec<Mod> double_to_Mod(Vec<D> const& v, UI mod) {
	Vec<Mod> result;
	SZ size = v.size();
	for (SZ i = 0; i < size; ++i)
	    result.emplace_back(mod,lround(v[i]));
	return result;
    }
    Vec<D> Mod_to_double(Vec<Mod> const& v) {
	Vec<D> result;
	SZ size = v.size();
	for (SZ i = 0; i < size; ++i)
	    result.push_back(D(v[i]));
	return result;
    }
}
