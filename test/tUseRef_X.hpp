#pragma once

#include "../UniqueOwnedReferenceForHPP.hpp"
#include "tUseRef_Y_fwd.hpp"

struct X {
  private:
    UniqueOwnedReference<Y> y;
  public:
    X(int y_val);
    ~X();
    Y& get_y();
    Y const & get_y() const;
};
