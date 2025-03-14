#include "UniqueOwnedReferenceForHPP.hpp"

#include <cstdint>
#include <optional>
#include <string_view>

struct PreciseRange {
  public:
    // Constructors
    PreciseRange(std::int64_t value);
    PreciseRange(std::string_view numericLiteral /* hex, binary, octal, or decimal, decimal point and negative sign allowed */);
    PreciseRange(PreciseRange const&);
    ~PreciseRange();
    PreciseRange& operator=(PreciseRange const&);

    // Constants
    static PreciseRange pi();

    // Unary operators that produce another PreciseRange
    // Note: output must be continuous in input in order to maintain convergence
    PreciseRange operator-() const;
    friend PreciseRange sqrt(PreciseRange const& x);
    friend PreciseRange distanceToNearestInteger(PreciseRange const& x);
    friend PreciseRange exp(PreciseRange const& x);
    friend PreciseRange sin(PreciseRange const& x);
    friend PreciseRange cos(PreciseRange const& x);
    friend PreciseRange sinh(PreciseRange const& x);
    friend PreciseRange cosh(PreciseRange const& x);

    // Binary operators that produce another PreciseRange
    // Note: output must be continuous in inputs in order to maintain convergence
    friend PreciseRange operator+(PreciseRange const& x, PreciseRange const& y);
    friend PreciseRange operator-(PreciseRange const& x, PreciseRange const& y);
    friend PreciseRange operator*(PreciseRange const& x, PreciseRange const& y);
    friend PreciseRange operator/(PreciseRange const& x, PreciseRange const& y);

    // Binary operators that produce a usable result
    // Note: if input values are equal, may run forever or throw an exception
    friend bool operator<(PreciseRange const& x, PreciseRange const& y);
    friend bool operator>(PreciseRange const& x, PreciseRange const& y);

    // Printing utilities

    // Prints an expression representing the exact value, like "√(3 * π + 4 / -7)
    std::string toStringExact() const;
    
    // If maxUncertaintyLog2 is provided, produces a result whose uncertainty is ≤ 2^maxUncertaintyLog2.
    // Otherwise produces a result with uncertainty based on what the already completed computations have required.
    std::string toStringWithUncertaintyLog2AtMost(std::optional<std::int64_t> maxUncertaintyLog2) const;

  private:
    struct Impl;
    friend struct Impl;
    UniqueOwnedReference<Impl> fImpl;
    PreciseRange(Impl const& implToCopy);
    Impl const& impl() const;
};
