#pragma once

#include "Class.hpp"
#include "DebugPrint.hpp"
#include "Math.hpp"
#include "Memory.hpp"
#include "TypeUtils.hpp"

#include <type_traits>

struct Fail_Construct{};

extern bool SafeMath_failFast;
// SafeMath<T>
template<class X> struct SafeMath {
    static_assert(is_pure<X>,"SafeMath can only be used with non-cvref types");
    enum State { Ok, Failed_Construction, Failed_Math };
    friend struct Class<SafeMath>;
    template<class Y> friend struct SafeMath;
  private:
    Lazy<X> lazy_x;
    State state;

    void fastFail() const;
    X& x();
  public:
    template<class... Args> SafeMath(Args&&... args);
    template<class Y, class = std::enable_if_t<is_same<SafeMath,No_cvref<Y>>>>
    SafeMath(Y&& y);
    SafeMath(SafeMath const&);
    SafeMath(Fail_Construct);
    ~SafeMath();
    
    [[nodiscard]] bool ok() const; // returns if the value is valid
    void check() const; // throws if the value is invalid
    [[nodiscard]] X const& value() const; // calls check() and returns the value
    [[nodiscard]] X const& operator*() const; // same as value()
    [[nodiscard]] operator X const&() const; // implicit conversion to value()

    static SafeMath failure();
    template<class Y> static SafeMath cast(Y const& y);
    template<class Y> static SafeMath cast(SafeMath<Y> const& y);
    
    template<class Y> SafeMath<bool> operator<(Y const& y) const;
    template<class Y> SafeMath<bool> operator>(Y const& y) const;
    template<class Y> SafeMath<bool> operator<=(Y const& y) const;
    template<class Y> SafeMath<bool> operator>=(Y const& y) const;
    template<class Y> SafeMath<bool> operator==(Y const& y) const;
    template<class Y> SafeMath<bool> operator!=(Y const& y) const;
    
    template<class Y> SafeMath<bool> operator<(SafeMath<Y> const& y) const;
    template<class Y> SafeMath<bool> operator>(SafeMath<Y> const& y) const;
    template<class Y> SafeMath<bool> operator<=(SafeMath<Y> const& y) const;
    template<class Y> SafeMath<bool> operator>=(SafeMath<Y> const& y) const;
    template<class Y> SafeMath<bool> operator==(SafeMath<Y> const& y) const;
    template<class Y> SafeMath<bool> operator!=(SafeMath<Y> const& y) const;
    
    template<class Y> SafeMath& operator+=(Y const& y);
    template<class Y> SafeMath& operator-=(Y const& y);
    template<class Y> SafeMath& operator*=(Y const& y);
    template<class Y> SafeMath& operator/=(Y const& y);
    template<class Y> SafeMath& operator%=(Y const& y);
    template<class Y> SafeMath& operator<<=(Y const& y);
    template<class Y> SafeMath& operator>>=(Y const& y);
    
    template<class Y> SafeMath& operator+=(SafeMath<Y> const& y);
    template<class Y> SafeMath& operator-=(SafeMath<Y> const& y);
    template<class Y> SafeMath& operator*=(SafeMath<Y> const& y);
    template<class Y> SafeMath& operator/=(SafeMath<Y> const& y);
    template<class Y> SafeMath& operator%=(SafeMath<Y> const& y);
    template<class Y> SafeMath& operator<<=(SafeMath<Y> const& y);
    template<class Y> SafeMath& operator>>=(SafeMath<Y> const& y);
    
    template<class Y> [[nodiscard]] auto operator+(Y const& y) const;
    template<class Y> [[nodiscard]] auto operator-(Y const& y) const;
    template<class Y> [[nodiscard]] auto operator*(Y const& y) const;
    template<class Y> [[nodiscard]] auto operator/(Y const& y) const;
    template<class Y> [[nodiscard]] auto operator%(Y const& y) const;
    template<class Y> [[nodiscard]] auto operator<<(Y const& y) const;
    template<class Y> [[nodiscard]] auto operator>>(Y const& y) const;
};
template<class X> SafeMath(X&&) -> SafeMath<No_cvref<X>>;




// template impl

template<class X>
X& SafeMath<X>::x() {
    return lazy_x;
}

template<class X>
void SafeMath<X>::fastFail() const {
    if (SafeMath_failFast)
	check();
}

template<class X> template<class... Args>
SafeMath<X>::SafeMath(Args&&... args)
    : state(Ok) {
    try {
	lazy_x.construct(fwd<Args>(args)...);
    } catch (...) {
	state = Failed_Construction;
	fastFail();
    }
}

template<class X> template<class Y, class>
SafeMath<X>::SafeMath(Y&& y)
    : state(y.state) {
    fastFail();
    if (y.ok()) {
	try {
	    lazy_x.construct(y.value());
	} catch (...) {
	    state = Failed_Construction;
	    fastFail();
	}
    }
}

template<class X>
SafeMath<X>::SafeMath(SafeMath const& y)
    : state(y.state) {
    fastFail();
    if (y.ok()) {
	try {
	    lazy_x.construct(y.value());
	} catch (...) {
	    state = Failed_Construction;
	    fastFail();
	}
    }
}
    

template<class X>
SafeMath<X>::SafeMath(Fail_Construct)
    : state(Failed_Construction) {
    fastFail();
}

template<class X>
SafeMath<X>::~SafeMath() {
    if (state != Failed_Construction)
	lazy_x.destroy();
}

template<class X>
[[nodiscard]] B SafeMath<X>::ok() const {
    return state == Ok;
}

template<class X>
void SafeMath<X>::check() const {
    switch (state) {
      case Ok:
	return;
      case Failed_Construction:
	throw_exception<std::invalid_argument>("Construction failed in SafeMath");
      case Failed_Math:
	throw_exception<std::invalid_argument>("Math error detected in SafeMath");
    }
}

template<class X>
X const& SafeMath<X>::value() const {
    check();
    return lazy_x;
}

template<class X>
X const& SafeMath<X>::operator*() const {
    return value();
}

template<class X>
SafeMath<X>::operator X const&() const {
    return value();
}

template<class X> template<class Y>
SafeMath<X> SafeMath<X>::cast(Y const& y) {
    SafeMath result(X(0));
    result += y;
    return result;
}

template<class X> template<class Y>
SafeMath<X> SafeMath<X>::cast(SafeMath<Y> const& y) {
    if (!y.ok())
	return Fail_Construct();
    return cast(*y);
}

template<class X> template<class Y>
SafeMath<B> SafeMath<X>::operator<(SafeMath<Y> const& y) const {
    if (!ok() || !y.ok()) {
	return Fail_Construct();
    }
    B const x_neg = value() < X(0);
    B const y_neg = y.value() < Y(0);
    if (x_neg != y_neg) {
	return x_neg;
    }
    auto const abs_x = my_abs(value());
    auto const abs_y = my_abs(y.value());
    if (x_neg) {
	auto ret = abs_y < abs_x;
	// If we don't debug print here, -O3 messes up SafeMath<__int128>(-1) < SafeMath<__int128>(intmin<__int128>())
	debug::cout << __FUNCTION__ << ": " << abs_x << " " << abs_y << std::endl;
	return ret;
    }
    return abs_x < abs_y;
}

template<class X> template<class Y>
SafeMath<B> SafeMath<X>::operator>(SafeMath<Y> const& y) const {
    return y < *this;
}

template<class X> template<class Y>
SafeMath<B> SafeMath<X>::operator<=(SafeMath<Y> const& y) const {
    auto result = *this > y;
    if (result.ok()) {
	result.x() = !result.x();
    }
    return result;
}

template<class X> template<class Y>
SafeMath<B> SafeMath<X>::operator>=(SafeMath<Y> const& y) const {
    return y <= *this;
}

template<class X> template<class Y>
SafeMath<B> SafeMath<X>::operator==(SafeMath<Y> const& y) const {
    if (!ok() || !y.ok())
	return Fail_Construct();
    B const x_neg = value() < X(0);
    B const y_neg = y.value() < Y(0);
    if (x_neg != y_neg)
	return false;
    return my_abs(value()) == my_abs(y.value());
}

template<class X> template<class Y>
SafeMath<B> SafeMath<X>::operator!=(SafeMath<Y> const& y) const {
    auto result = *this == y;
    if (result.ok())
	result.x() = !result.x();
    return result;
}

template<class X> template<class Y>
SafeMath<B> SafeMath<X>::operator<(Y const& y) const {
    return *this < SafeMath<Y>(y);
}

template<class X> template<class Y>
SafeMath<B> SafeMath<X>::operator>(Y const& y) const {
    return *this > SafeMath<Y>(y);
}

template<class X> template<class Y>
SafeMath<B> SafeMath<X>::operator<=(Y const& y) const {
    return *this <= SafeMath<Y>(y);
}

template<class X> template<class Y>
SafeMath<B> SafeMath<X>::operator>=(Y const& y) const {
    return *this >= SafeMath<Y>(y);
}

template<class X> template<class Y>
SafeMath<B> SafeMath<X>::operator==(Y const& y) const {
    return *this == SafeMath<Y>(y);
}

template<class X> template<class Y>
SafeMath<B> SafeMath<X>::operator!=(Y const& y) const {
    return *this != SafeMath<Y>(y);
}

template<class X> template<class Y>
SafeMath<X>& SafeMath<X>::operator+=(SafeMath<Y> const& yS) {
    if (!ok() || !yS.ok()) {
	state = Failed_Math;
	fastFail();
	return *this;
    }
    auto y = yS.value();
    // x_min <= x+y <= x_max
    bool const x_neg = value() < X(0);
    bool const y_neg = y < Y(0);
    if (y_neg) { // only need x_min <= x+y
	X const x_min = std::numeric_limits<X>::lowest();
	if (x_neg) { // x<0 -> x_min-x is valid
	    // you may laugh, but char-char = int
	    if (SafeMath<X>(static_cast<X>(x_min-x())) > y) {
		state = Failed_Math;
		fastFail();
	    }
	} else { // x_min<=0<=x so can only fail if x+y<0
	    if (my_abs(x())<my_abs(y)) // x<-y (x+y<0)
	        if (SafeMath<X>(x_min) > y+static_cast<Y>(x())) { // y<=y+Y(x)<=0 so y+Y(x) can be computed safely in Y
		    state = Failed_Math;
		    fastFail();
		}
	}
    } else { // only need x+y <= x_max
	X const x_max = std::numeric_limits<X>::max();
	if (x_neg) { // x<0<=x_max so can only fail if x_max<y
	    if (SafeMath<X>(x_max) < y) // x_max<y so y-x_max is valid
	        if (my_abs(y-static_cast<Y>(x_max)) > my_abs(x())) { // abs(x) = -x
		    state = Failed_Math;
		    fastFail();
		}
	} else { // x>=0 -> x_max-x is valid
	    // you may laugh, but char-char = int
	    if (SafeMath<X>(static_cast<X>(x_max-x())) < y) {
		state = Failed_Math;
		fastFail();
	    }
	}
    }
    if (ok()) {
	x() += static_cast<X>(y);
    }
    return *this;
}

template<class X> template<class Y>
SafeMath<X>& SafeMath<X>::operator-=(SafeMath<Y> const& yS) {
    // x_min <= x-y <= x_max
    if (!ok() || !yS.ok()) {
	state = Failed_Math;
	fastFail();
	return *this;
    }
    auto y = yS.value();
    bool const x_neg = x() < X(0);
    bool const y_neg = y < Y(0);
    if (y_neg) {
	return *this += my_abs(y);
    } else { // only need x_min <= x-y
	X const x_min = std::numeric_limits<X>::lowest();
	using X_NN = decltype(my_abs(x()));
	if (x_neg) {
	    // you may laugh, but char-char = int
	    if (SafeMath<X_NN>(my_abs(static_cast<X>(x_min-x()))) < y) {
		state = Failed_Math;
		fastFail();
	    }
	} else {
	    if (SafeMath<X_NN>(my_abs(x_min)) < y)
		if (SafeMath<Y>(static_cast<Y>(y-static_cast<Y>(my_abs(x_min)))) > x()) {
		    state = Failed_Math;
		    fastFail();
		}
	}
    }
    if (ok())
	x() -= static_cast<X>(y);
    return *this;
}

template<class X>
static inline bool equals(X const& x, X const& y) {
    // implemented so clang doesn't complain about floating-point equality comparison
    return x <= y && y <= x;
}

template<class X> template<class Y>
SafeMath<X>& SafeMath<X>::operator*=(SafeMath<Y> const& yS) {
    if (!ok() || !yS.ok()) {
	state = Failed_Math;
	fastFail();
	return *this;
    }
    auto y = yS.value();
    if (!equals(x(),X(0)) && !equals(y,Y(0))) {
	if (equals(x(),X(-1)) && X(-1) < X(0)) {
	    // special case for -1 because, with some type combinations,
	    // -1 * lowest = lowest
	    // we need x_min <= -y <= x_max
	    if (!(SafeMath<X>(static_cast<X>(0)) -= y).ok()) {
		state = Failed_Math;
		fastFail();
	    }
	} else {
	    bool const x_neg = x() < X(0);
	    bool const y_neg = y < Y(0);
	    X const comp = x_neg == y_neg ? std::numeric_limits<X>::max()/x() : std::numeric_limits<X>::lowest()/x();
	    SafeMath<Y> y_safe(y);
	    if (*(y_neg ? y_safe < comp : y_safe > comp)) {
		state = Failed_Math;
		fastFail();
	    }
	}
    }
    if (ok())
	x() *= static_cast<X>(y);
    return *this;
}

template<class X> template<class Y>
SafeMath<X>& SafeMath<X>::operator/=(SafeMath<Y> const& yS) {
    if (!ok() || !yS.ok()) {
	state = Failed_Math;
	fastFail();
	return *this;
    }
    auto y = yS.value();
    if (y == Y(0)) {
	state = Failed_Math;
	fastFail();
    } else {
	bool const is_neg = (x() < X(0)) ^ (y < Y(0));
	auto abs_result = my_abs(x()) / my_abs(y);
	if (is_neg) {
	    if (my_abs(std::numeric_limits<X>::lowest()) >= abs_result) {
		x() = -X(abs_result);
	    } else {
		state = Failed_Math;
		fastFail();
	    }	
	} else {
	    if (std::numeric_limits<X>::max() >= abs_result) {
		x() = X(abs_result);
	    } else {
		state = Failed_Math;
		fastFail();
	    }
	}
    }
    return *this;
}

#define SafeMath_OP_DECL(OP)					\
    template<class X> template<class Y>				\
    SafeMath<X>& SafeMath<X>::operator OP##=(Y const& y) {	\
	return *this OP##= SafeMath<Y>(y);			\
    }								\
	template<class X> template<class Y> [[nodiscard]]	\
	auto SafeMath<X>::operator OP(Y const& y) const {	\
	    using Z = decltype(**this OP y);			\
	    SafeMath<Z> z = SafeMath<Z>::cast(*this);		\
	    z OP##= y;						\
	    return z;						\
	}
SafeMath_OP_DECL(+);
SafeMath_OP_DECL(-);
SafeMath_OP_DECL(*);
SafeMath_OP_DECL(/);
SafeMath_OP_DECL(%);
SafeMath_OP_DECL(<<);
SafeMath_OP_DECL(>>);

template<class X> struct Class<SafeMath<X> > {
    using T = SafeMath<X>;
    static std::ostream& print(std::ostream& os, T const& t) {
	switch (t.state) {
	  case T::Ok:
	    return os << t.value();
	  case T::Failed_Construction:
	    return os << "Failed_to_Construct<" << Class<X>::name() << ">";
	  case T::Failed_Math:
	    return os << "Failed_Math<" << Class<X>::name() << ">";
	}
    }
    static Optional<T> parse(std::istream& is) {
	auto result = Class<X>::parse(is);
	if (!result)
	    return Failure(result.cause());
	return *result;
    }
    static std::string name() {
	return "SafeMath<" + Class<X>::name() + ">";
    }
    static std::string format() {
	return Class<X>::format();
    }
};


namespace std {
    template<class T> struct make_unsigned<SafeMath<T>> { using type = SafeMath<Make_u<T>>; };
}
