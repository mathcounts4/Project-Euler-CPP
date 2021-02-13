#include "../UniqueOwnedReferenceForCPP.hpp"
#include "tUseRef_X.hpp"
#include "tUseRef_Y.hpp"

X::X(int y_val)
    : y(y_val) {
}

X::~X() = default;

Y& X::get_y() {
    return y;
}

Y const & X::get_y() const {
    return y;
}
