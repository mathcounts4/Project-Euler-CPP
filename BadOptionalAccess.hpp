#pragma once

#include <optional>
#include <string>

struct BadOptionalAccess
    : public std::bad_optional_access {
  private:
    std::string cause;
  public:
    explicit BadOptionalAccess(std::string cause);
    ~BadOptionalAccess() noexcept override;
    char const * what() const noexcept override;
};

