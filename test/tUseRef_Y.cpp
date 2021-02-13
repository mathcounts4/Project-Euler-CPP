#include "tUseRef_Y.hpp"

Y::Y(int v)
    : value(v) {
}

void Y::set_value(int new_value) {
    value = new_value;
}

int Y::get_value() const {
    return value;
}
