#pragma once

#include <iostream>
#include <typeinfo>

// This class stores a matrix of T's.
// for identity and *, T needs to have a constructor T(0) that returns 'zero'
// for identity and ^, T needs to have a constructor T(1) that returns 'one'

// Most important operator supported:
// []
// Access TestMatrix<int> x by using x[i][j] to set and get

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
// TestMatrix<int> a(3,4,-23);
// TestMatrix<double> b(3,4,0.3);
// b += a; a = b; a*b; 


// The following operators are also supported, (not templatized)
// (unary) -
// (assignment) =(rvalue)
// (binary) ^
// NOTE: the ^ operator requires T to have the * operator

template<typename T>
class TestMatrix
{
  private:
    unsigned int m_rows;
    unsigned int m_cols;
    T* m_data;
    
  public:
    TestMatrix()
	{
	    std::cout << "default constructor" << std::endl;
	}
    
    TestMatrix(unsigned int rows,
	       unsigned int cols) :
	m_rows(rows),
	m_cols(cols),
	m_data(new T[rows*cols])
	{
	    std::cout << "constructor" << std::endl;
	}

    TestMatrix(unsigned int rows,
	       unsigned int cols,
	       const T& init) :
	TestMatrix(rows,cols)
	{
	    std::cout << "init constructor" << std::endl;
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    (*this)[i][j] = init;
		}
	    }
	}

    template<typename... Args>
    TestMatrix(unsigned int rows,
	       unsigned int cols,
	       const Args&&... args) :
	TestMatrix(rows,cols)
	{
	    std::cout << "init(args) constructor" << std::endl;
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    (*this)[i][j] = T(args...);
		}
	    }
	}
    
    ~TestMatrix()
	{
	    delete[] m_data;
	}

    inline unsigned int numRows() const { return m_rows; }
    inline unsigned int numCols() const { return m_cols; }

    TestMatrix(const TestMatrix& other) :
	TestMatrix(other.m_rows,other.m_cols)
	{
	    std::cout << "copy constructor" << std::endl;
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    (*this)[i][j] = other[i][j];
		}
	    }
	}

    template<typename R>
    TestMatrix(const TestMatrix<R>& other) :
	TestMatrix(other.numRows(),other.numCols())
	{
	    std::cout << "template copy constructor" << std::endl;
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    (*this)[i][j] = T(other[i][j]);
		}
	    }
	}

    TestMatrix(TestMatrix&& other)
	{
	    std::cout << "&& (steal) constructor" << std::endl;
	    m_rows = other.m_rows;
	    m_cols = other.m_cols;
	    m_data = other.m_data;
	    other.m_data = nullptr;
	}

    TestMatrix& operator=(const TestMatrix& other)
	{
	    std::cout << "op=(&)" << std::endl;
	    if (this != &other)
	    {
		if (this->m_rows * this->m_cols != other.m_rows * other.m_cols)
		{
		    std::cout << "Size mismatch using Matrix '=' operator, reallocating memory..." << std::endl;
		    delete[] m_data;
		    m_data = new T[other.m_rows * other.m_cols];
		}
		m_rows = other.m_rows;
		m_cols = other.m_cols;
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
    TestMatrix& operator=(const TestMatrix<R>& other)
	{
	    std::cout << "template op=(&)" << std::endl;
	    if (this->m_rows * this->m_cols != other.numRows() * other.numCols())
	    {
		std::cout << "Size mismatch using template Matrix '=' operator, reallocating memory..." << std::endl;
		delete[] m_data;
		m_data = new T[other.numRows() * other.numCols()];
	    }
	    m_rows = other.numRows();
	    m_cols = other.numCols();
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    (*this)[i][j] = T(other[i][j]);
		}
	    }
	    return *this;
	}

    TestMatrix& operator=(TestMatrix&& other)
	{
	    std::cout << "op=(&&)" << std::endl;
	    delete[] m_data;
	    m_rows = other.m_rows;
	    m_cols = other.m_cols;
	    m_data = other.m_data;
	    other.m_data = nullptr;
	    return *this;
	}

    inline T* operator[](unsigned int row) const
	{
	    return m_data + row * m_cols;
	}

    void print() const
	{
	    T example_T;
	    std::cout << m_rows << " by " << m_cols << " of " <<
		typeid(example_T).name() << ":" << std::endl;
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

    TestMatrix& negate()
	{
	    std::cout << "negate" << std::endl;
	    
	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    (*this)[i][j] = -(*this)[i][j];
		}
	    }

	    return *this;
	}

    TestMatrix operator-()
	{
	    std::cout << "- op (unary)" << std::endl;
	    TestMatrix result(m_rows,m_cols);

	    for (auto i = 0U; i < m_rows; i++)
	    {
		for (auto j = 0U; j < m_cols; j++)
		{
		    result[i][j] = -(*this)[i][j];
		}
	    }
	    
	    return result;
	}

    TestMatrix& operator+=(const TestMatrix& other)
	{
	    std::cout << "+= op" << std::endl;

	    if (m_rows != other.numRows() ||
		m_cols != other.numCols())
	    {
		std::cout << "Dimension mismatch using '+=' operator. Doing nothing." << std::endl;
	    }
	    else
	    {
		for (auto i = 0U; i < m_rows; i++)
		{
		    for (auto j = 0U; j < m_cols; j++)
		    {
			(*this)[i][j] += other[i][j];
		    }
		}
	    }

	    return *this;
	}
    
    template<typename R>
    TestMatrix& operator+=(const TestMatrix<R>& other)
	{
	    std::cout << "template += op" << std::endl;

	    if (m_rows != other.numRows() ||
		m_cols != other.numCols())
	    {
		std::cout << "Dimension mismatch using template '+=' operator. Doing nothing." << std::endl;
	    }
	    else
	    {
		for (auto i = 0U; i < m_rows; i++)
		{
		    for (auto j = 0U; j < m_cols; j++)
		    {
			(*this)[i][j] += T(other[i][j]);
		    }
		}
	    }

	    return *this;
	}

    TestMatrix& operator-=(const TestMatrix& other)
	{
	    std::cout << "-= op" << std::endl;

	    if (m_rows != other.numRows() ||
		m_cols != other.numCols())
	    {
		std::cout << "Dimension mismatch using '-=' operator. Doing nothing." << std::endl;
	    }
	    else
	    {
		for (auto i = 0U; i < m_rows; i++)
		{
		    for (auto j = 0U; j < m_cols; j++)
		    {
			(*this)[i][j] -= other[i][j];
		    }
		}
	    }

	    return *this;
	}

    template<typename R>
    TestMatrix& operator-=(const TestMatrix<R>& other)
	{
	    std::cout << "template -= op" << std::endl;

	    if (m_rows != other.numRows() ||
		m_cols != other.numCols())
	    {
		std::cout << "Dimension mismatch using template '-=' operator. Doing nothing." << std::endl;
	    }
	    else
	    {
		for (auto i = 0U; i < m_rows; i++)
		{
		    for (auto j = 0U; j < m_cols; j++)
		    {
			(*this)[i][j] -= T(other[i][j]);
		    }
		}
	    }

	    return *this;
	}

    TestMatrix operator+(const TestMatrix& other) const
	{
	    std::cout << "+ op" << std::endl;

	    if (m_rows != other.numRows() ||
		m_cols != other.numCols())
	    {
		std::cout << "Dimension mismatch using '+' operator. Returning empty array." << std::endl;
		return TestMatrix(0,0);
	    }
	    else
	    {
		TestMatrix sum(m_rows,m_cols);
		for (auto i = 0U; i < m_rows; i++)
		{
		    for (auto j = 0U; j < m_cols; j++)
		    {
			sum[i][j] = (*this)[i][j] + other[i][j];
		    }
		}
		return sum;
	    }
	}

    template<typename R>
    TestMatrix operator+(const TestMatrix<R>& other) const
	{
	    std::cout << "template + op" << std::endl;

	    if (m_rows != other.numRows() ||
		m_cols != other.numCols())
	    {
		std::cout << "Dimension mismatch using template '+' operator. Returning empty array." << std::endl;
		return TestMatrix(0,0);
	    }
	    else
	    {
		TestMatrix sum(m_rows,m_cols);
		for (auto i = 0U; i < m_rows; i++)
		{
		    for (auto j = 0U; j < m_cols; j++)
		    {
			sum[i][j] = (*this)[i][j] + T(other[i][j]);
		    }
		}
		return sum;
	    }
	}

    TestMatrix&& operator+(TestMatrix&& other) const
	{
	    std::cout << "+ && op" << std::endl;

	    if (m_rows != other.numRows() ||
		m_cols != other.numCols())
	    {
		std::cout << "Dimension mismatch using '+' && operator. Returning second operand." << std::endl;
		return std::move(other);
	    }
	    else
	    {
		for (auto i = 0U; i < m_rows; i++)
		{
		    for (auto j = 0U; j < m_cols; j++)
		    {
			other[i][j] = (*this)[i][j] + other[i][j];
		    }
		}
	    }
	    return std::move(other);
	}

    TestMatrix operator-(const TestMatrix& other) const
	{
	    std::cout << "- op (binary)" << std::endl;

	    if (m_rows != other.numRows() ||
		m_cols != other.numCols())
	    {
		std::cout << "Dimension mismatch using '-' operator. Returning empty array." << std::endl;
		return TestMatrix(0,0);
	    }
	    else
	    {
		TestMatrix difference(m_rows,m_cols);
		for (auto i = 0U; i < m_rows; i++)
		{
		    for (auto j = 0U; j < m_cols; j++)
		    {
			difference[i][j] = (*this)[i][j] - other[i][j];
		    }
		}
		return difference;
	    }
	}
    
    template<typename R>
    TestMatrix operator-(const TestMatrix<R>& other) const
	{
	    std::cout << "template - op (binary)" << std::endl;

	    if (m_rows != other.numRows() ||
		m_cols != other.numCols())
	    {
		std::cout << "Dimension mismatch using template '-' operator. Returning empty array." << std::endl;
		return TestMatrix(0,0);
	    }
	    else
	    {
		TestMatrix difference(m_rows,m_cols);
		for (auto i = 0U; i < m_rows; i++)
		{
		    for (auto j = 0U; j < m_cols; j++)
		    {
			difference[i][j] = (*this)[i][j] - T(other[i][j]);
		    }
		}
		return difference;
	    }
	}

    TestMatrix& operator-(TestMatrix&& other) const
	{
	    std::cout << "- && op" << std::endl;

	    if (m_rows != other.numRows() ||
		m_cols != other.numCols())
	    {
		std::cout << "Dimension mismatch using '-' && operator. Returning second operand." << std::endl;
		return other;
	    }
	    else
	    {
		for (auto i = 0U; i < m_rows; i++)
		{
		    for (auto j = 0U; j < m_cols; j++)
		    {
			other[i][j] = (*this)[i][j] - other[i][j];
		    }
		}
	    }
	    return other;
	}

    TestMatrix operator*(const TestMatrix& other) const
	{
	    std::cout << "* op" << std::endl;

	    if (m_cols != other.numRows())
	    {
		std::cout << "Dimension mismatch using '*' operator. Returning empty array." << std::endl;
	        return TestMatrix(0,0);
	    }
	    else
	    {
		TestMatrix product(m_rows,other.numCols(),0);
		for (auto i = 0U; i < m_rows; i++)
		{
		    for (auto j = 0U; j < m_cols; j++)
		    {
			for (auto k = 0U; k < other.numCols(); k++)
			{
			    product[i][k] += (*this)[i][j] * other[j][k];
			}
		    }
		}
		return product;
	    }
	}

    template<typename R>
    TestMatrix operator*(const TestMatrix<R>& other) const
	{
	    std::cout << "template * op" << std::endl;

	    if (m_cols != other.numRows())
	    {
		std::cout << "Dimension mismatch using template '*' operator. Returning empty array." << std::endl;
	        return TestMatrix(0,0);
	    }
	    else
	    {
		TestMatrix product(m_rows,other.numCols(),0);
		for (auto i = 0U; i < m_rows; i++)
		{
		    for (auto j = 0U; j < m_cols; j++)
		    {
			for (auto k = 0U; k < other.numCols(); k++)
			{
			    product[i][k] += (*this)[i][j] * T(other[j][k]);
			}
		    }
		}
		return product;
	    }
	}

    static TestMatrix identity(unsigned int dim)
	{
	    TestMatrix value(dim,dim,0);
	    for (auto i = 0U; i < dim; i++)
	    {
		value[i][i] = T(1);
	    }
	    return value;
	}

    TestMatrix operator^(unsigned int power) const
	{
	    std::cout << "^ op" << std::endl;

	    if (m_rows != m_cols)
	    {
		std::cout << "Dimension mismatch using '^' operator. Returning empty array." << std::endl;
		return TestMatrix(0,0);
	    }
	    else
	    {
		std::cout << "'^' operator identity" << std::endl;
		TestMatrix result = TestMatrix::identity(m_rows);

		std::cout << "'^' operator pow init" << std::endl;
		TestMatrix pow(*this);
		
		while (power > 0U)
		{
		    if ((power & 1U) == 1U)
		    {
			std::cout << "'^' operator result mult" << std::endl;
			result = result * pow;
		    }

		    std::cout << "'^' operator pow mult" << std::endl;
		    pow = pow * pow;
		    
		    power >>= 1;
		}
		return result;
	    }
	}
};
