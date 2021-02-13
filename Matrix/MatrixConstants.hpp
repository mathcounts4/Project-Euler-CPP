#pragma once

#include <cstddef>
#include "../Constants/Zero.hpp"
#include "../Constants/One.hpp"
#include "../Construct.hpp"

template<class T, std::size_t M, std::size_t N> class Matrix;

template<class T, std::size_t M, std::size_t N>
class Zero<Matrix<T,M,N> > {
  public:
    static Matrix<T,M,N> get() {
	return Matrix<T,M,N>(Construct::piecewise,Zero<T>::get());
    }
};

template<class T, std::size_t N>
class One<Matrix<T,N,N> > {
  public:
    static Matrix<T,N,N> get() {
	Matrix<T,N,N> result = Zero<Matrix<T,N,N> >::get();
	for (std::size_t i = 0; i < N; i++)
	    result[i][i] = One<T>::get();
	return result;
    }
};
