#pragma once

#include "TypeUtils.hpp"

#include <vector>

/*
TriangularArray<int> t(5);
t[0][0];
t[1][0]; t[1][1];
t[2][0]; t[2][1]; t[2][2];
t[3][0]; t[3][1]; t[3][2]; t[3][3];
t[4][0]; t[4][1]; t[4][2]; t[4][3]; t[4][4]
 */
template<class T>
class TriangularArray {
  private:
    std::vector<T> fData;

    static SZ triangleSize(UI sideLength) {
	return static_cast<SZ>(sideLength) * (sideLength+1) / 2;
    }

  public:
    TriangularArray(UI n)
	: fData(triangleSize(n)) {
    }
    TriangularArray(UI n, T value)
	: fData(triangleSize(n), value) {
    }

    T* operator[](UI i) {
	return &fData[triangleSize(i)];
    }
    T const* operator[](UI i) const {
	return &fData[triangleSize(i)];
    }
};
