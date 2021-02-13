#pragma once

#include <iostream>

// Based on https://en.wikipedia.org/wiki/Expression_templates

template<typename E,typename T,size_t L>
class MatrixRow
{
private:
    MatrixExpr
}


// For each: T is type of data, R is rows, C is cols

// MATRIX EXPRESSION:
// E is type of MatrixExpr
template<typename E,typename T,size_t R,size_t C>
class MatrixExpr
{
  public:
    inline MatrixRow& operator[](size_t index) const
	{
	    return static_cast<const E&>(*this)[index];
	}

    operator E& ()
	{
	    return static_cast<E&>(*this);
	}

    operator const E& ()
	{
	    return static_cast<const E&>(*this);
	}
};








// MATRIX:

template<typename T,size_t R,size_t C>
class Matrix : public MatrixExpr<Matrix<T,R,C>,T,R,C>
{
  private:
    T* data;
    
  public:
    inline T* operator[](size_t index) const
	{
	    return data + index * C;
	}

    inline size_t numRows() const
	{
	    return R;
	}

    inline size_t numCols() const
	{
	    return C;
	}

    template<typename E>
    Matrix(MatrixExpr<E,T,R,C> const& matExpr)
	: Matrix()
	{
	    for (size_t i = 0; i < R; i++)
	    {
	        for (size_t j = 0; j < C; j++)
		{
		    (*this)[i][j] = matExpr[i][j];
		}
	    }
	}

    Matrix() : data(new T[R*C]) { std::cout << "NEW" << std::endl; }

    ~Matrix() { delete[] data; }
    
    Matrix(const T& init) : Matrix()
	{
	    for (size_t i = 0; i < R; i++)
	    {
	        for (size_t j = 0; j < C; j++)
		{
		    (*this)[i][j] = init;
		}
	    }
	}

    template<typename Z>
    Matrix(const Matrix<Z,R,C>& other) : Matrix()
	{
	    for (size_t i = 0; i < R; i++)
	    {
	        for (size_t j = 0; j < C; j++)
		{
		    (*this)[i][j] = other[i][j];
		}
	    }
	}
};

// ostream << operator
template<typename T,size_t R,size_t C>
std::ostream& operator<<(std::ostream& os,const Matrix<T,R,C>& M)
{
    for (size_t i = 0; i < R; i++)
    {
	for (size_t j = 0; j < C; j++)
	{
	    os << M[i][j] << " ";
	}
	os << std::endl;
    }
    return os;
}








// MATRIX SUM
// E1 is type of first expr, E2 is type of 2nd expr
template<typename E1,typename E2,typename T,size_t R,size_t C>
class MatrixSum : public MatrixExpr<MatrixSum<E1,E2,T,R,C>,T,R,C>
{
  private:
    const MatrixExpr<E1,T,R,C>& m_1;
    const MatrixExpr<E2,T,R,C>& m_2;
    
  public:
    inline VectorExpr& operator[](size_t index) const
	{
	    return m_1[index] + m_2[index];
	}

    MatrixSum(const MatrixExpr<E1,T,R,C>& _m_1,
	      const MatrixExpr<E2,T,R,C>& _m_2)
	: m_1(_m_1), m_2(_m_2) { }
};

template<typename E1,typename E2,typename T,size_t R,size_t C>
const MatrixSum<E1,E2,T,R,C>
operator+(
    const MatrixExpr<E1,T,R,C>& e1,
    const MatrixExpr<E2,T,R,C>& e2
    )
{
    return MatrixSum<E1,E2,T,R,C>(e1,e2);
}








// MATRIX PRODUCT
// E1 is type of first expr, E2 is type of 2nd expr
template<typename E1,typename E2,typename T,size_t R,size_t C>
class MatrixProd : public MatrixExpr<MatrixProd<E1,E2,T,R,C>,T,R,C>
{
  private:
    const Matrix<T,R,C> m_1;
    const Matrix<T,R,C> m_2;
    
  public:
    inline T* operator[](size_t index) const
	{
	    return m_1[index] + m_2[index];
	}

    MatrixSum(const MatrixExpr<E1,T,R,C>& _m_1,
	      const MatrixExpr<E2,T,R,C>& _m_2)
	: m_1(_m_1), m_2(_m_2) { }
};

template<typename E1,typename E2,typename T,size_t R,size_t C>
const MatrixSum<E1,E2,T,R,C>
operator+(
    const MatrixExpr<E1,T,R,C>& e1,
    const MatrixExpr<E2,T,R,C>& e2
    )
{
    return MatrixSum<E1,E2,T,R,C>(e1,e2);
}
