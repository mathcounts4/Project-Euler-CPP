#pragma once

#include <memory>

/* This class is meant to help with header #include overhead
 * Requirements:
 * 1. class X "owns" a member y of class Y, constructed upon X's construction
 * 2. X.hpp should NOT #include Y.hpp
 * Implementation:
 * 1. X.hpp #include's UniqueOwnedReferenceForHPP.hpp and a header forward declaring Y
 * 2. X.cpp #includes UniqueOwnedReferenceForCPP.cpp
 * 3. Every cpp using y as a Y (preseumably at least X.cpp) #include's Y.hpp
 */ 
template<class T>
struct UniqueOwnedReference {
  private:
    // Though t is nullable, it will only be null if this object has been moved-from
    //   (in which case, why are you looking at this object?)
    std::unique_ptr<T> t;
  public:
    template<class... Args> UniqueOwnedReference(Args&&... args);
    UniqueOwnedReference(UniqueOwnedReference&&) = default;
    UniqueOwnedReference& operator=(UniqueOwnedReference&&) = default;
    ~UniqueOwnedReference();
    operator T& () noexcept {
	return *t;
    }
    operator T const& () const noexcept {
	return *t;
    }
    T& operator*() noexcept {
	return *t;
    }
    T const& operator*() const noexcept {
	return *t;
    }
    T* operator->() noexcept {
	return t.get();
    }
    T const* operator->() const noexcept {
	return t.get();
    }
};
