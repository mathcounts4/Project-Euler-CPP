#include "BadOptionalAccess.hpp"

BadOptionalAccess::BadOptionalAccess(std::string cause_str)
    : std::bad_optional_access()
    , cause(std::move(cause_str)) {
}

BadOptionalAccess::~BadOptionalAccess() noexcept {
}

char const * BadOptionalAccess::what() const noexcept {
    return cause.c_str();
}
