#pragma once

#include "UniqueOwnedReferenceForHPP.hpp"

template<class T> template<class... Args>
UniqueOwnedReference<T>::UniqueOwnedReference(Args&&... args)
    : t(*new T(static_cast<Args&&>(args)...)) {
}

template<class T>
UniqueOwnedReference<T>::~UniqueOwnedReference() {
    delete &t;
}
