#pragma once

#include "BadOptionalAccess.hpp"
#include "ExitUtils.hpp"
#include "Failure.hpp"
#include "TypeUtils.hpp"

#include <string>
#include <utility>
#include <variant>

template<class T> struct Optional {
  private:
    std::variant<T,std::string> value;

  public:
    Optional(Failure f);
    template<class... R> Optional(R&&... r);
    
    explicit operator bool() const;
    
    std::string const& cause() const;
    
    T& operator*();
    T const& operator*() const;
    T* operator->();
    T const* operator->() const;
    T& deref_without_stack();
    T const& deref_without_stack() const;
};

template<>
struct Optional<void> {
  private:
    std::optional<std::string> value;
    
  public:
    Optional();
    Optional(Failure f);

    operator bool() const;

    std::string const& cause() const;

    void operator*() const;
    void deref_without_stack() const;
};

template<class T>
static constexpr B is_Optional = false;
template<class T>
static constexpr B is_Optional<Optional<T>> = true;



/* Impl */

template<class T>
Optional<T>::Optional(Failure f)
    : value(std::in_place_index<1>,std::move(f.reason)) {
}

template<class T> template<class... R>
Optional<T>::Optional(R&&... r)
    : value(std::in_place_index<0>,fwd<R>(r)...) {
}

template<class T>
Optional<T>::operator bool() const {
    return value.index() == 0;
}

template<class T>
std::string const& Optional<T>::cause() const {
    if (*this)
	throw_exception<BadOptionalAccess>("cause() called on a valid Optional");
    return std::get<1>(value);
}

template<class T>
T& Optional<T>::operator*() {
    if (!*this)
	throw_exception<BadOptionalAccess>(cause());
    return std::get<0>(value);
}

template<class T>
T const& Optional<T>::operator*() const {
    if (!*this)
	throw_exception<BadOptionalAccess>(cause());
    return std::get<0>(value);
}

template<class T>
T* Optional<T>::operator->() {
    return &**this;
}

template<class T>
T const* Optional<T>::operator->() const {
    return &**this;
}

template<class T>
T& Optional<T>::deref_without_stack() {
    if (!*this)
	throw_exception_no_stack<BadOptionalAccess>(cause());
    return std::get<0>(value);
}

template<class T>
T const& Optional<T>::deref_without_stack() const {
    if (!*this)
	throw_exception_no_stack<BadOptionalAccess>(cause());
    return std::get<0>(value);
}

