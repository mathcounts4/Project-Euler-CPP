#pragma once

#include "TypeUtils.hpp"

#include <vector>

template<class... T> using Vec = std::vector<T...>;

// The following is not valid, since initializer_list yields const refs, which can't be moved from.
//std::vector<std::unique_ptr<int>> vui{std::make_unique<int>(7)};
// See the following page for a solution:
// https://tristanbrindle.com/posts/beware-copies-initializer-list

