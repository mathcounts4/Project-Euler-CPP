#include "BadOptionalAccess.hpp"

BadOptionalAccess::BadOptionalAccess(std::string cause_str)
    : std::bad_optional_access()
    , cause(std::move(cause_str)) {
}

BadOptionalAccess::~BadOptionalAccess() _NOEXCEPT {
}

char const * BadOptionalAccess::what() const _NOEXCEPT {
    return cause.c_str();
}
