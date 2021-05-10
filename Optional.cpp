#include "Optional.hpp"

#include "BadOptionalAccess.hpp"
#include "ExitUtils.hpp"
#include "Failure.hpp"

#include <utility>

Optional<void>::Optional()
    : value{} {
}

Optional<void>::Optional(Failure f)
    : value{std::in_place, std::move(f.reason)} {
}

Optional<void>::operator bool() const {
    return !value;
}

std::string const & Optional<void>::cause() const {
    if (*this) {
	throw_exception<BadOptionalAccess>("cause() called on a valid Optional");
    }
    return *value;
}

void Optional<void>::operator*() const {
    if (!*this) {
	throw_exception<BadOptionalAccess>(cause());
    }
}

void Optional<void>::deref_without_stack() const {
    if (!*this) {
	throw_exception_no_stack<BadOptionalAccess>(cause());
    }
}

