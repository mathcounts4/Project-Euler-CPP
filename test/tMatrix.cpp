#include "../Matrix/Matrix.hpp"
#include "harness.hpp"

#include "../Mod.hpp"
#include "../TypeUtils.hpp"
#include "../Vec.hpp"

struct MustPassInt {
    MustPassInt(int) {}
};

struct Uncopyable {
    Uncopyable() = default;
    Uncopyable(Uncopyable const&) = delete;
    Uncopyable(Uncopyable&&) = default;
    Uncopyable& operator=(Uncopyable const&) = delete;
    Uncopyable& operator=(Uncopyable &&) = default;
};

struct FromUncopyable {
    FromUncopyable(Uncopyable&&) {}
};

TEST(Matrix, Construction) {
    // default
    Matrix<D, 2, 3> md;
    for (UI i = 0; i < 2; ++i) {
	for (UI j = 0; j < 3; ++j) {
	    CHECK(md[i][j], equals(0.0));
	    md[i][j] = i + j;
	}
    }

    // copy from another datatype
    Matrix<UI, 2, 3> mi(md);
    for (UI i = 0; i < 2; ++i) {
	for (UI j = 0; j < 3; ++j) {
	    CHECK(mi[i][j], equals(i + j));
	}
    }

    // move should call move on each element (same or different datatype)
    Matrix<Uncopyable, 1, 2> mun1;
    Matrix<Uncopyable, 1, 2> mun2(std::move(mun1));
    mun2 = std::move(mun1);
    Matrix<FromUncopyable, 1, 2> mfun4(std::move(mun2));
    mfun4 = std::move(mun2);

    // braced-init lists
    Matrix<MustPassInt, 2, 2> mpi1{{1, 2}, {3, 4}};
    Vec<MustPassInt> row;
    Matrix<MustPassInt, 2, 2> mpi2{row, row};

    // piecewise
    Matrix<MustPassInt, 4, 5> mpi3(Construct::piecewise, 4);
}

TEST(Matrix, Iteration) {
    Matrix<SI, 2, 3> m;
    SI i = 0;
    for (auto&& row : m) {
	for (auto&& x : row) {
	    x = i++;
	}
    }
    CHECK(m[0][0], equals(0));
    CHECK(m[0][1], equals(1));
    CHECK(m[0][2], equals(2));
    CHECK(m[1][0], equals(3));
    CHECK(m[1][1], equals(4));
    CHECK(m[1][2], equals(5));
}

TEST(Matrix, Empty) {
    Matrix<SI, 3, 0> mempty1;
    Matrix<SI, 0, 3> mempty2;
    Matrix<SI, 0, 0> mempty3;
}

TEST(Matrix, Constants) {
    auto m0 = zero<Matrix<SI, 2, 1>>();
    CHECK(m0[0][0], equals(0));
    CHECK(m0[1][0], equals(0));

    // non-square 1: all elements
    auto m1 = one<Matrix<SI, 2, 1>>();
    CHECK(m1[0][0], equals(1));
    CHECK(m1[1][0], equals(1));

    // square 1: only diagonal
    auto m22 = one<Matrix<SI, 2, 2>>();
    CHECK(m22[0][0], equals(1));
    CHECK(m22[1][0], equals(0));
    CHECK(m22[0][1], equals(0));
    CHECK(m22[1][1], equals(1));
}

TEST(Matrix, Slicing) {
    Matrix<SI, 2, 3> m{{1, 2, 3}, {4, 5, 6}};
    auto sliced = m.slice<0, 1, 1, 3>();
    static_assert(sliced.rows == 1);
    static_assert(sliced.cols == 2);
    static_assert(sliced.numel == 2);
    CHECK(sliced[0][0], equals(2));
    CHECK(sliced[0][1], equals(3));
}

TEST(Matrix, UniqueElement) {
    Matrix<SI, 1, 1> m{{4}};
    CHECK(m.uniqueElement(), equals(4));
}

TEST(Matrix, Equality) {
    Matrix<D, 1, 2> x{{1, 2}};
    Matrix<SI, 1, 2> x2{{1, 2}};
    Matrix<SI, 1, 2> y{{1, 3}};
    
    // same datatype
    CHECK(x == x, isTrue());
    CHECK(x2 == y, isFalse());
    
    // different datatypes
    CHECK(x == x2, isTrue());
    CHECK(x == y, isFalse());
    
    // same datatype
    CHECK(x != x, isFalse());
    CHECK(x2 != y, isTrue());
    
    // different datatypes
    CHECK(x != x2, isFalse());
    CHECK(x != y, isTrue());
}

TEST(Matrix, Negation) {
    // works with different datatypes
    Matrix<D, 1, 2> m{{1, 2}};
    auto result = -m;
    CHECK(result[0][0], equals(-1));
    CHECK(result[0][1], equals(-2));
}

TEST(Matrix, Addition) {
    // works with different datatypes
    auto result = Matrix<D, 1, 2>{{1, 2}} + Matrix<SI, 1, 2>{{3, 4}};
    CHECK(result[0][0], equals(4));
    CHECK(result[0][1], equals(6));

    result += Matrix<SI, 1, 2>{{2, 2}};
    CHECK(result[0][0], equals(6));
    CHECK(result[0][1], equals(8));
}

TEST(Matrix, Subtraction) {
    // works with different datatypes
    auto result = Matrix<D, 1, 2>{{1, 2}} - Matrix<SI, 1, 2>{{3, 5}};
    CHECK(result[0][0], equals(-2));
    CHECK(result[0][1], equals(-3));

    result -= Matrix<SI, 1, 2>{{2, 2}};
    CHECK(result[0][0], equals(-4));
    CHECK(result[0][1], equals(-5));
}

TEST(Matrix, Multiplication) {
    // works with different datatypes
    auto result = Matrix<D, 1, 2>{{1, 2}} * Matrix<SI, 2, 1>{{3}, {5}};
    CHECK(result[0][0], equals(13));

    result *= Matrix<SI, 1, 1>{{2}};
    CHECK(result[0][0], equals(26));

    auto bigger = Matrix<SI, 2, 3>{
	{1, 3, 5},
	{7, 2, 4}
    } * Matrix<SI, 3, 4>{
	{1, 4, 2, 3},
	{3, 1, 0, 2},
	{1, 1, 9, 5}
    };
    Matrix<SI, 2, 4> expected{
	{15, 12, 47, 34},
	{17, 34, 50, 45}
    };
    CHECK(bigger, equals(expected));
}

TEST(Matrix, Exponentiation) {
    using T = Matrix<SI, 2, 2>;
    T m{
	{1, 1},
	{2, 3}
    };
    CHECK(m ^ 0, equals(one<T>()));
    CHECK(m ^ 1, equals(m));
    CHECK(m ^ 2, equals(T{{3, 4}, {8, 11}}));
    CHECK(m ^ 3, equals(T{{11, 15}, {30, 41}}));

    // Verify we can do large exponents in log(exp) time
    using M = Matrix<Mod, 3, 3>;
    Mod::GlobalModSentinel sentinel(static_cast<UI>(1e9+7));
    CHECK((M{{1, 5, 7}, {6, 23, 77}, {94, 6, 2}} ^ 2345786234992857692UL), equals(M{{599276685, 357509479, 459514963}, {940844896, 509439784, 704523770}, {662176952, 581640396, 522866088}}));

    Matrix<D, 3, 3> d{
	{1, 0, 0},
	{0, 0, 0},
	{0, 0, 2}
    };
    CHECK(d ^ 1022, equals(Matrix<D, 3, 3>{{1., 0., 0.}, {0., 0., 0.}, {0., 0., std::pow(2, 1022)}}));
}

TEST(Matrix, Ldivide) {
    using M22 = Matrix<D, 2, 2>;
    using M23 = Matrix<D, 2, 3>;

    // solving AX = B for X

    M22 A{
	{2, 5},
	{3, 7}
    };
    M23 X{
	{8, 9, 22},
	{87, 56, -3}
    };
    auto B = A * X;
    auto calcX = B.ldivide(A);
    auto precision = std::pow(2, -20);
    for (auto&& row : calcX)
	for (auto&& x : row)
	    x = std::round(x / precision) * precision;
    CHECK(calcX, equals(X));
}
