#pragma once

// Need to have the template definition
// CLASS<TYPE,ROWS,COLS>

#include "../../Constants/Zero.hpp"

#include <iostream>

#define LOOP(I,I_MAX,J,J_MAX,EXPR)		\
    for (auto I = 0U; I < I_MAX; I++)		\
    {						\
	for (auto J = 0U; J < J_MAX; j++)	\
	{					\
	    EXPR;				\
	}					\
    }						\
    0

#define MATRIX_GUTS(CLASS,TYPE,ROWS,COLS)				\
private:								\
    TYPE* m_data;							\
    									\
public:									\
CLASS() : m_data(new TYPE[ROWS*COLS]) {std::cout << "NEW\n"; }		\
									\
    ~CLASS() { delete[] m_data; }					\
									\
    CLASS(const TYPE& init)						\
	: CLASS()							\
    {									\
	LOOP(i,ROWS,							\
	     j,COLS,							\
	     (*this)[i][j] = init					\
	    );								\
    }									\
									\
    CLASS(const CLASS& other)						\
	: CLASS()							\
    {									\
	*this = other;							\
    }									\
									\
    template<typename OTHERTYPE>					\
    CLASS(const CLASS<OTHERTYPE,ROWS,COLS>& other)			\
	: CLASS()							\
    {									\
	*this = other;							\
    }									\
									\
    CLASS(CLASS&& other) :						\
	m_data(other.m_data)						\
    {									\
	std::cout << "STEAL\n";						\
	other.m_data = nullptr;						\
    }									\
									\
    inline unsigned int numRows(void) const				\
    {									\
	return ROWS;							\
    }									\
									\
    inline unsigned int numCols(void) const				\
    {									\
	return COLS;							\
    }									\
									\
    inline TYPE* operator[](unsigned int row) const			\
    {									\
	return m_data + row * COLS;					\
    }									\
									\
    CLASS& operator=(const CLASS& other)				\
    {									\
        LOOP(i,ROWS,							\
	     j,COLS,							\
	     (*this)[i][j] = other[i][j]				\
	    );								\
	return *this;							\
    }									\
									\
    template<typename OTHERTYPE>					\
    CLASS& operator=(const CLASS<OTHERTYPE,ROWS,COLS>& other)		\
    {									\
        LOOP(i,ROWS,							\
	     j,COLS,							\
	     (*this)[i][j] = other[i][j]				\
	    );								\
	return *this;							\
    }									\
									\
    CLASS& operator=(CLASS&& other)					\
    {									\
	delete[] m_data;						\
	m_data = other.m_data;						\
	other.m_data = nullptr;						\
	return *this;							\
    }									\
									\
    CLASS& negate()							\
    {									\
        LOOP(i,ROWS,							\
	     j,COLS,							\
	     (*this)[i][j] = -((*this)[i][j])				\
	    );								\
	return *this;							\
    }									\
									\
    CLASS operator-() const						\
    {									\
	CLASS result;							\
        LOOP(i,ROWS,							\
	     j,COLS,							\
	     result[i][j] = -((*this)[i][j])				\
	    );								\
	return result;							\
    }									\
									\
    template<typename OTHERTYPE>					\
    CLASS& operator+=(const CLASS<OTHERTYPE,ROWS,COLS>& other)		\
    {									\
        LOOP(i,ROWS,							\
	     j,COLS,							\
	     (*this)[i][j] += other[i][j]				\
	    );								\
	return *this;							\
    }									\
									\
    template<typename OTHERTYPE>					\
    CLASS& operator-=(const CLASS<OTHERTYPE,ROWS,COLS>& other)		\
    {									\
        LOOP(i,ROWS,							\
	     j,COLS,							\
	     (*this)[i][j] -= other[i][j]				\
	    );								\
	return *this;							\
    }									\
									\
    template<typename OTHERTYPE>					\
    CLASS operator+(const CLASS<OTHERTYPE,ROWS,COLS>& other) const	\
    {									\
	CLASS sum;							\
	LOOP(i,ROWS,							\
	     j,COLS,							\
	     sum[i][j] = (*this)[i][j] + other[i][j]			\
	    );								\
	return sum;							\
    }									\
									\
    CLASS&& operator+(CLASS&& other) const				\
    {									\
	LOOP(i,ROWS,							\
	     j,COLS,							\
	     other[i][j] = (*this)[i][j] + other[i][j]			\
	    );								\
	return std::move(other);					\
    }									\
									\
    template<typename OTHERTYPE>					\
    CLASS operator-(const CLASS<OTHERTYPE,ROWS,COLS>& other) const	\
    {									\
	CLASS dif;							\
	LOOP(i,ROWS,							\
	     j,COLS,							\
	     dif[i][j] = (*this)[i][j] - other[i][j]			\
	    );								\
	return dif;							\
    }									\
									\
    CLASS&& operator-(CLASS&& other) const				\
    {									\
	LOOP(i,ROWS,							\
	     j,COLS,							\
	     other[i][j] = (*this)[i][j] - other[i][j]			\
	    );								\
	return std::move(other);					\
    }									\
									\
    template<typename OTHERTYPE,unsigned int OTHERDIM>			\
    CLASS<TYPE,ROWS,OTHERDIM> operator*(				\
	const CLASS<OTHERTYPE,COLS,OTHERDIM>& other) const		\
    {									\
	CLASS<TYPE,ROWS,OTHERDIM> product(Zero<TYPE>::get());		\
	LOOP(i,ROWS,							\
	     j,COLS,							\
	     for (auto k = 0U; k < OTHERDIM; k++)			\
	     {								\
		 product[i][k] += (*this)[i][j] * other[j][k];		\
	     }								\
	    );								\
	return product;							\
    }									\
									\
    bool isEmpty() const						\
    {									\
	return ((ROWS == 0) &&						\
		(COLS == 0));						\
    }									\
									\
    void print() const							\
    {									\
        for (auto i = 0U; i < ROWS; i++)				\
	{								\
	    for (auto j = 0U; j < COLS; j++)				\
	    {								\
		std::cout << (*this)[i][j] << " ";			\
	    }								\
	    std::cout << std::endl;					\
	}								\
    }
