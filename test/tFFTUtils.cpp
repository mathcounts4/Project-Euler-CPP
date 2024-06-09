#include "../FFTUtils.hpp"
#include "harness.hpp"

using CD = FFT::CD;

TEST(fft, basic) {
    CHECK(FFT::fft(Vec<D>{1, -2, 4, -7}), equals(Vec<CD>{-4, CD{-3, -5}, CD{14, 0}}));
    CHECK(FFT::ifft(4u, Vec<CD>{-4, CD{-3, -5}, CD{14, 0}}), equals(Vec<D>{1, -2, 4, -7}));
    
    CHECK(FFT::ifft(4u, FFT::fft(Vec<D>{5, 17, 83, 92})), equals(Vec<D>{5, 17, 83, 92}));
    CHECK(FFT::fft(FFT::ifft(4u, Vec<CD>{5, 17, 83})), equals(Vec<CD>{5, 17, 83}));
}

TEST(conv, basic) {
    CHECK(FFT::convolution(Vec<D>{1, 4, 6}, Vec<D>{2, 1}), equals(Vec<D>{2, 9, 16, 6}));
}
