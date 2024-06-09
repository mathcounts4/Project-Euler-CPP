#pragma once

#include "fftw.hpp"

#include "Function.hpp"
#include "TypeUtils.hpp"
#include "Vec.hpp"

namespace FFT {
    
    template<class Data> struct FFTW_Vector {
	SZ const size;
	Data* const data;
	FFTW_Vector(SZ size);
	FFTW_Vector(FFTW_Vector const&) = delete;
	FFTW_Vector& operator=(FFTW_Vector const&) = delete;
	~FFTW_Vector();
    };
    
    template<class In, class Out> struct FFT_1D_Container {
	FFTW_Vector<In> const input;
	FFTW_Vector<Out> const output;
	fftw_plan plan;

	static constexpr B in_bigger = sizeof(In) > sizeof(Out);
	static constexpr B out_bigger = sizeof(Out) > sizeof(In);

	template<class Fcn_In, class Fcn_Out> FFT_1D_Container(SZ n, Function<fftw_plan,int,Fcn_In*,Fcn_Out*,UI> make_plan, B try_hard = false);
	FFT_1D_Container(FFT_1D_Container const&) = delete;
	FFT_1D_Container& operator=(FFT_1D_Container const&) = delete;
	~FFT_1D_Container();
	
	Vec<Out> operator()(Vec<In> const& data);
    };
}

#include "FFTDataImpl.hpp"
