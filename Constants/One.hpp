#pragma once

template<typename T>
class One {
  public:
    static inline T get() {
	return T(1);
    }
    static inline T get(T const&) {
	return get();
    }
};

template<class T>
T one() {
    return One<T>::get();
}

template<class T>
T one(T const& t) {
    return One<T>::get(t);
}
