#include "UniqueOwnedReferenceForHPP.hpp"

#include <cstdint>
#include <optional>
#include <string_view>

struct PreciseRange {
  public:
    // Constructors
    PreciseRange(std::int64_t value);
    PreciseRange(std::string_view numericLiteral /* hex, binary, octal, or decimal, decimal point and negative sign allowed */);
    template<std::size_t n>
    PreciseRange(char const (&stringLiteral)[n])
	: PreciseRange(std::string_view(stringLiteral)) {}
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
    // Note: when evaluating the result of operator/, if, after refining y to high precision, we cannot prove that y ≠ 0, this throws std::domain_error.
    friend PreciseRange operator/(PreciseRange const& x, PreciseRange const& y);
    // Note: when evaluating the result of mod, if, after refining y to high precision, we cannot prove that y ≠ 0, this throws std::domain_error.
    // mod(x, y) returns a range contained in [-|y|, |y|], and refinement will always produce a sub-range of a previous range.
    // For example, mod(7, 5) may converge on 2 or -3.
    // Useful for evaluating periodic functions like sin and cos.
    friend PreciseRange mod(PreciseRange const& x, PreciseRange const& y);
    // x^exponent = power(x, exponent)
    friend PreciseRange operator^(PreciseRange const& x, std::int64_t exponent);
    friend PreciseRange operator<<(PreciseRange const& x, std::int64_t shift);
    friend PreciseRange operator>>(PreciseRange const& x, std::int64_t shift);

    // Binary operators that produce a usable result
    enum class Cmp { Less, ApproxEqual, Greater };
    // If x = y, returns ApproxEqual.
    // If |x-y| > 2^maxUncertaintyLog2, returns (x < y -> LESS, x > y -> Greater)
    // Else, returns EITHER ApproxEqual OR (x < y -> LESS, x > y -> Greater)
    friend Cmp cmp(PreciseRange const& x, PreciseRange const& y, std::int64_t maxUncertaintyLog2);
    // If x = y, returns true.
    // If |x-y| > 2^maxUncertaintyLog2, returns false
    // Otherwise may return true or false.
    friend bool eq(PreciseRange const& x, PreciseRange const& y, std::int64_t maxUncertaintyLog2);

    // Printing utilities

    // Prints an expression representing the exact value, like "√(3*π+4/-7)
    std::string toStringExact() const;
    
    // If maxUncertaintyLog2 is provided, produces a result whose uncertainty is ≤ 2^maxUncertaintyLog2.
    // Otherwise produces a result with uncertainty based on what the already completed computations have required, or an estimate if no computation has been performed.
    std::string toStringWithUncertaintyLog2AtMost(std::optional<std::int64_t> maxUncertaintyLog2) const;

  private:
    template<std::int64_t... Derivatives>
    friend struct PowerSeriesOp;
    struct Impl;
    friend struct Impl;
    UniqueOwnedReference<Impl> fImpl;
    PreciseRange(Impl const& implToCopy);
    Impl const& impl() const;
};
