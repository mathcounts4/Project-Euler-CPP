#include "../CircleIterator.hpp"
#include "harness.hpp"

#include "../Vec.hpp"
#include "../Zip2.hpp"

TEST(CircleIteratorX,zero) {
    SI cnt = 0;
    for (auto point : CircleIteratorX(0)) {
	CHECK(point, equals(CirclePoint{0,0}));
	++cnt;
    }
    CHECK(cnt, equals(1));
}

TEST(CircleIteratorX,one) {
    Vec<CirclePoint> expected {
	{1,0},
	{0,1},
	{-1,0},
	{0,-1}
    };
    CircleIteratorX actual(1);
    for (auto [expected_point,actual_point] : zip(expected,actual))
	CHECK(expected_point, equals(actual_point));
}

TEST(CircleIteratorX,two) {
    Vec<CirclePoint> expected {
	{2,0},
	{1,1},
	{0,2},
	{-1,1},
	{-2,0},
	{-1,-1},
	{0,-2},
	{1,-1}
    };
    CircleIteratorX actual(2);
    for (auto [actual_point,expected_point] : zip(actual,expected))
	CHECK(actual_point, equals(expected_point));
}

TEST(CircleIteratorX,n) {
    for (SI n : {5,7,12,65,100,3814}) {
	for (auto actual : CircleIteratorX(static_cast<UI>(n))) {
	    SI const x = actual.x;
	    SI const abs_y = static_cast<SI>(std::sqrt(n*n-x*x));
	    SI const y = actual.y > 0 ? abs_y : -abs_y;
	    CirclePoint expected{x,y};
	    CHECK(actual, equals(expected));
	}
    }
}



TEST(CircleIterator,zero) {
    Vec<std::pair<CirclePoint,CirclePoint> > expected {
	{{0,0},{1,0}},
	{{0,0},{0,1}},
	{{0,0},{-1,0}},
	{{0,0},{0,-1}}
    };
    CircleIterator actual(0);
    for (auto [expected_points,actual_points] : zip(expected,actual))
	CHECK(expected_points, equals(actual_points));
}

TEST(CircleIterator,one) {
    Vec<std::pair<CirclePoint,CirclePoint> > expected {
	{{1,0},{2,0}},
	{{1,0},{1,1}},
	{{0,1},{1,1}},
	{{0,1},{0,2}},
	{{0,1},{-1,1}},
	{{-1,0},{-1,1}},
	{{-1,0},{-2,0}},
	{{-1,0},{-1,-1}},
	{{0,-1},{-1,-1}},
	{{0,-1},{0,-2}},
	{{0,-1},{1,-1}},
	{{1,0},{1,-1}}
    };
    CircleIterator actual(1);
    for (auto [expected_points,actual_points] : zip(expected,actual))
	CHECK(expected_points, equals(actual_points));
}

TEST(CircleIterator,n) {
    for (SI const s_n : {5,7,12,65,100,3814}) {
	UI const n = static_cast<UI>(s_n);
	for (auto [inside,outside] : CircleIterator(n)) {
	    CHECK(inside.x*inside.x + inside.y*inside.y, isLeq(s_n*s_n));
	    CHECK(outside.x*outside.x + outside.y*outside.y, isGreaterThan(s_n*s_n));
	}
    }
}

TEST(CircleIterator,big) {
    // note that 1000000^2 is bigger than the max UI
    UI const n = 1000000;
    SI const s_n = n;
    auto sq = [](SL x) -> SL { return x*x; };
    for (auto [inside,outside] : CircleIterator(n)) {
	CHECK(sq(inside.x) + sq(inside.y), isLeq(sq(s_n)));
	CHECK(sq(outside.x) + sq(outside.y), isGreaterThan(sq(s_n)));
    }
}

