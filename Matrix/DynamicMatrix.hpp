#pragma once

#include <iostream>
#include <typeinfo>

// This class stores a matrix of T's.
// for identity and *, T needs to have a constructor T(0) that returns 'zero'
// for identity and ^, T needs to have a constructor T(1) that returns 'one'

// Most important operator supported:
// []
// Access DynamicMatrix<int> x by using x[i][j] to set and get

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
// DynamicMatrix<int> a(3,4,-23);
// DynamicMatrix<double> b(3,4,0.3);
// b += a; a = b; a*b; 


// The following operators are also supported, (not templatized)
// (unary) -
// (assignment) =(rvalue)
// (binary) ^
// NOTE: the ^ operator requires T to have the * operator

template<typename T>
class DynamicMatrix {
  private:
    unsigned int m_rows;
    unsigned int m_cols;
    T* m_data;
    
  public:
    DynamicMatrix(unsigned int rows,
		  unsigned int cols)
	: m_rows(rows),
	  m_cols(cols),
	  m_data(new T[rows*cols]) {}

    DynamicMatrix(unsigned int rows,
		  unsigned int cols,
		  const T& init)
	: DynamicMatrix(rows,cols) {
	for (auto i = 0U; i < m_rows; i++) {
	    for (auto j = 0U; j < m_cols; j++) {
		(*this)[i][j] = init;
	    }
	}
    }

    template<typename... Args>
    DynamicMatrix(unsigned int rows,
		  unsigned int cols,
		  const Args&&... args)
	: DynamicMatrix(rows,cols) {
	for (auto i = 0U; i < m_rows; i++) {
	    for (auto j = 0U; j < m_cols; j++) {
		(*this)[i][j] = T(args...);
	    }
	}
    }
    
    ~DynamicMatrix() {
	delete[] m_data;
    }

    inline unsigned int numRows() const { return m_rows; }
    inline unsigned int numCols() const { return m_cols; }

    DynamicMatrix(const DynamicMatrix& other)
	: DynamicMatrix(other.m_rows,other.m_cols) {
	for (auto i = 0U; i < m_rows; i++) {
	    for (auto j = 0U; j < m_cols; j++) {
		(*this)[i][j] = other[i][j];
	    }
	}
    }

    template<typename R>
    DynamicMatrix(const DynamicMatrix<R>& other)
        : DynamicMatrix(other.numRows(),other.numCols()) {
	for (auto i = 0U; i < m_rows; i++) {
	    for (auto j = 0U; j < m_cols; j++) {
		(*this)[i][j] = T(other[i][j]);
	    }
	}
    }

    DynamicMatrix(DynamicMatrix&& other)
	: m_rows(other.m_rows),
	  m_cols(other.m_cols),
	  m_data(other.m_data) {
	other.m_data = nullptr;
    }
    
    DynamicMatrix& operator=(const DynamicMatrix& other) {
	if (this != &other) {
	    if (this->m_rows * this->m_cols != other.m_rows * other.m_cols) {
		delete[] m_data;
		m_data = new T[other.m_rows * other.m_cols];
	    }
	    m_rows = other.m_rows;
	    m_cols = other.m_cols;
	    for (auto i = 0U; i < m_rows; i++) {
		for (auto j = 0U; j < m_cols; j++) {
		    (*this)[i][j] = other[i][j];
		}
	    }
	}
	return *this;
    }

    template<typename R>
    DynamicMatrix& operator=(const DynamicMatrix<R>& other) {
	if (this->m_rows * this->m_cols != other.numRows() * other.numCols()) {
	    delete[] m_data;
	    m_data = new T[other.numRows() * other.numCols()];
	}
	m_rows = other.numRows();
	m_cols = other.numCols();
	for (auto i = 0U; i < m_rows; i++) {
	    for (auto j = 0U; j < m_cols; j++) {
		(*this)[i][j] = T(other[i][j]);
	    }
	}
	return *this;
    }

    DynamicMatrix& operator=(DynamicMatrix&& other) {
	delete[] m_data;
	m_rows = other.m_rows;
	m_cols = other.m_cols;
	m_data = other.m_data;
	other.m_data = nullptr;
	return *this;
    }

    inline T* operator[](unsigned int row) const {
	return m_data + row * m_cols;
    }

    void print() const {
	T example_T;
	std::cout << m_rows << " by " << m_cols << " of " <<
	    typeid(example_T).name() << ":" << std::endl;
	for (auto i = 0U; i < m_rows; i++) {
	    for (auto j = 0U; j < m_cols; j++) {
		std::cout << (*this)[i][j] << " ";
	    }
	    std::cout << std::endl;
	}
    }

    bool isEmpty() const{
	return (m_rows == 0) || (m_cols == 0);
    }

    DynamicMatrix& negate() {
	for (auto i = 0U; i < m_rows; i++) {
	    for (auto j = 0U; j < m_cols; j++) {
		(*this)[i][j] = -(*this)[i][j];
	    }
	}

	return *this;
    }

    DynamicMatrix operator-() {
	DynamicMatrix result(m_rows,m_cols);

	for (auto i = 0U; i < m_rows; i++) {
	    for (auto j = 0U; j < m_cols; j++) {
		result[i][j] = -(*this)[i][j];
	    }
	}
	    
	return result;
    }

    DynamicMatrix& operator+=(const DynamicMatrix& other) {
	if (m_rows != other.numRows() ||
	    m_cols != other.numCols()) {
	} else {
	    for (auto i = 0U; i < m_rows; i++) {
		for (auto j = 0U; j < m_cols; j++) {
		    (*this)[i][j] += other[i][j];
		}
	    }
	}

	return *this;
    }
    
    template<typename R>
    DynamicMatrix& operator+=(const DynamicMatrix<R>& other) {
	if (m_rows != other.numRows() ||
	    m_cols != other.numCols()) {
	} else {
	    for (auto i = 0U; i < m_rows; i++) {
		for (auto j = 0U; j < m_cols; j++) {
		    (*this)[i][j] += T(other[i][j]);
		}
	    }
	}

	return *this;
    }

    DynamicMatrix& operator-=(const DynamicMatrix& other) {
	if (m_rows != other.numRows() ||
	    m_cols != other.numCols()) {
	} else {
	    for (auto i = 0U; i < m_rows; i++) {
		for (auto j = 0U; j < m_cols; j++) {
		    (*this)[i][j] -= other[i][j];
		}
	    }
	}

	return *this;
    }

    template<typename R>
    DynamicMatrix& operator-=(const DynamicMatrix<R>& other) {
	if (m_rows != other.numRows() ||
	    m_cols != other.numCols()) {
	} else {
	    for (auto i = 0U; i < m_rows; i++) {
		for (auto j = 0U; j < m_cols; j++) {
		    (*this)[i][j] -= T(other[i][j]);
		}
	    }
	}

	return *this;
    }

    DynamicMatrix operator+(const DynamicMatrix& other) const {
	if (m_rows != other.numRows() ||
	    m_cols != other.numCols()) {
	    return DynamicMatrix(0,0);
	} else {
	    DynamicMatrix sum(m_rows,m_cols);
	    for (auto i = 0U; i < m_rows; i++) {
		for (auto j = 0U; j < m_cols; j++) {
		    sum[i][j] = (*this)[i][j] + other[i][j];
		}
	    }
	    return sum;
	}
    }

    template<typename R>
    DynamicMatrix operator+(const DynamicMatrix<R>& other) const {
	if (m_rows != other.numRows() ||
	    m_cols != other.numCols()) {
	    return DynamicMatrix(0,0);
	} else {
	    DynamicMatrix sum(m_rows,m_cols);
	    for (auto i = 0U; i < m_rows; i++) {
		for (auto j = 0U; j < m_cols; j++) {
		    sum[i][j] = (*this)[i][j] + T(other[i][j]);
		}
	    }
	    return sum;
	}
    }

    DynamicMatrix operator-(const DynamicMatrix& other) const {
	if (m_rows != other.numRows() ||
	    m_cols != other.numCols()) {
	    return DynamicMatrix(0,0);
	} else {
	    DynamicMatrix difference(m_rows,m_cols);
	    for (auto i = 0U; i < m_rows; i++) {
		for (auto j = 0U; j < m_cols; j++) {
		    difference[i][j] = (*this)[i][j] - other[i][j];
		}
	    }
	    return difference;
	}
    }
    
    template<typename R>
    DynamicMatrix operator-(const DynamicMatrix<R>& other) const {
	if (m_rows != other.numRows() ||
	    m_cols != other.numCols()) {
	    return DynamicMatrix(0,0);
	} else {
	    DynamicMatrix difference(m_rows,m_cols);
	    for (auto i = 0U; i < m_rows; i++) {
		for (auto j = 0U; j < m_cols; j++) {
		    difference[i][j] = (*this)[i][j] - T(other[i][j]);
		}
	    }
	    return difference;
	}
    }

    DynamicMatrix operator*(const DynamicMatrix& other) const {
	if (m_cols != other.numRows()) {
	    return DynamicMatrix(0,0);
	} else {
	    DynamicMatrix product(m_rows,other.numCols(),0);
	    for (auto i = 0U; i < m_rows; i++) {
		for (auto j = 0U; j < m_cols; j++) {
		    for (auto k = 0U; k < other.numCols(); k++) {
			product[i][k] += (*this)[i][j] * other[j][k];
		    }
		}
	    }
	    return product;
	}
    }

    template<typename R>
    DynamicMatrix operator*(const DynamicMatrix<R>& other) const {
	if (m_cols != other.numRows()) {
	    return DynamicMatrix(0,0);
	} else {
	    DynamicMatrix product(m_rows,other.numCols(),0);
	    for (auto i = 0U; i < m_rows; i++) {
		for (auto j = 0U; j < m_cols; j++) {
		    for (auto k = 0U; k < other.numCols(); k++) {
			product[i][k] += (*this)[i][j] * T(other[j][k]);
		    }
		}
	    }
	    return product;
	}
    }

    static DynamicMatrix identity(unsigned int dim) {
	DynamicMatrix value(dim,dim,0);
	for (auto i = 0U; i < dim; i++) {
	    value[i][i] = T(1);
	}
	return value;
    }

    DynamicMatrix operator^(unsigned int power) const {
	if (m_rows != m_cols) {
	    return DynamicMatrix(0,0);
	} else {
	    DynamicMatrix result = DynamicMatrix::identity(m_rows);

	    DynamicMatrix pow(*this);
		
	    while (power > 0U) {
		if ((power & 1U) == 1U) {
		    result = result * pow;
		}

		pow = pow * pow;
		    
		power >>= 1;
	    }
	    return result;
	}
    }
};
