#pragma once

#include <iostream>
#include "MatrixGuts.hpp"
#include "NewMatrixConstants.hpp"

// General Matrix template class
template <typename T, unsigned int m_rows, unsigned int m_cols>
class NewMatrix
{
    MATRIX_GUTS(NewMatrix,T,m_rows,m_cols)
};

// Square Matrix template class
template <typename T, unsigned int m_dim>
class NewMatrix<T,m_dim,m_dim>
{
    MATRIX_GUTS(NewMatrix,T,m_dim,m_dim)

    template<typename R>
    NewMatrix& operator*=(const NewMatrix<R,m_dim,m_dim>& other)
	{
	    auto P = (*this) * other;
	    (*this) = std::move(P);
	    return *this;
	}

    NewMatrix operator^(unsigned long power) const
	{
	    NewMatrix result = One<NewMatrix>::get();

	    NewMatrix pow(*this);

	    while (power > 0UL)
	    {
		if ((power & 1UL) == 1UL)
		{
		    result *= pow;
		}

		pow *= pow;
		power >>= 1UL;
	    }

	    return result;
	}
};

template<typename T,typename R, unsigned int m_rows, unsigned int m_cols>
NewMatrix<T,m_rows,m_cols>&& operator+(
    NewMatrix<T,m_rows,m_cols>&& first,
    const NewMatrix<R,m_rows,m_cols>& second)
{
    std::cout << "&& + &" << std::endl;
    LOOP(i,m_rows,
	 j,m_cols,
	 first[i][j] = first[i][j] + second[i][j]
	);
    return std::move(first);
}

template<typename T, unsigned int m_rows, unsigned int m_cols>
std::ostream& operator<<(std::ostream& os,const NewMatrix<T,m_rows,m_cols>& M)
{
    for (auto i = 0U; i < m_rows; i++)
    {
	for (auto j = 0U; j < m_cols; j++)
	{
	    os << M[i][j] << " ";
	}
	os << std::endl;
    }
    return os;
}
