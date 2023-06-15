#pragma once

template<class T>
class Zero {
  public:
    static inline T get() {
	return T(0);
    }
    static inline T get(T const&) {
	return get();
    }
};

template<class T>
T zero() {
    return Zero<T>::get();
}

template<class T>
T zero(T const& t) {
    return Zero<T>::get(t);
}
