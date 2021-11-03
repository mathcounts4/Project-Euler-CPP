#pragma once

#include <stdexcept>
#include <fftw3.h>

#include "FFTData.hpp"

namespace FFT {
    
    template<class Data> FFTW_Vector<Data>::FFTW_Vector(SZ size)
	: size(size)
	, data(static_cast<Data*>(fftw_malloc(sizeof(Data) * size))) {}

    template<class Data> FFTW_Vector<Data>::~FFTW_Vector() {
	try {
	    fftw_free(data);
	} catch (...) {}
    }

    template<class In, class Out>
    template<class Fcn_In, class Fcn_Out>
    FFT_1D_Container<In,Out>::FFT_1D_Container(
	SZ n,
	Function<fftw_plan,SI,Fcn_In*,Fcn_Out*,UI> make_plan,
	B try_hard)
	// Bigger data is complex, smaller is real
	: input(in_bigger ? n/2+1 : n)
	, output(out_bigger ? n/2+1 : n)
	, plan(make_plan(
		   n,
		   reinterpret_cast<Fcn_In*>(input.data),
		   reinterpret_cast<Fcn_Out*>(output.data),
		   try_hard ? FFTW_MEASURE : FFTW_ESTIMATE)) {
	static_assert(sizeof(In)==sizeof(Fcn_In),"Specified input type for FFT_1D_Container does not match function signature passed to constructor");
	static_assert(sizeof(Out)==sizeof(Fcn_Out),"Specified output type for FFT_1D_Container does not match function signature passed to constructor");
    }

    template<class In, class Out> FFT_1D_Container<In,Out>::~FFT_1D_Container() {
	try {
	    fftw_destroy_plan(plan);
	} catch (...) {}
    }

    template<class In, class Out> V<Out> FFT_1D_Container<In,Out>::operator()(V<In> const& data) {
	if (data.size() != input.size)
	    throw_exception<std::invalid_argument>("Wrong data size passed to FFT_1D_Container: expected size " + std::to_string(input.size) + " but passed " + std::to_string(data.size()));

	V<Out> result(output.size);
	memcpy(input.data,&data[0],sizeof(In) * input.size);
	fftw_execute(plan);
	memcpy(&result[0],output.data,sizeof(Out) * output.size);
	return result;
    }
}
