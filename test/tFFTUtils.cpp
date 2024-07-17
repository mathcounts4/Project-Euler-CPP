#include "../FFTUtils.hpp"
#include "../PrimeUtils.hpp"
#include "harness.hpp"

using CD = FFT::CD;

TEST(fft, basic) {
    CHECK(FFT::fft(Vec<D>{1, -2, 4, -7}), equals(Vec<CD>{-4, CD{-3, -5}, CD{14, 0}}));
    CHECK(FFT::ifft(4u, Vec<CD>{-4, CD{-3, -5}, CD{14, 0}}), equals(Vec<D>{1, -2, 4, -7}));
    
    CHECK(FFT::ifft(4u, FFT::fft(Vec<D>{5, 17, 83, 92})), equals(Vec<D>{5, 17, 83, 92}));
    CHECK(FFT::fft(FFT::ifft(4u, Vec<CD>{5, 17, 83})), equals(Vec<CD>{5, 17, 83}));
}

TEST(conv, basic) {
    CHECK(FFT::convolution(Vec<D>{2}, Vec<D>{3}), equals(Vec<D>{6}));
    CHECK(FFT::convolution(Vec<D>{1, 4, 6}, Vec<D>{2, 1}), equals(Vec<D>{2, 9, 16, 6}));
}

TEST(conv, modBasic) {
    for (UI mod : {
	    120u,
	    1u << 31u,
	    FFT::ConvolutionConsts::biggest[0].fPrime,
	    FFT::ConvolutionConsts::biggest[1].fPrime,
	    FFT::ConvolutionConsts::biggest[2].fPrime}) {
	Mod::GlobalModSentinel s(mod);
	CHECK(FFT::convolution(Vec<Mod>{2}, Vec<Mod>{3}), equals(Vec<Mod>{6}));
	CHECK(FFT::convolution(Vec<Mod>{1, 4, 6}, Vec<Mod>{2, 1}), equals(Vec<Mod>{2, 9, 16, 6}));
    }
}

TEST(conv, algorithmConsts) {
    for (auto const& info : FFT::ConvolutionConsts::biggest) {
	auto e = info.fExp2;
	auto m = info.fMultFactor;
	auto p = info.fPrime;
	auto r = info.fPrimRoot;
	auto x = info.fRootPow2;
	CHECK(m * (1u << e) + 1u, equals(p));
	CHECK(is_prime(p), isTrue());
	CHECK(primitiveRoot(p), equals(r));
	CHECK(Mod(p, r) ^ m, equals(x));
	CHECK(Mod(p, r) ^ (p - 1), equals(1));
	CHECK(Mod(p, x) ^ (1u << e), equals(1));
    }
}
