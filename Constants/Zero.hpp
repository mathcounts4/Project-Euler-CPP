#pragma once

template<class T>
class Zero {
  public:
    static inline T get() {
	return T(0);
    }
};

template<class T>
T zero() {
    return Zero<T>::get();
}
