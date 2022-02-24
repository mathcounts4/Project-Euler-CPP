#pragma once

template<typename T>
class One {
  public:
    static inline T get() {
	return T(1);
    }
};

template<class T>
T one() {
    return One<T>::get();
}
