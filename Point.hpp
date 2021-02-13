#pragma once

#include "Class.hpp"
#include "MyHashUtil.hpp"

template<class Data>
struct Point2D {
    Data x;
    Data y;
    bool operator==(Point2D const & o) const {
	return x == o.x && y == o.y;
    }
};
MAKE_HASHABLE_TEMPLATE(<class Data>,Point2D<Data>,obj,obj.x,obj.y);
template<class Data>
struct Class<Point2D<Data> > {
    using T = Point2D<Data>;
    static std::ostream& print(std::ostream& os, T const & t) {
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
    bool operator==(Point3D const & o) const {
	return x == o.x && y == o.y && z == o.z;
    }
};
MAKE_HASHABLE_TEMPLATE(<class Data>,Point3D<Data>,obj,obj.x,obj.y,obj.z);

