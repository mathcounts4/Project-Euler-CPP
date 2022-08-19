#pragma once

#include "UniqueOwnedReferenceForHPP.hpp"

template<class T> template<class... Args>
UniqueOwnedReference<T>::UniqueOwnedReference(Args&&... args)
    : t(std::make_unique<T>(static_cast<Args&&>(args)...)) {
    // If T's constructor is not public and this causes a compilation error,
    //   pass your own constructed T (after friending the owner class) and ensure that T is publicly movable.
}

template<class T>
UniqueOwnedReference<T>::~UniqueOwnedReference() = default;
