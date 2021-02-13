#pragma once

#include <iostream>
#include <type_traits>
#include "MatrixConstants.hpp"
#include "../Class.hpp"
#include "../Construct.hpp"
#include "../Memory.hpp"
#include "../Zip2.hpp"



template<class T_cvref> class ItemViewIterator;
template<class T_cvref, SZ N> class RowView;
template<class T_cvref, SZ N> class RowViewIterator;
template<class T, SZ M, SZ N> class Matrix;

// T_cvref should be a reference, possibly with const/volatile
// does NOT own the data
// iterator with minimal support for range-based for loops
// basically a possibly-cv pointer that knows if it's & or &&
template<class T_cvref>
class ItemViewIterator {
  private:
    static_assert(std::is_reference_v<T_cvref>,"ItemView should know l-value or r-value reference");
    using T_cv = std::remove_reference_t<T_cvref>;
    
    T_cv* fItem;
  public:
    ItemViewIterator(T_cv* item);
    ItemViewIterator& operator++();
    bool operator!=(ItemViewIterator const & x) const;
    T_cvref operator*();
};

// T_cvref should be a reference, possibly with const/volatile
// does NOT own the data
template<class T_cvref, SZ N>
class RowView {
    static_assert(std::is_reference_v<T_cvref>,"RowView should know l-value or r-value reference");
    using T_cv = std::remove_reference_t<T_cvref>;
    using T = std::remove_cv_t<T_cv>;
    template<class _T, SZ _M, SZ _N> friend class Matrix;
    
    static constexpr SZ size = N;
  private:
    T_cv* fFirstElement; // view of first element in row

    // construct from another row-like object
    template<class SomeRow, class = decltype(declval<SomeRow>()[0])> void constructors(SomeRow&& x);
    // construct using the same arguments for every element
    template<class... Args> void constructors(Construct::Piecewise, Args&&... args);
    void destructors();
  public:
    RowView(T_cv* firstElement);
    // should probably add const + non-const versions, where const return T_cvref with const inside the ref
    T_cvref operator[](SZ j) const;
    ItemViewIterator<T_cvref> begin() const;
    ItemViewIterator<T_cvref> end() const;
    // copy from another row-like object
    template<class SomeRow> RowView& operator=(SomeRow&& x);
    template<class SomeRow> bool operator==(SomeRow const & x);
    template<class SomeRow> bool operator!=(SomeRow const & x);
};

// T_cvref should be a reference, possibly with const/volatile
// does NOT own the data
template<class T_cvref, SZ N>
class RowViewIterator {
  private:
    static_assert(std::is_reference_v<T_cvref>,"RowView should know l-value or r-value reference");
    using T_cv = std::remove_reference_t<T_cvref>;
    
    T_cv* fRowStart;
  public:
    RowViewIterator(T_cv* rowStart);
    RowViewIterator& operator++();
    bool operator!=(RowViewIterator const & x) const;
    RowView<T_cvref,N> operator*();
};

template<class T, SZ M, SZ N>
class Matrix : private SmartMemory<sizeof(T)*M*N> {
    using Memory = SmartMemory<sizeof(T)*M*N>;
    
    using Row      = RowView<T       &, N>;
    using ConstRow = RowView<T const &, N>;
    using RrefRow  = RowView<T      &&, N>;
    
    using RowIterator      = RowViewIterator<T       &, N>;
    using ConstRowIterator = RowViewIterator<T const &, N>;
    using RrefRowIterator  = RowViewIterator<T      &&, N>;
    
  private:
    T* row_addr(SZ i);
    T const * row_addr(SZ i) const;
    
  public:
    static constexpr SZ rows = M;
    static constexpr SZ cols = N;
    static constexpr SZ numel = M*N;
    
    ConstRow operator[](SZ i) const &;
    Row      operator[](SZ i)       &;
    RrefRow  operator[](SZ i)      &&;
    ConstRowIterator begin() const &;
    RowIterator      begin()       &;
    RrefRowIterator  begin()      &&;
    ConstRowIterator   end() const &;
    RowIterator        end()       &;
    RrefRowIterator    end()      &&;

    // construct from another matrix-like object
    template<class OtherMatrix, class = decltype(declval<OtherMatrix>()[0][0])>
    Matrix(OtherMatrix&& x);
    // construct from {r1,r2} because C++ won't deduce std::inializer_list
    template<class OtherRow>
    Matrix(std::initializer_list<OtherRow> const & x);
    // construct from {{a,b},{c,d}} because C++ won't deduce std::inializer_list
    template<class Element>
    Matrix(std::initializer_list<std::initializer_list<Element> > const & x);
    // construct using the same arguments for every element
    template<class... Args>
    Matrix(Construct::Piecewise, Args&&... args);
    // construct using no arguments for every element
    Matrix();
    Matrix(Matrix const & x);
    Matrix(Matrix && x);
    ~Matrix();

    // copy from another matrix-like object
    template<class OtherMatrix, class = decltype(declval<OtherMatrix>()[0][0])>
    Matrix& operator=(OtherMatrix&& x);

    Matrix operator-() const;

    template<class OtherMatrix, class = decltype(declval<OtherMatrix>()[0][0])>
    Matrix& operator+=(OtherMatrix const & other);

    template<class OtherMatrix, class = decltype(declval<OtherMatrix>()[0][0])>
    Matrix& operator-=(OtherMatrix const & other);

    template<class OtherMatrix, class = decltype(declval<OtherMatrix>()[0][0])>
    Matrix operator+(OtherMatrix const & other) const;

    template<class OtherMatrix, class = decltype(declval<OtherMatrix>()[0][0])>
    Matrix&& operator+(OtherMatrix const & other) &&;
    
    Matrix&& operator+(Matrix&& other) const;

    template<class OtherMatrix, class = decltype(declval<OtherMatrix>()[0][0])>
    Matrix operator-(OtherMatrix const & other) const;

    template<class OtherMatrix, class = decltype(declval<OtherMatrix>()[0][0])>
    Matrix&& operator-(OtherMatrix const & other) &&;
    
    Matrix&& operator-(Matrix&& other) const;

    template<class S, SZ P, class R = decltype(declval<T>()*declval<S>())>
    Matrix<R,M,P> operator*(Matrix<S,N,P> const & other) const;

    template<class S>
    Matrix& operator*=(Matrix<S,N,N> const & other);
    
    Matrix operator^(UL exponent) const;

    Matrix& operator^=(UL exponent);
};

template<class T, SZ M, SZ N>
struct Class<Matrix<T,M,N> > {
    using X = Matrix<T,M,N>;
    static std::ostream& print(std::ostream& os, X const & x);
    static Optional<X> parse(std::istream& is);
    static std::string name();
    static std::string format();
};



/* ItemViewIterator template impl */
template<class T_cvref>
ItemViewIterator<T_cvref>::ItemViewIterator(T_cv* item)
    : fItem(item) {
}
template<class T_cvref>
ItemViewIterator<T_cvref>& ItemViewIterator<T_cvref>::operator++() {
    ++fItem;
    return *this;
}
template<class T_cvref>
bool ItemViewIterator<T_cvref>::operator!=(ItemViewIterator const & x) const {
    return fItem != x.fItem;
}
template<class T_cvref>
T_cvref ItemViewIterator<T_cvref>::operator*() {
    return static_cast<T_cvref>(*fItem);
}

/* RowView template impl */
template<class T_cvref, SZ N>
template<class SomeRow, class>
void RowView<T_cvref,N>::constructors(SomeRow&& x) {
    for (auto&&[a,b] : zipNoTmpRvalues(*this,static_cast<SomeRow&&>(x)))
	new(&a) T(static_cast<decltype(b)>(b));
}
template<class T_cvref, SZ N>
template<class... Args>
void RowView<T_cvref,N>::constructors(Construct::Piecewise, Args&&... args) {
    for (T_cvref element : *this)
	new(&element) T(args...);
}
template<class T_cvref, SZ N>
void RowView<T_cvref,N>::destructors() {
    for (T_cvref element : *this)
	element.~T();
}
template<class T_cvref, SZ N>
RowView<T_cvref,N>::RowView(T_cv* firstElement)
    : fFirstElement(firstElement) {
}
template<class T_cvref, SZ N>
T_cvref RowView<T_cvref,N>::operator[](SZ j) const {
    return static_cast<T_cvref>(fFirstElement[j]);
}
template<class T_cvref, SZ N>
ItemViewIterator<T_cvref> RowView<T_cvref,N>::begin() const {
    return fFirstElement;
}
template<class T_cvref, SZ N>
ItemViewIterator<T_cvref> RowView<T_cvref,N>::end() const {
    return fFirstElement + N;
}
template<class T_cvref, SZ N>
template<class SomeRow>
RowView<T_cvref,N>& RowView<T_cvref,N>::operator=(SomeRow&& x) {
    for (auto&& [x,y] : zipNoTmpRvalues(*this,static_cast<SomeRow&&>(x)))
	x = static_cast<decltype(y)>(y);
    return *this;
}
template<class T_cvref, SZ N>
template<class SomeRow>
bool RowView<T_cvref,N>::operator==(SomeRow const & x) {
    for (auto&& [x,y] : zip(*this,x))
	if (!(x == y))
	    return false;
    return true;
}
template<class T_cvref, SZ N>
template<class SomeRow>
bool RowView<T_cvref,N>::operator!=(SomeRow const & x) {
    for (auto&& [x,y] : zip(*this,x))
	if (x != y)
	    return true;
    return false;
}

/* RowViewIterator template impl */
template<class T_cvref, SZ N>
RowViewIterator<T_cvref,N>::RowViewIterator(T_cv* rowStart)
    : fRowStart(rowStart) {
}
template<class T_cvref, SZ N>
RowViewIterator<T_cvref,N>& RowViewIterator<T_cvref,N>::operator++() {
    fRowStart += N;
    return *this;
}
template<class T_cvref, SZ N>
bool RowViewIterator<T_cvref,N>::operator!=(RowViewIterator const & x) const {
    return fRowStart != x.fRowStart;
}
template<class T_cvref, SZ N>
RowView<T_cvref,N> RowViewIterator<T_cvref,N>::operator*() {
    return fRowStart;
}

/* Matrix template impl */
template<class T, SZ M, SZ N>
T* Matrix<T,M,N>::row_addr(SZ i) {
    return reinterpret_cast<T*>(Memory::fData + i*(N*sizeof(T)));
}
template<class T, SZ M, SZ N>
T const * Matrix<T,M,N>::row_addr(SZ i) const {
    return reinterpret_cast<T const *>(Memory::fData + i*(N*sizeof(T)));
}
template<class T, SZ M, SZ N>
typename Matrix<T,M,N>::ConstRow Matrix<T,M,N>::operator[](SZ i) const & {
    return row_addr(i);
}
template<class T, SZ M, SZ N>
typename Matrix<T,M,N>::Row Matrix<T,M,N>::operator[](SZ i) & {
    return row_addr(i);
}
template<class T, SZ M, SZ N>
typename Matrix<T,M,N>::RrefRow Matrix<T,M,N>::operator[](SZ i) && {
    return row_addr(i);
}
template<class T, SZ M, SZ N>
typename Matrix<T,M,N>::ConstRowIterator Matrix<T,M,N>::begin() const & {
    return row_addr(0);
}
template<class T, SZ M, SZ N>
typename Matrix<T,M,N>::RowIterator Matrix<T,M,N>::begin() & {
    return row_addr(0);
}
template<class T, SZ M, SZ N>
typename Matrix<T,M,N>::RrefRowIterator Matrix<T,M,N>::begin() && {
    return row_addr(0);
}
template<class T, SZ M, SZ N>
typename Matrix<T,M,N>::ConstRowIterator Matrix<T,M,N>::end() const & {
    return row_addr(M);
}
template<class T, SZ M, SZ N>
typename Matrix<T,M,N>::RowIterator Matrix<T,M,N>::end() & {
    return row_addr(M);
}
template<class T, SZ M, SZ N>
typename Matrix<T,M,N>::RrefRowIterator Matrix<T,M,N>::end() && {
    return row_addr(M);
}

template<class T, SZ M, SZ N>
template<class OtherMatrix, class>
Matrix<T,M,N>::Matrix(OtherMatrix&& x) {
    for (auto&& [row,x_row] : zipNoTmpRvalues(*this,static_cast<OtherMatrix&&>(x)))
	row.constructors(static_cast<decltype(x_row)>(x_row));
}
template<class T, SZ M, SZ N>
template<class OtherRow>
Matrix<T,M,N>::Matrix(std::initializer_list<OtherRow> const & x) {
    for (auto&& [row,x_row] : zip(*this,x))
	row.constructors(x_row);
}
template<class T, SZ M, SZ N>
template<class Element>
Matrix<T,M,N>::Matrix(std::initializer_list<std::initializer_list<Element> > const & x) {
    for (auto&& [row,x_row] : zip(*this,x))
	row.constructors(x_row);
}
template<class T, SZ M, SZ N>
template<class... Args>
Matrix<T,M,N>::Matrix(Construct::Piecewise, Args&&... args) {
    for (auto row : *this)
	row.constructors(Construct::piecewise, args...);
}
template<class T, SZ M, SZ N>
Matrix<T,M,N>::Matrix()
    : Matrix(Construct::piecewise) {
}
template<class T, SZ M, SZ N>
Matrix<T,M,N>::Matrix(Matrix const & x) {
    for (auto&& [row,x_row] : zip(*this,x))
	row.constructors(x_row);
}
template<class T, SZ M, SZ N>
Matrix<T,M,N>::Matrix(Matrix && x) {
    for (auto&& [row,x_row] : zipNoTmpRvalues(*this,static_cast<Matrix&&>(x)))
	row.constructors(static_cast<decltype(x_row)>(x_row));
}
template<class T, SZ M, SZ N>
Matrix<T,M,N>::~Matrix() {
    for (auto row : *this)
	row.destructors();
}

template<class T, SZ M, SZ N>
template<class OtherMatrix, class>
Matrix<T,M,N>& Matrix<T,M,N>::operator=(OtherMatrix&& x) {
    for (auto&& [row,x_row] : zipNoTmpRvalues(*this,static_cast<OtherMatrix&&>(x)))
	row = static_cast<decltype(x_row)>(x_row);
    return *this;
}

template<class T, SZ M, SZ N>
Matrix<T,M,N> Matrix<T,M,N>::operator-() const {
    Matrix result(*this);
    for (auto&& row : result)
	row = -row;
    return result;
}

template<class T, SZ M, SZ N>
template<class OtherMatrix, class>
Matrix<T,M,N>& Matrix<T,M,N>::operator+=(OtherMatrix const & other) {
    for (SZ i = 0; i < M; ++i)
	for (SZ j = 0; j < N; ++j)
	    (*this)[i][j] += other[i][j];
    return *this;
}
template<class T, SZ M, SZ N>
template<class OtherMatrix, class>
Matrix<T,M,N>& Matrix<T,M,N>::operator-=(OtherMatrix const & other) {
    for (SZ i = 0; i < M; ++i)
	for (SZ j = 0; j < N; ++j)
	    (*this)[i][j] -= other[i][j];
    return *this;
}
template<class T, SZ M, SZ N>
template<class OtherMatrix, class>
Matrix<T,M,N> Matrix<T,M,N>::operator+(OtherMatrix const & other) const {
    Matrix result(*this);
    result += other;
    return result;
}
template<class T, SZ M, SZ N>
template<class OtherMatrix, class>
Matrix<T,M,N>&& Matrix<T,M,N>::operator+(OtherMatrix const & other) && {
    *this += other;
    return static_cast<Matrix&&>(*this);
}
template<class T, SZ M, SZ N>
Matrix<T,M,N>&& Matrix<T,M,N>::operator+(Matrix&& other) const {
    other += *this;
    return static_cast<Matrix&&>(other);
}
template<class T, SZ M, SZ N>
template<class OtherMatrix, class>
Matrix<T,M,N> Matrix<T,M,N>::operator-(OtherMatrix const & other) const {
    Matrix result(*this);
    result -= other;
    return result;
}
template<class T, SZ M, SZ N>
template<class OtherMatrix, class>
Matrix<T,M,N>&& Matrix<T,M,N>::operator-(OtherMatrix const & other) && {
    *this -= other;
    return static_cast<Matrix&&>(*this);
}
template<class T, SZ M, SZ N>
Matrix<T,M,N>&& Matrix<T,M,N>::operator-(Matrix&& other) const {
    for (SZ i = 0; i < M; ++i)
	for (SZ j = 0; j < N; ++j)
	    other[i][j] = (*this)[i][j] - other[i][j];
    return static_cast<Matrix&&>(other);
}
template<class T, SZ M, SZ N>
template<class S, SZ P, class R>
Matrix<R,M,P> Matrix<T,M,N>::operator*(Matrix<S,N,P> const & other) const {
    Matrix<R,M,P> product = Zero<Matrix<R,M,P> >::get();
    for (SZ i = 0; i < M; ++i)
	for (SZ j = 0; j < N; ++j)
	    for (SZ k = 0; k < P; ++k)
		product[i][k] += (*this)[i][j] * other[j][k];
    return product;
}
template<class T, SZ M, SZ N>
template<class S>
Matrix<T,M,N>& Matrix<T,M,N>::operator*=(Matrix<S,N,N> const & other) {
    return *this = (*this).template operator*<S,N,T>(other);
}
template<class T, SZ M, SZ N>
Matrix<T,M,N> Matrix<T,M,N>::operator^(UL exponent) const {
    static_assert(M == N, "Cannot exponentiate non-square matrices");
    Matrix result = One<Matrix>::get();
    for (Matrix pow(*this); exponent > 0; exponent >>= 1) {
	if ((exponent & 1UL) == 1UL)
	    result *= pow;
	pow *= pow;
    }
    return result;
}
template<class T, SZ M, SZ N>
Matrix<T,M,N>& Matrix<T,M,N>::operator^=(UL exponent) {
    return *this = *this ^ exponent;
}

/* Class<Matrix> template impl */
template<class T, SZ M, SZ N>
std::ostream& Class<Matrix<T,M,N> >::print(std::ostream& os, Matrix<T,M,N> const & m) {
    Matrix<std::string,M,N> disp;
    for (SZ i = 0; i < M; ++i)
	for (SZ j = 0; j < N; ++j)
	    disp[i][j] = to_string(m[i][j]);
    for (SZ j = 0; j < N; ++j) {
	SZ maxLen = 0;
	for (SZ i = 0; i < M; ++i)
	    maxLen = std::max(maxLen,disp[i][j].size());
	for (SZ i = 0; i < M; ++i)
	    disp[i][j] = std::string(maxLen-disp[i][j].size(),' ') + disp[i][j];
    }
    os << '\n';
    auto const topLeft = M > 1 ? "┌" : "[";
    auto const botLeft = M > 1 ? "└" : "[";
    auto const edge = "│";
    auto const topRight = M > 1 ? "┐" : "]";
    auto const botRight = M > 1 ? "┘" : "]";
    for (SZ i = 0; i < M; ++i) {
	if (i == 0) {
	    os << topLeft;
	} else if (i+1 == M) {
	    os << botLeft;
	} else {
	    os << edge;
	}
	for (SZ j = 0; j < N; ++j) {
	    if (j > 0)
		os << " ";
	    os << disp[i][j];
	}
	if (i == 0) {
	    os << topRight;
	} else if (i+1 == M) {
	    os << botRight;
	} else {
	    os << edge;
	}
	os << '\n';
    }
    return os;
}
template<class T, SZ M, SZ N>
Optional<Matrix<T,M,N> > Class<Matrix<T,M,N> >::parse(std::istream& is) {
    (void) is;
    // not yet implemented
}
template<class T, SZ M, SZ N>
std::string Class<Matrix<T,M,N> >::name() {
    return "Matrix<" + class_name<T>() + ',' + to_string(M) + ',' + to_string(N) + '>';
}
template<class T, SZ M, SZ N>
std::string Class<Matrix<T,M,N> >::format() {
    return "{" + Class<T>::format() + "...}";
}

