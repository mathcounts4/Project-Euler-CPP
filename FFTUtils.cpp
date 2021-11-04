// make sure to link against fftw3:
// ... -lfftw3 ...

#include <fftw3.h>
#include <stdexcept>
#include <utility>

#include "ExitUtils.hpp"
#include "FFTData.hpp"
#include "FFTUtils.hpp"
#include "Function.hpp"
#include "TypeUtils.hpp"

namespace FFT {
    
    template<class In, class Out> using Map_1D_Type = U_Map<SZ,FFT_1D_Container<In,Out> >;
    static Map_1D_Type<D,CD> r2c_plans;
    static Map_1D_Type<CD,D> c2r_plans;

    void fft_1d_r2c_discard(SI n) {
	r2c_plans.erase(n);
    }
    void fft_1d_c2r_discard(int n) {
	c2r_plans.erase(n);
    }

    V<CD> fft(V<D> const& in, B save) {
	SZ const n = in.size();
	B exists = r2c_plans.count(n) > 0;
	B discard = !exists && !save;
	if (!exists)
	    r2c_plans.emplace(std::piecewise_construct,
			      std::make_tuple(n),
			      std::make_tuple(n,fftw_plan_dft_r2c_1d,save));
	V<CD> result = r2c_plans.at(n)(in);
	if (discard)
	    fft_1d_r2c_discard(n);
	return result;
    }
    V<D> ifft(SZ const n, V<CD> const& in, B save) {
	B exists = c2r_plans.count(n) > 0;
	B discard = !exists && !save;
	if (!exists)
	    c2r_plans.emplace(std::piecewise_construct,
			      std::make_tuple(n),
			      std::make_tuple(n,fftw_plan_dft_c2r_1d,save));
	V<D> result = c2r_plans.at(n)(in);
	if (discard)
	    fft_1d_c2r_discard(n);
	for (D& x : result)
	    x /= n;
	return result;
    }

    V<D> convolution(V<D> x, V<D> y, SZ goal, B save) {
	if (x.size() == 0 || y.size() == 0)
	    return {};
	SZ size = x.size() + y.size() - 1;
	if (goal == static_cast<SZ>(-1))
	    goal = size;
	x.resize(size,0);
	y.resize(size,0);
	V<CD> fft_x = fft(x,save);
	V<CD> fft_y = fft(y,save);
	SZ fft_size = fft_x.size();
	for (SZ i = 0; i < fft_size; ++i)
	    fft_x[i] *= fft_y[i];
	V<D> result = ifft(size,fft_x,save);
	result.resize(goal);
	return result;
    }
    V<Mod> convolution(V<Mod> const& x, V<Mod> const& y, SZ goal, B save) {
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
	V<Mod> result(goal,Mod(mod,0));

	V<Mod> const& shorter = x.size()<y.size() ? x : y;
	V<Mod> const& longer = x.size()<y.size() ? y : x;
    
	SZ small = std::min(shorter.size(),goal);
	SZ big = std::min(longer.size(),goal);
    
	SZ bottom = 0;
	SZ top = small;
	for (; top < big; bottom += small, top += small) {
	    throw_exception<std::invalid_argument>("convolution for Mod not fully implemented");
	}
    
	return double_to_Mod(
	    convolution(
		Mod_to_double(x),
		Mod_to_double(y),
		goal,
		save),
	    mod);
    }

    V<Mod> double_to_Mod(V<D> const& v, UI mod) {
	V<Mod> result;
	SZ size = v.size();
	for (SZ i = 0; i < size; ++i)
	    result.emplace_back(mod,lround(v[i]));
	return result;
    }
    V<D> Mod_to_double(V<Mod> const& v) {
	V<D> result;
	SZ size = v.size();
	for (SZ i = 0; i < size; ++i)
	    result.push_back(D(v[i]));
	return result;
    }
}
