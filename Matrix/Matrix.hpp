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
    bool operator!=(ItemViewIterator const& x) const;
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
    template<class SomeRow, class = decltype(zipNoTmpRvalues(declval<RowView&>(),declval<SomeRow&&>())/*.begin() - commented for initializer list Matrix initialization due to clang bug https://bugs.llvm.org/show_bug.cgi?id=47175 */)> void constructors(SomeRow&& x);
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
    template<class SomeRow> bool operator==(SomeRow const& x);
    template<class SomeRow> bool operator!=(SomeRow const& x);
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
    bool operator!=(RowViewIterator const& x) const;
    RowView<T_cvref,N> operator*();
};

template<class T, SZ M, SZ N>
class Matrix : private SmartMemory<sizeof(T)*M*N, alignof(T)> {
    using Memory = SmartMemory<sizeof(T)*M*N, alignof(T)>;
    
    using Row      = RowView<T      &, N>;
    using ConstRow = RowView<T const&, N>;
    using RrefRow  = RowView<T      &&, N>;
    
    using RowIterator      = RowViewIterator<T      &, N>;
    using ConstRowIterator = RowViewIterator<T const&, N>;
    using RrefRowIterator  = RowViewIterator<T      &&, N>;
    
  private:
    T* row_addr(SZ i);
    T const* row_addr(SZ i) const;
    
  public:
    static constexpr SZ rows = M;
    static constexpr SZ cols = N;
    static constexpr SZ numel = M*N;
    
    ConstRow operator[](SZ i) const&;
    Row      operator[](SZ i)      &;
    RrefRow  operator[](SZ i)      &&;
    ConstRowIterator begin() const&;
    RowIterator      begin()      &;
    RrefRowIterator  begin()      &&;
    ConstRowIterator   end() const&;
    RowIterator        end()      &;
    RrefRowIterator    end()      &&;

    // construct from another matrix-like object
    template<class OtherMatrix, class = decltype(declval<OtherMatrix>()[0][0])>
    Matrix(OtherMatrix&& x);
    // construct from {r1,r2} because C++ won't deduce std::inializer_list
    template<class OtherRow>
    Matrix(std::initializer_list<OtherRow> const& x);
    // construct from {{a,b},{c,d}} because C++ won't deduce std::inializer_list
    template<class Element>
    Matrix(std::initializer_list<std::initializer_list<Element> > const& x);
    // construct using the same arguments for every element
    template<class... Args>
    Matrix(Construct::Piecewise, Args&&... args);
    // construct using no arguments for every element
    Matrix();
    Matrix(Matrix const& x);
    Matrix(Matrix&& x);
    ~Matrix();

    template<SZ M_ST, SZ M_END, SZ N_ST, SZ N_END>
    Matrix<T, M_END - M_ST, N_END - N_ST> slice() const;

    T const& uniqueElement() const;

    // copy from another matrix-like object
    template<class OtherMatrix, class = decltype(declval<OtherMatrix>()[0][0])>
    Matrix& operator=(OtherMatrix&& x);

    Matrix operator-() const;

    template<class OtherMatrix, class = decltype(declval<OtherMatrix>()[0][0])>
    Matrix& operator+=(OtherMatrix const& other);

    template<class OtherMatrix, class = decltype(declval<OtherMatrix>()[0][0])>
    Matrix& operator-=(OtherMatrix const& other);

    template<class OtherMatrix, class = decltype(declval<OtherMatrix>()[0][0])>
    Matrix operator+(OtherMatrix const& other) const;

    template<class OtherMatrix, class = decltype(declval<OtherMatrix>()[0][0])>
    Matrix&& operator+(OtherMatrix const& other) &&;
    
    Matrix&& operator+(Matrix&& other) const;

    template<class OtherMatrix, class = decltype(declval<OtherMatrix>()[0][0])>
    Matrix operator-(OtherMatrix const& other) const;

    template<class OtherMatrix, class = decltype(declval<OtherMatrix>()[0][0])>
    Matrix&& operator-(OtherMatrix const& other) &&;
    
    Matrix&& operator-(Matrix&& other) const;

    template<class S, SZ P, class R = decltype(declval<T>()*declval<S>())>
    Matrix<R,M,P> operator*(Matrix<S,N,P> const& other) const;

    template<class S>
    Matrix& operator*=(Matrix<S,N,N> const& other);

    // solves M⨯M lhs * M⨯N result = *this
    Matrix ldivide(Matrix<T,M,M> lhs) const;
    
    Matrix operator^(UL exponent) const;

    Matrix& operator^=(UL exponent);

    template<class OtherMatrix, class = decltype(declval<OtherMatrix>()[0][0])>
    bool operator==(OtherMatrix const& other) const;

    template<class OtherMatrix, class = decltype(declval<OtherMatrix>()[0][0])>
    bool operator!=(OtherMatrix const& other) const;
};

template<class T, SZ M, SZ N>
struct Class<Matrix<T,M,N> > {
    using X = Matrix<T,M,N>;
    static std::ostream& print(std::ostream& os, X const& x);
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
bool ItemViewIterator<T_cvref>::operator!=(ItemViewIterator const& x) const {
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
RowView<T_cvref,N>& RowView<T_cvref,N>::operator=(SomeRow&& r) {
    for (auto&& [x,y] : zipNoTmpRvalues(*this,static_cast<SomeRow&&>(r)))
	x = static_cast<decltype(y)>(y);
    return *this;
}
template<class T_cvref, SZ N>
template<class SomeRow>
bool RowView<T_cvref,N>::operator==(SomeRow const& other) {
    for (auto&& [x,y] : zip(*this, other))
	if (!(x == y))
	    return false;
    return true;
}
template<class T_cvref, SZ N>
template<class SomeRow>
bool RowView<T_cvref,N>::operator!=(SomeRow const& other) {
    for (auto&& [x,y] : zip(*this, other))
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
bool RowViewIterator<T_cvref,N>::operator!=(RowViewIterator const& x) const {
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
T const* Matrix<T,M,N>::row_addr(SZ i) const {
    return reinterpret_cast<T const*>(Memory::fData + i*(N*sizeof(T)));
}
template<class T, SZ M, SZ N>
typename Matrix<T,M,N>::ConstRow Matrix<T,M,N>::operator[](SZ i) const& {
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
typename Matrix<T,M,N>::ConstRowIterator Matrix<T,M,N>::begin() const& {
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
typename Matrix<T,M,N>::ConstRowIterator Matrix<T,M,N>::end() const& {
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
Matrix<T,M,N>::Matrix(std::initializer_list<OtherRow> const& x) {
    for (auto&& [row,x_row] : zip(*this,x))
	row.constructors(x_row);
}
template<class T, SZ M, SZ N>
template<class Element>
Matrix<T,M,N>::Matrix(std::initializer_list<std::initializer_list<Element> > const& x) {
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
Matrix<T,M,N>::Matrix(Matrix const& x) {
    for (auto&& [row,x_row] : zip(*this,x))
	row.constructors(x_row);
}
template<class T, SZ M, SZ N>
Matrix<T,M,N>::Matrix(Matrix&& x) {
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
template<SZ M_ST, SZ M_END, SZ N_ST, SZ N_END>
Matrix<T, M_END - M_ST, N_END - N_ST> Matrix<T,M,N>::slice() const {
    static_assert(M_ST < M_END);
    static_assert(N_ST < N_END);
    constexpr SZ M_REAL_END = std::min(M, M_END);
    constexpr SZ N_REAL_END = std::min(N, N_END);
    auto result = Zero<Matrix<T, M_END - M_ST, N_END - N_ST>>::get();
    for (SZ i = M_ST; i < M_REAL_END; ++i) {
	auto const& src = (*this)[i];
	auto&& dst = result[i - M_ST];
	for (SZ j = N_ST; j < N_REAL_END; ++j) {
	    dst[j - N_ST] = src[j];
	}
    }
    return result;
}

template<class T, SZ M, SZ N>
T const& Matrix<T,M,N>::uniqueElement() const {
    static_assert(M == 1 && N == 1, "uniqueElement can only be called on 1x1 matrices");
    return (*this)[0][0];
}

template<class T, SZ M, SZ N>
Matrix<T,M,N> Matrix<T,M,N>::operator-() const {
    Matrix result(*this);
    for (auto&& row : result)
	for (auto&& x : row)
	    x = -x;
    return result;
}

template<class T, SZ M, SZ N>
template<class OtherMatrix, class>
Matrix<T,M,N>& Matrix<T,M,N>::operator+=(OtherMatrix const& other) {
    for (SZ i = 0; i < M; ++i)
	for (SZ j = 0; j < N; ++j)
	    (*this)[i][j] += other[i][j];
    return *this;
}
template<class T, SZ M, SZ N>
template<class OtherMatrix, class>
Matrix<T,M,N>& Matrix<T,M,N>::operator-=(OtherMatrix const& other) {
    for (SZ i = 0; i < M; ++i)
	for (SZ j = 0; j < N; ++j)
	    (*this)[i][j] -= other[i][j];
    return *this;
}
template<class T, SZ M, SZ N>
template<class OtherMatrix, class>
Matrix<T,M,N> Matrix<T,M,N>::operator+(OtherMatrix const& other) const {
    Matrix result(*this);
    result += other;
    return result;
}
template<class T, SZ M, SZ N>
template<class OtherMatrix, class>
Matrix<T,M,N>&& Matrix<T,M,N>::operator+(OtherMatrix const& other) && {
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
Matrix<T,M,N> Matrix<T,M,N>::operator-(OtherMatrix const& other) const {
    Matrix result(*this);
    result -= other;
    return result;
}
template<class T, SZ M, SZ N>
template<class OtherMatrix, class>
Matrix<T,M,N>&& Matrix<T,M,N>::operator-(OtherMatrix const& other) && {
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
namespace Strassen {
    constexpr SZ minDimSize = 256;
    template<SZ M, SZ N, SZ P>
    constexpr bool shouldStrassen = M >= minDimSize && N >= minDimSize && P >= minDimSize &&
	/* most big matrix mult is exponentiation, so we only handle exact match for now */
	M == N && M == P;
};
template<SZ M, SZ N, SZ P, class = void>
struct FastMultiply {
    template<class T, class S, class R = decltype(declval<T>()*declval<S>())>
    static Matrix<R,M,P> multiply(Matrix<T,M,N> const& x, Matrix<S,N,P> const& y) {
	// naive matrix mult
	Matrix<R,M,P> product = Zero<Matrix<R,M,P>>::get();
	for (SZ i = 0; i < M; ++i) {
	    auto const& xRow = x[i];
	    auto&& productRow = product[i];
	    for (SZ j = 0; j < N; ++j) {
		auto const& yCol = y[j];
		auto const& xEl = xRow[j];
		for (SZ k = 0; k < P; ++k)
		    productRow[k] += xEl * yCol[k];
	    }
	}
	return product;
    }
};
template<SZ M, SZ N, SZ P>
struct FastMultiply<M, N, P, std::enable_if_t<Strassen::shouldStrassen<M, N, P>>> {
    template<class T, class S>
    static Matrix<decltype(declval<T>()*declval<S>()),M,P> multiply(Matrix<T,M,N> const& A, Matrix<S,N,P> const& B) {
	using R = decltype(declval<T>()*declval<S>());
	constexpr SZ M_2 = M/2 + (M&1);
	constexpr SZ N_2 = N/2 + (P&1);
	constexpr SZ P_2 = N/2 + (P&1);

	auto A11 = A.template slice<0, M_2, 0, N_2>();
	auto A12 = A.template slice<0, M_2, N_2, N_2 * 2>();
	auto A21 = A.template slice<M_2, M_2 * 2, 0, N_2>();
	auto A22 = A.template slice<M_2, M_2 * 2, N_2, N_2 * 2>();

	auto B11 = B.template slice<0, N_2, 0, P_2>();
	auto B12 = B.template slice<0, N_2, P_2, P_2 * 2>();
	auto B21 = B.template slice<N_2, N_2 * 2, 0, P_2>();
	auto B22 = B.template slice<N_2, N_2 * 2, P_2, P_2 * 2>();
	
	// https://en.wikipedia.org/wiki/Strassen_algorithm
	auto M1 = (A11 + A22) * (B11 + B22);
	auto M2 = (A21 + A22) * B11;
	auto M3 = A11 * (B12 - B22);
	auto M4 = A22 * (B21 - B11);
	auto M5 = (A11 + A12) * B22;
	auto M6 = (A21 - A11) * (B11 + B12);
	auto M7 = (A12 - A22) * (B21 + B22);
	auto C11 = M1 + M4  - M5  + M7;
	auto C12 = M3 + M5;
	auto C21 = M2 + M4;
	auto C22 = M1 - M2 + M3  + M6;

        auto product = Zero<Matrix<R,M,P>>::get();
	for (SZ i = 0; i < M_2; ++i) {
	    auto const& src = C11[i];
	    auto&& dst = product[i];
	    for (SZ j = 0; j < P_2; ++j) {
		dst[j] = src[j];
	    }
	}
	for (SZ i = 0; i < M_2; ++i) {
	    auto const& src = C12[i];
	    auto&& dst = product[i];
	    for (SZ j = P_2; j < P; ++j) {
		dst[j] = src[j - P_2];
	    }
	}
	for (SZ i = M_2; i < M; ++i) {
	    auto const& src = C21[i - M_2];
	    auto&& dst = product[i];
	    for (SZ j = 0; j < P_2; ++j) {
		dst[j] = src[j];
	    }
	}
	for (SZ i = M_2; i < M; ++i) {
	    auto const& src = C22[i - M_2];
	    auto&& dst = product[i];
	    for (SZ j = P_2; j < P; ++j) {
		dst[j] = src[j - P_2];
	    }
	}
	return product;
    }
};
    
template<class T, SZ M, SZ N>
template<class S, SZ P, class R>
Matrix<R,M,P> Matrix<T,M,N>::operator*(Matrix<S,N,P> const& other) const {
    return FastMultiply<M, N, P>::template multiply<T, S>(*this, other);
}
template<class T, SZ M, SZ N>
template<class S>
Matrix<T,M,N>& Matrix<T,M,N>::operator*=(Matrix<S,N,N> const& other) {
    return *this = (*this).template operator*<S,N,T>(other);
}

// solves M⨯M lhs * M⨯N result = *this
template<class T, SZ M, SZ N>
Matrix<T,M,N> Matrix<T,M,N>::ldivide(Matrix<T,M,M> lhs) const {
    static_assert(!std::is_integral_v<T>, "Cannot properly divide integer matrices - use floating-point instead");
    // Gauss-Jordan elimination
    Matrix rhs = *this;
    // ignore rows that were already chosen for some column
    std::array<B, M> usedRows{};
    std::array<SZ, M> rowToCol{};
    for (SZ col = 0; col < M; ++col) {
	SZ biggestRow = 0;
	T biggestAbsRowVal(0);
	for (SZ row = 0; row < M; ++row) {
	    if (usedRows[row]) {
		continue;
	    }
	    auto absVal = std::abs(lhs[row][col]);
	    if (absVal > biggestAbsRowVal) {
		biggestAbsRowVal = absVal;
		biggestRow = row;
	    }
	}
	if (biggestAbsRowVal == static_cast<T>(0)) {
	    throw_exception<std::logic_error>("Singular matrix in column " + to_string(col) + " with row " + to_string(biggestRow) + " = " + to_string(lhs[biggestRow][col]) + " with biggestAbsRowVal = " + to_string(biggestAbsRowVal) + ":\n" + to_string(lhs));
	}
	usedRows[biggestRow] = true;
	rowToCol[biggestRow] = col;
	for (SZ row = 0; row < M; ++row) {
	    if (row != biggestRow) {
		auto scale = lhs[row][col] / lhs[biggestRow][col];
		for (SZ colSub = 0; colSub < N; ++colSub) {
		    rhs[row][colSub] -= scale * rhs[biggestRow][colSub];
		}
		for (SZ colSub = col+1; colSub < M; ++colSub) {
		    lhs[row][colSub] -= scale * lhs[biggestRow][colSub];
		}
		lhs[row][col] = static_cast<T>(0);
	    }
	}
    }
    // Need to permute rhs and divide by entries in lhs
    // We have a sparse LHS with entries in [row][col] (one per row, one per col).
    auto permutedScaled = rhs;
    for (SZ row = 0; row < M; ++row) {
	auto col = rowToCol[row];
	auto scale = lhs[row][col];
	// lhs[row][...] * ans[...][i] = rhs[row][i]
	// lhs[row] only has nonzero "scale" at lhs[row][col]
	// lhs[row][col] * ans[col][i] = rhs[row][i]
	for (SZ i = 0; i < N; ++i) {
	    permutedScaled[col][i] = rhs[row][i] / scale;
	}
    }
    return permutedScaled;
}

template<class T, SZ M, SZ N>
Matrix<T,M,N> Matrix<T,M,N>::operator^(UL exponent) const {
    static_assert(M == N, "Cannot exponentiate non-square matrices");
    Matrix result = One<Matrix>::get();
    for (Matrix pow(*this); ; ) {
	if (exponent & 1UL)
	    result *= pow;
	exponent >>= 1;
	if (!exponent)
	    break;
	pow *= pow;
    }
    return result;
}
template<class T, SZ M, SZ N>
Matrix<T,M,N>& Matrix<T,M,N>::operator^=(UL exponent) {
    return *this = *this ^ exponent;
}

namespace matrix::detail {
    template<class T> static constexpr bool isMatrix = false;
    template<class T, SZ M, SZ N> static constexpr bool isMatrix<Matrix<T, M, N>> = true;
}

template<class T, SZ M, SZ N>
template<class OtherMatrix, class>
bool Matrix<T,M,N>::operator==(OtherMatrix const& other) const {
    if constexpr (matrix::detail::isMatrix<OtherMatrix>) {
	static_assert(OtherMatrix::rows == M && OtherMatrix::cols == N);
    }
    for (SZ i = 0; i < M; ++i)
	for (SZ j = 0; j < N; ++j)
	    if ((*this)[i][j] != other[i][j]) {
		return false;
	    }
    return true;
}
template<class T, SZ M, SZ N>
template<class OtherMatrix, class>
bool Matrix<T,M,N>::operator!=(OtherMatrix const& other) const {
    return !(*this == other);
}

/* Class<Matrix> template impl */
template<class T, SZ M, SZ N>
std::ostream& Class<Matrix<T,M,N> >::print(std::ostream& os, Matrix<T,M,N> const& m) {
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

