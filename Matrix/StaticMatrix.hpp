#pragma once

#include <iostream>
#include "StaticMatrixConstants.hpp"
#include "SquareStaticMatrix.hpp"

// This class stores a matrix of T's, using dimensions as template arguments
// for zero, *, and ^, there needs to be a method T zero<T>()
// for one and ^, T there needs to be a method T one<T>()
// I've defined a template that supports zero<> and one<>
// for all primitive data types

// Most important operator supported:
// []
// Access StaticMatrix<int> x by using x[i][j] to set and get

// The following operators with one template argument R are supported,
// each requiring that
// (1) T supports that operator on itself,
// (2) T has a constructor T(R)
// (3) T has a copy constructor T a = T(R)

// (assignment) =(lvalue) += -=
// (binary) + -
// (binary) *
// NOTE: the * operator also requires T to have the + operator

// Example:
// StaticMatrix<int> a(3,4,-23);
// StaticMatrix<double> b(3,4,0.3);
// b += a; a = b; a*b; 


// The following operators are also supported, (not templatized)
// (unary) -
// (assignment) =(rvalue)
// (binary) ^
// NOTE: the ^ operator requires T to have the * operator

template<typename T,unsigned int m_rows,unsigned int m_cols>
class StaticMatrix
{
  private:
    T* m_data;
    
  public:
    StaticMatrix() :
	m_data(new T[m_rows * m_cols])
	{ }
    
    ~StaticMatrix()
	{
	    delete[] m_data;
	}

    inline T* operator[](unsigned int row) const
	{
	    return m_data + row * m_cols;
	}

    inline unsigned int numRows() const { return m_rows; }
    inline unsigned int numCols() const { return m_cols; }

    StaticMatrix(const T& init) :
	StaticMatrix()
	{
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    (*this)[i][j] = init;
		}
	    }
	}

    template<typename... Args>
    StaticMatrix(const Args&... args) :
	StaticMatrix(T(args...))
	{ }

    StaticMatrix(const StaticMatrix& other) :
	StaticMatrix()
	{
	    *this = other;
	}

    template<typename R>
    StaticMatrix(const StaticMatrix<R,m_rows,m_cols>& other) :
	StaticMatrix()
	{
	    *this = other;
	}

    StaticMatrix(StaticMatrix&& other) :
	m_data(other.m_data)
	{
	    other.m_data = nullptr;
	}

    StaticMatrix& operator=(const StaticMatrix& other)
	{
	    if (this != &other)
	    {
		for (auto i = 0U; i < m_rows; i++)
		{
		    for (auto j = 0U; j < m_cols; j++)
		    {
			(*this)[i][j] = other[i][j];
		    }
		}
	    }
	    return *this;
	}

    template<typename R>
    StaticMatrix& operator=(const StaticMatrix<R,m_rows,m_cols>& other)
	{
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    (*this)[i][j] = T(other[i][j]);
		}
	    }
	    return *this;
	}

    StaticMatrix& operator=(StaticMatrix&& other)
	{
	    delete[] m_data;
	    m_data = other.m_data;
	    other.m_data = nullptr;
	    return *this;
	}

    void print() const
	{
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    std::cout << (*this)[i][j] << " ";
		}
		std::cout << std::endl;
	    }
	}

    bool isEmpty() const
	{
	    return (m_rows == 0) && (m_cols == 0);
	}

    StaticMatrix& negate()
	{
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    (*this)[i][j] = -(*this)[i][j];
		}
	    }

	    return *this;
	}

    StaticMatrix operator-()
	{
	    StaticMatrix result(m_rows,m_cols);

	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    result[i][j] = -(*this)[i][j];
		}
	    }
	    
	    return result;
	}

    StaticMatrix& operator+=(const StaticMatrix& other)
	{
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    (*this)[i][j] += other[i][j];
		}
	    }

	    return *this;
	}
    
    template<typename R>
    StaticMatrix& operator+=(const StaticMatrix<R,m_rows,m_cols>& other)
	{
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    (*this)[i][j] += T(other[i][j]);
		}
	    }

	    return *this;
	}

    StaticMatrix& operator-=(const StaticMatrix& other)
	{
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    (*this)[i][j] -= other[i][j];
		}
	    }

	    return *this;
	}

    template<typename R>
    StaticMatrix& operator-=(const StaticMatrix<R,m_rows,m_cols>& other)
	{
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    (*this)[i][j] -= T(other[i][j]);
		}
	    }

	    return *this;
	}

    StaticMatrix operator+(const StaticMatrix& other) const
	{
	    StaticMatrix sum;
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    sum[i][j] = (*this)[i][j] + other[i][j];
		}
	    }
	    return sum;
	}

    template<typename R>
    StaticMatrix operator+(const StaticMatrix<R,m_rows,m_cols>& other) const
	{
	    StaticMatrix sum;
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    sum[i][j] = (*this)[i][j] + T(other[i][j]);
		}
	    }
	    return sum;
	}

    StaticMatrix&& operator+(StaticMatrix&& other) const
	{
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    other[i][j] = (*this)[i][j] + other[i][j];
		}
	    }
	    return std::move(other);
	}

    StaticMatrix operator-(const StaticMatrix& other) const
	{
	    StaticMatrix difference;
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    difference[i][j] = (*this)[i][j] - other[i][j];
		}
	    }
	    return difference;
	}
    
    template<typename R>
    StaticMatrix operator-(const StaticMatrix<R,m_rows,m_cols>& other) const
	{
	    StaticMatrix difference;
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    difference[i][j] = (*this)[i][j] - T(other[i][j]);
		}
	    }
	    return difference;
	}

    StaticMatrix&& operator-(StaticMatrix&& other) const
	{
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    other[i][j] = (*this)[i][j] - other[i][j];
		}
	    }
	    return std::move(other);
	}

    template<unsigned int o_cols>
    StaticMatrix<T,m_rows,o_cols> operator*(const StaticMatrix<T,m_cols,o_cols>& other) const
	{
	    StaticMatrix<T,m_rows,o_cols> product(Zero<T>::get());
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    for (auto k = 0U; k < o_cols; k++)
		    {
			product[i][k] += (*this)[i][j] * other[j][k];
		    }
		}
	    }
	    return product;
	}

    template<typename R, unsigned int o_cols>
    StaticMatrix<T,m_rows,o_cols> operator*(const StaticMatrix<R,m_cols,o_cols>& other) const
	{
	    StaticMatrix<T,m_rows,o_cols> product(Zero<T>::get());
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    for (auto k = 0U; k < o_cols; k++)
		    {
			product[i][k] += (*this)[i][j] * T(other[j][k]);
		    }
		}
	    }
	    return product;
	}
};
