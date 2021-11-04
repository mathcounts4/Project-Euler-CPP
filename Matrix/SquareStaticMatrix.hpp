#pragma once

#include <iostream>
#include "StaticMatrixConstants.hpp"

template<typename T,unsigned int m_rows,unsigned int m_cols>
class StaticMatrix;

template<typename T,unsigned int dim>
class StaticMatrix<T,dim,dim>
{
  private:
    T* m_data;
    
  public:
    StaticMatrix() :
	m_data(new T[dim * dim])
	{ }
    
    ~StaticMatrix()
	{
	    delete[] m_data;
	}

    inline T* operator[](unsigned int row) const
	{
	    return m_data + row * dim;
	}

    inline unsigned int numRows() const { return dim; }
    inline unsigned int numCols() const { return dim; }

    StaticMatrix(const T& init) :
	StaticMatrix()
	{
	    for (auto i = 0U; i < dim; i++)
	    {
		for (auto j = 0U; j < dim; j++)
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
    StaticMatrix(const StaticMatrix<R,dim,dim>& other) :
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
		for (auto i = 0U; i < dim; i++)
		{
		    for (auto j = 0U; j < dim; j++)
		    {
			(*this)[i][j] = other[i][j];
		    }
		}
	    }
	    return *this;
	}

    template<typename R>
    StaticMatrix& operator=(const StaticMatrix<R,dim,dim>& other)
	{
	    for (auto i = 0U; i < dim; i++)
	    {
		for (auto j = 0U; j < dim; j++)
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
	    for (auto i = 0U; i < dim; i++)
	    {
		for (auto j = 0U; j < dim; j++)
		{
		    std::cout << (*this)[i][j] << " ";
		}
		std::cout << std::endl;
	    }
	}

    bool isEmpty() const
	{
	    return (dim == 0) && (dim == 0);
	}

    StaticMatrix& negate()
	{
	    for (auto i = 0U; i < dim; i++)
	    {
		for (auto j = 0U; j < dim; j++)
		{
		    (*this)[i][j] = -(*this)[i][j];
		}
	    }

	    return *this;
	}

    StaticMatrix operator-()
	{
	    StaticMatrix result(dim,dim);

	    for (auto i = 0U; i < dim; i++)
	    {
		for (auto j = 0U; j < dim; j++)
		{
		    result[i][j] = -(*this)[i][j];
		}
	    }
	    
	    return result;
	}

    StaticMatrix& operator+=(const StaticMatrix& other)
	{
	    for (auto i = 0U; i < dim; i++)
	    {
		for (auto j = 0U; j < dim; j++)
		{
		    (*this)[i][j] += other[i][j];
		}
	    }

	    return *this;
	}
    
    template<typename R>
    StaticMatrix& operator+=(const StaticMatrix<R,dim,dim>& other)
	{
	    for (auto i = 0U; i < dim; i++)
	    {
		for (auto j = 0U; j < dim; j++)
		{
		    (*this)[i][j] += T(other[i][j]);
		}
	    }

	    return *this;
	}

    StaticMatrix& operator-=(const StaticMatrix& other)
	{
	    for (auto i = 0U; i < dim; i++)
	    {
		for (auto j = 0U; j < dim; j++)
		{
		    (*this)[i][j] -= other[i][j];
		}
	    }

	    return *this;
	}

    template<typename R>
    StaticMatrix& operator-=(const StaticMatrix<R,dim,dim>& other)
	{
	    for (auto i = 0U; i < dim; i++)
	    {
		for (auto j = 0U; j < dim; j++)
		{
		    (*this)[i][j] -= T(other[i][j]);
		}
	    }

	    return *this;
	}

    StaticMatrix operator+(const StaticMatrix& other) const
	{
	    StaticMatrix sum;
	    for (auto i = 0U; i < dim; i++)
	    {
		for (auto j = 0U; j < dim; j++)
		{
		    sum[i][j] = (*this)[i][j] + other[i][j];
		}
	    }
	    return sum;
	}

    template<typename R>
    StaticMatrix operator+(const StaticMatrix<R,dim,dim>& other) const
	{
	    StaticMatrix sum;
	    for (auto i = 0U; i < dim; i++)
	    {
		for (auto j = 0U; j < dim; j++)
		{
		    sum[i][j] = (*this)[i][j] + T(other[i][j]);
		}
	    }
	    return sum;
	}

    StaticMatrix&& operator+(StaticMatrix&& other) const
	{
	    for (auto i = 0U; i < dim; i++)
	    {
		for (auto j = 0U; j < dim; j++)
		{
		    other[i][j] = (*this)[i][j] + other[i][j];
		}
	    }
	    return std::move(other);
	}

    StaticMatrix operator-(const StaticMatrix& other) const
	{
	    StaticMatrix difference;
	    for (auto i = 0U; i < dim; i++)
	    {
		for (auto j = 0U; j < dim; j++)
		{
		    difference[i][j] = (*this)[i][j] - other[i][j];
		}
	    }
	    return difference;
	}
    
    template<typename R>
    StaticMatrix operator-(const StaticMatrix<R,dim,dim>& other) const
	{
	    StaticMatrix difference;
	    for (auto i = 0U; i < dim; i++)
	    {
		for (auto j = 0U; j < dim; j++)
		{
		    difference[i][j] = (*this)[i][j] - T(other[i][j]);
		}
	    }
	    return difference;
	}

    StaticMatrix&& operator-(StaticMatrix&& other) const
	{
	    for (auto i = 0U; i < dim; i++)
	    {
		for (auto j = 0U; j < dim; j++)
		{
		    other[i][j] = (*this)[i][j] - other[i][j];
		}
	    }
	    return std::move(other);
	}

    template<unsigned int o_cols>
    StaticMatrix<T,dim,o_cols> operator*(const StaticMatrix<T,dim,o_cols>& other) const
	{
	    StaticMatrix<T,dim,o_cols> product(Zero<T>::get());
	    for (auto i = 0U; i < dim; i++)
	    {
		for (auto j = 0U; j < dim; j++)
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
    StaticMatrix operator*(const StaticMatrix<R,dim,o_cols>& other) const
	{
	    StaticMatrix<T,dim,o_cols> product(Zero<T>::get());
	    for (auto i = 0U; i < dim; i++)
	    {
		for (auto j = 0U; j < dim; j++)
		{
		    for (auto k = 0U; k < o_cols; k++)
		    {
			product[i][k] += (*this)[i][j] * T(other[j][k]);
		    }
		}
	    }
	    return product;
	}

    StaticMatrix operator^(unsigned long power) const
	{
	    StaticMatrix result = One<StaticMatrix>::get();

	    StaticMatrix pow(*this);

	    while (power > 0UL)
	    {
		if ((power & 1UL) == 1UL)
		{
		    result = result * pow;
		}

		pow = pow * pow;
		    
		power >>= 1;
	    }
	    return result;
	}
};
