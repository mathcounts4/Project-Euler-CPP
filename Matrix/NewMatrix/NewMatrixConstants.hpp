#pragma once

#include "../../Constants/Zero.hpp"
#include "../../Constants/One.hpp"

template<typename T,unsigned int m_cols,unsigned int m_rows>
class NewMatrix;

template<typename T,unsigned int m_rows,unsigned int m_cols>
class Zero<NewMatrix<T,m_rows,m_cols> >
{
  public:
    static inline NewMatrix<T,m_rows,m_cols> get()
	{
	    return NewMatrix<T,m_rows,m_cols>(Zero<T>::get());
	}
};

template<typename T,unsigned int dim>
class One<NewMatrix<T,dim,dim> >
{
  public:
    static inline NewMatrix<T,dim,dim> get()
	{
	    NewMatrix<T,dim,dim> result(Zero<T>::get());
	    T one = One<T>::get();
	    for (auto i = 0U; i < dim; i++)
	    {
		result[i][i] = one;
	    }
	    return result;
	}
};
