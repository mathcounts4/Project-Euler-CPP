#pragma once

// uses fftw3 from http://www.fftw.org/ to expose fft and convolution functions
// make sure to link against the library using -l

#include <complex>

#include "Math.hpp"
#include "Mod.hpp"
#include "TypeUtils.hpp"
#include "Vec.hpp"

namespace FFT {
    
    using CD = std::complex<D>;
    
    Vec<CD> fft(Vec<D> const& in, B save = false);
    Vec<D> ifft(SZ const n, Vec<CD> const& in, B save = false);

    Vec<D> convolution(Vec<D> x, Vec<D> y, SZ goal = SZ(-1), B save = false);
    Vec<Mod> convolution(Vec<Mod> const& x, Vec<Mod> const& y, SZ goal = SZ(-1));
}
