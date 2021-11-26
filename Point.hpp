#pragma once

#include "Class.hpp"
#include "MyHashUtil.hpp"

template<class Data>
struct Point2D {
    Data x;
    Data y;
    friend bool operator==(Point2D const& a, Point2D const& b) {
	return a.x == b.x && a.y == b.y;
    }
    friend bool operator<(Point2D const& a, Point2D const& b) {
	if (a.x != b.x) {
	    return a.x < b.x;
	}
	if (a.y != b.y) {
	    return a.y < b.y;
	}
	return false;
    }
};
MAKE_HASHABLE_TEMPLATE(<class Data>,Point2D<Data>,obj,obj.x,obj.y);
template<class Data>
struct Class<Point2D<Data> > {
    using T = Point2D<Data>;
    static std::ostream& print(std::ostream& os, T const& t) {
	return os << "{" << t.x << "," << t.y << "}";
    }
    static Optional<T> parse(std::istream& is) {
	Resetter resetter{is};
	if (auto opt = consume_opt(is,'{'); !opt)
	    return Failure(opt.cause());
	auto x = Class<Data>::parse(is);
	if (!x)
	    return Failure(x.cause());
	if (auto opt = consume_opt(is,','); !opt)
	    return Failure(opt.cause());
	auto y = Class<Data>::parse(is);
	if (!y)
	    return Failure(y.cause());
	if (auto opt = consume_opt(is,'}'); !opt)
	    return Failure(opt.cause());
	resetter.ignore();
	return {x,y};
    }
    static std::string name() {
	return "Point2D<" + Class<T>::name() + ">";
    }
    static std::string format() {
	return "{" + Class<T>::format() + "," + Class<T>::format() + "}";
    }
};

template<class Data>
struct Point3D {
    Data x;
    Data y;
    Data z;
    friend bool operator==(Point3D const& a, Point3D const& b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
    }
    friend bool operator<(Point3D const& a, Point3D const& b) {
	if (a.x != b.x) {
	    return a.x < b.x;
	}
	if (a.y != b.y) {
	    return a.y < b.y;
	}
	if (a.z != b.z) {
	    return a.z < b.z;
	}
	return false;
    }
};
MAKE_HASHABLE_TEMPLATE(<class Data>,Point3D<Data>,obj,obj.x,obj.y,obj.z);

