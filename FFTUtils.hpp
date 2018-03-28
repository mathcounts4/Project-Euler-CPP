#pragma once
#ifndef FFT_UTILS
#define FFT_UTILS

// uses fftw3 from http://www.fftw.org/ to expose fft and convolution functions
// make sure to link against the library using -l

#include <complex>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include <fftw3.h>

#include "MathUtils.hpp"
#include "TypeUtils.hpp"

#include <iostream>

namespace
{
    template<class type>
    struct fftw_vector
    {
	std::size_t size;
	std::unique_ptr<type*> data;
	fftw_vector(std::size_t size)
	    :
	    size(size),
	    data(new type*)
	    {
		*data = (type*)fftw_malloc(sizeof(type) * size);
	    }
	fftw_vector(fftw_vector && other)
	    :
	    size(other.size),
	    data(std::move(other.data))
	    {
		other.size = 0;
	    }
	~fftw_vector()
	    {
		if (data)
		    fftw_free(*data);
	    }
	type* ptr()
	    {
		return *data;
	    }
    };
    
    template<class in, class out>
    struct fft_1d_container
    {
        fftw_vector<in> input;
        fftw_vector<out> output;
	std::unique_ptr<fftw_plan> plan;

	static constexpr bool in_bigger = sizeof(in) > sizeof(out);
	static constexpr bool out_bigger = sizeof(out) > sizeof(in);

	template<class fcn_in, class fcn_out>
	fft_1d_container(std::size_t n,
			 function_type<fftw_plan,int,fcn_in*,fcn_out*,unsigned int> make_plan,
			 bool try_hard = false)
	    :
	    // if the input or output is bigger, it in complex while the other is real,
	    // se only allocate n/2+1 for the larger data type
	    input(in_bigger ? n/2+1 : n),
	    output(out_bigger ? n/2+1 : n),
	    plan(new fftw_plan)
	    {
		static_assert(sizeof(in)==sizeof(fcn_in),"Specified input type for fft_1d_container does not match function signature passed to constructor");
		static_assert(sizeof(out)==sizeof(fcn_out),"Specified output type for fft_1d_container does not match function signature passed to constructor");

		*plan = make_plan(
		    n,
		    (fcn_in*)input.ptr(),
		    (fcn_out*)output.ptr(),
		    try_hard ? FFTW_MEASURE : FFTW_ESTIMATE);
	    }
	fft_1d_container(fft_1d_container && other)
	    :
	    input(std::move(other.input)),
	    output(std::move(other.output)),
	    plan(std::move(other.plan))
	    {}
	~fft_1d_container()
	    {
		if (plan)
		    fftw_destroy_plan(*plan);
	    }
	fft_1d_container& operator=(fft_1d_container const &) = delete;
	fft_1d_container& operator=(fft_1d_container &&) = delete;
	std::vector<out> operator()(std::vector<in> const & data)
	    {
		if (data.size() != input.size)
		    throw std::invalid_argument("Wrong data size passed to fft_1d_container: expected size " + std::to_string(input.size) + " but passed " + std::to_string(data.size()));

		std::vector<out> result(output.size);
		memcpy(input.ptr(),&data[0],sizeof(in) * input.size);
		fftw_execute(*plan);
		memcpy(&result[0],output.ptr(),sizeof(out) * output.size);
		return result;
	    }
    };

    template<class in, class out>
    using map_1d_type = std::unordered_map<std::size_t,fft_1d_container<in,out> >;
    using cdata = std::complex<double>;
    
    map_1d_type<double,cdata> r2c_plans;
    map_1d_type<cdata,double> c2r_plans;
}

void fft_1d_r2c_discard(int n)
{
    r2c_plans.erase(n);
}

std::vector<cdata> fft(std::vector<double> const & in,
		       bool save = false)
{
    std::size_t const n = in.size();
    bool exists = r2c_plans.count(n) > 0;
    bool discard = !exists && !save;
    if (!exists)
	r2c_plans.insert({n,fft_1d_container<double,cdata>(n,fftw_plan_dft_r2c_1d,save)});
    std::vector<cdata> result = r2c_plans.at(n)(in);
    if (discard)
        fft_1d_r2c_discard(n);
    return result;
}

void fft_1d_c2r_discard(int n)
{
    c2r_plans.erase(n);
}

std::vector<double> ifft(std::size_t const n,
			 std::vector<cdata> const & in,
			 bool save = false)
{
    bool exists = c2r_plans.count(n) > 0;
    bool discard = !exists && !save;
    if (!exists)
	c2r_plans.insert({n,fft_1d_container<cdata,double>(n,fftw_plan_dft_c2r_1d,save)});
    std::vector<double> result = c2r_plans.at(n)(in);
    if (discard)
        fft_1d_c2r_discard(n);
    for (double& x : result)
	x /= n;
    return result;
}

std::vector<double> convolution(std::vector<double> x,
				std::vector<double> y,
				std::size_t goal = -1,
				bool save = false)
{
    if (x.size() == 0 || y.size() == 0)
	return {};
    std::size_t size = x.size() + y.size() - 1;
    if (goal == -1)
	goal = size;
    x.resize(size,0);
    y.resize(size,0);
    std::vector<cdata> fft_x = fft(x,save);
    std::vector<cdata> fft_y = fft(y,save);
    std::size_t fft_size = fft_x.size();
    for (std::size_t i = 0; i < fft_size; ++i)
	fft_x[i] *= fft_y[i];
    std::vector<double> result = ifft(size,fft_x,save);
    result.resize(goal);
    return result;
}

std::vector<Mod> double_to_Mod(std::vector<double> const & v,
			       unsigned int mod)
{
    std::vector<Mod> result;
    std::size_t size = v.size();
    for (std::size_t i = 0; i < size; ++i)
	result.emplace_back(mod,lround(v[i]));
    return result;
}

std::vector<double> Mod_to_double(std::vector<Mod> const & v)
{
    std::vector<double> result;
    std::size_t size = v.size();
    for (std::size_t i = 0; i < size; ++i)
	result.push_back(double(v[i]));
    return result;
}

namespace
{
    
}

std::vector<Mod> convolution(std::vector<Mod> const & x,
			     std::vector<Mod> const & y,
			     std::size_t goal = -1)
{
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
	my_exit("vector passed to convolution has 0 size");
    if (goal == 0)
	my_exit("goal length passed to convolution was 0");
    if (goal == -1)
	goal = x.size() + y.size() - 1;
    unsigned int mod = x[0].get_mod();
    std::vector<Mod> result(goal,Mod(mod,0));

    std::vector<Mod> const & shorter = x.size()<y.size() ? x : y;
    std::vector<Mod> const & longer = x.size()<y.size() ? y : x;
    
    std::size_t small = std::min(shorter.size(),goal);
    std::size_t big = std::min(longer.size(),goal);
    
    std::size_t bottom = 0;
    std::size_t top = small;
    for (; top < big; bottom += small, top += small)
    {
	
    }
    
    
    unsigned int mod = x[0].get_mod();
    return double_to_Mod(
	convolution(
	    Mod_to_double(x),
	    Mod_to_double(y),
	    goal,
	    save),
	mod);
}

#endif /* FFT_UTILS */
