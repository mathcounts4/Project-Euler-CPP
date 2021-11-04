#include "harness.hpp"

#include "tUseRef_X.hpp"
#include "tUseRef_Y.hpp"

#include <utility>

TEST(UniqueOwnedReference,ConstructGetSet) {
    X x(5);
    X const& const_x_ref = x;
    CHECK(const_x_ref.get_y().get_value(), equals(5));
    x.get_y().set_value(7);
    CHECK(const_x_ref.get_y().get_value(), equals(7));
}
