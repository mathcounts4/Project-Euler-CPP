#pragma once

// uses fftw3 from http://www.fftw.org/ to expose fft and convolution functions
// make sure to link against the library using -l

#include <complex>

#include "Math.hpp"
#include "TypeUtils.hpp"

namespace FFT {
    
    using CD = std::complex<D>;
    
    void fft_1d_r2c_discard(SI n);
    void fft_1d_c2r_discard(SI n);
    
    V<CD> fft(V<D> const& in, B save = false);
    V<D> ifft(SZ const n, V<CD> const& in, B save = false);

    V<D> convolution(V<D> x, V<D> y, SZ goal = -1, B save = false);
    V<Mod> convolution(V<Mod> const& x, V<Mod> const& y, SZ goal = -1, B save = false);
    
    V<Mod> double_to_Mod(V<D> const& v, UI mod);
    V<D> Mod_to_double(V<Mod> const& v);
}
