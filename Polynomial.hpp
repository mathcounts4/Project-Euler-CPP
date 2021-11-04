#pragma once

#include "Class.hpp"
#include "Cleanup.hpp"
#include "ExitUtils.hpp"
#include "MyHashUtil.hpp"

#include <vector>

template<class Coefficient> class Polynomial {
    friend struct Class<Polynomial>;
    friend class std::hash<Polynomial>;
    
    std::vector<Coefficient> coefficients;

    void reduce();
    
  public:
    Polynomial();
    Polynomial(Coefficient in);
    Polynomial(std::vector<Coefficient> in);
    void swap(Polynomial& o);
    std::size_t degree_plus_one() const;
    Coefficient const& operator[](std::size_t i) const;
    explicit operator bool() const;

    template<class X> auto eval(X const& x) const;

    static Polynomial term(std::size_t i);

    Polynomial& operator+=(Polynomial const& o);
    Polynomial operator+(Polynomial const& o) const&;
    Polynomial operator+(Polynomial const& o) &&;
    
    Polynomial& operator-=(Polynomial const& o);
    Polynomial operator-(Polynomial const& o) const&;
    Polynomial operator-(Polynomial const& o) &&;
    
    Polynomial& operator*=(Polynomial const& o);
    Polynomial operator*(Polynomial const& o) const;

    Polynomial& operator/=(Coefficient const& d);
    Polynomial operator/(Coefficient const& d) const;

    Polynomial& operator^=(std::size_t exp);
    Polynomial operator^(std::size_t exp) const&;
    Polynomial operator^(std::size_t exp) &&;
};
MAKE_HASHABLE_TEMPLATE(<class Coefficient>, Polynomial<Coefficient>, p, p.coefficients);

template<class Coefficient> struct Class<Polynomial<Coefficient> > {
    using T = Polynomial<Coefficient>;
    
    static std::ostream& print(std::ostream& os, T const& t);
    static Optional<T> parse(std::istream& is);
    static std::string name();
    static std::string format();
};



/* template Polynomial impl */

template<class Coefficient>
void Polynomial<Coefficient>::reduce() {
    while (coefficients.size() && !coefficients.back())
	coefficients.pop_back();
}

template<class Coefficient>
Polynomial<Coefficient>::Polynomial() {
}

template<class Coefficient>
Polynomial<Coefficient>::Polynomial(Coefficient in)
    : coefficients{std::move(in)} {
    reduce();
}

template<class Coefficient>
Polynomial<Coefficient>::Polynomial(std::vector<Coefficient> in)
    : coefficients(std::move(in)) {
    reduce();
}

template<class Coefficient>
void Polynomial<Coefficient>::swap(Polynomial& o) {
    std::swap(coefficients,o.coefficients);
}

template<class Coefficient>
std::size_t Polynomial<Coefficient>::degree_plus_one() const {
    return coefficients.size();
}

template<class Coefficient>
Coefficient const& Polynomial<Coefficient>::operator[](std::size_t i) const {
    if (i >= degree_plus_one())
	throw_exception<std::out_of_range>(to_string(i) + " too large for " + to_string(*this) + " - check degree_plus_one()");
    return coefficients[i];
}

template<class Coefficient>
Polynomial<Coefficient>::operator bool() const {
    return degree_plus_one() > 0;
}

template<class Coefficient>
template<class X>
auto Polynomial<Coefficient>::eval(X const& x) const {
    using Result = decltype(declval<Coefficient>() * declval<X>());

    Result result{};
    if (!x || degree_plus_one() < 2) {
	if (degree_plus_one())
	    result = (*this)[0];
    } else {
	X mult(1);
	for (auto const& c : coefficients) {
	    result += c * mult;
	    mult *= x;
	}
    }
    return result;
}

template<class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::term(std::size_t i) {
    Polynomial result;
    result.coefficients.resize(i+1);
    result.coefficients[i] = Coefficient(1);
    return result;
}

template<class Coefficient>
Polynomial<Coefficient>& Polynomial<Coefficient>::operator+=(Polynomial const& o) {
    std::size_t const n = o.coefficients.size();
    if (coefficients.size() < n)
	coefficients.resize(n);
    for (std::size_t i = 0; i < n; ++i)
	coefficients[i] += o.coefficients[i];
    reduce();
    return *this;
}

template<class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::operator+(Polynomial const& o) const& {
    Polynomial copy(*this);
    copy += o;
    return copy;
}

template<class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::operator+(Polynomial const& o) && {
    (*this) += o;
    return std::move(*this);
}

template<class Coefficient, class Coercible, class = decltype(Coefficient(declval<Coercible>()))>
Polynomial<Coefficient> operator+(Coercible x, Polynomial<Coefficient> const& y) {
    return Polynomial<Coefficient>(Coefficient(std::move(x))) + y;
}

template<class Coefficient>
Polynomial<Coefficient>& Polynomial<Coefficient>::operator-=(Polynomial const& o) {
    std::size_t const n = o.coefficients.size();
    if (coefficients.size() < n)
	coefficients.resize(n);
    for (std::size_t i = 0; i < n; ++i)
	coefficients[i] -= o.coefficients[i];
    reduce();
    return *this;
}

template<class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::operator-(Polynomial const& o) const& {
    Polynomial copy(*this);
    copy -= o;
    return copy;
}

template<class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::operator-(Polynomial const& o) && {
    (*this) -= o;
    return std::move(*this);
}

template<class Coefficient, class Coercible, class = decltype(Coefficient(declval<Coercible>()))>
Polynomial<Coefficient> operator-(Coercible x, Polynomial<Coefficient> const& y) {
    return Polynomial<Coefficient>(Coefficient(std::move(x))) - y;
}

template<class Coefficient>
Polynomial<Coefficient>& Polynomial<Coefficient>::operator*=(Polynomial const& o) {
    return *this = *this * o;
}

template<class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::operator*(Polynomial const& o) const {
    Polynomial result;
    if (!*this || !o)
	return result;
    std::size_t const n = coefficients.size();
    std::size_t const o_n = o.coefficients.size();
    result.coefficients.resize(n+o_n-1);
    for (std::size_t i = 0; i < n; ++i)
	for (std::size_t j = 0; j < o_n; ++j)
	    result.coefficients[i+j] += (*this)[i] * o[j];
    result.reduce();
    return result;
}

template<class Coefficient, class Coercible, class = decltype(Coefficient(declval<Coercible>()))>
Polynomial<Coefficient> operator*(Coercible x, Polynomial<Coefficient> const& y) {
    return Polynomial<Coefficient>(Coefficient(std::move(x))) * y;
}

template<class Coefficient>
Polynomial<Coefficient>& Polynomial<Coefficient>::operator/=(Coefficient const& d) {
    if (!d)
	throw_exception<std::domain_error>("Division by zero " + class_name<Polynomial<Coefficient> >());
    for (auto& c : coefficients)
	c /= d;
    return *this;
}

template<class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::operator/(Coefficient const& d) const {
    Polynomial copy(*this);
    copy /= d;
    return copy;
}

template<class Coefficient>
Polynomial<Coefficient>& Polynomial<Coefficient>::operator^=(std::size_t exp) {
    return *this = *this ^ exp;
}

template<class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::operator^(std::size_t exp) const& {
    Polynomial result(Coefficient(1));
    for (Polynomial mult(*this); exp; ) {
	if (exp & 1)
	    result *= mult;
	exp >>= 1;
	if (exp)
	    mult *= mult;
    }
    return result;
}

template<class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::operator^(std::size_t exp) && {
    Polynomial result(Coefficient(1));
    for ( ; exp; ) {
	if (exp & 1)
	    result *= *this;
	exp >>= 1;
	if (exp)
	    *this *= *this;
    }
    return result;
}

template<class Coefficient, class Coercible, class = decltype(Coefficient(declval<Coercible>()))>
Polynomial<Coefficient> operator^(Coercible x, Polynomial<Coefficient> const& y) {
    return Polynomial<Coefficient>(Coefficient(std::move(x))) ^ y;
}



/* template Class<Polynomial> impl */

template<class Coefficient>
std::ostream& Class<Polynomial<Coefficient> >::print(std::ostream& os, Polynomial<Coefficient> const& p) {
    os << '(';
    Cleanup closeParen{[&os](){os<<')';}};
    os << "[x] -> ";
    bool started = false;
    Coefficient const zero{};
    Coefficient const one(1);
    
    Cleanup appendZero{[&os,&started,&zero](){if (!started) os<<zero;}};
    
    for (std::size_t i = p.coefficients.size(); i--; ) {
	auto&& c = p.coefficients[i];
	if (c) {
	    if (started)
		os << "+";
	    if (c != one || i == 0)
		os << c;
	    if (i > 0) {
		os << 'x';
		if (i > 1)
		    os << "^" << i;
	    }
	    started = true;
	}
    }
    return os;
}

template<class Coefficient>
Optional<Polynomial<Coefficient> > Class<Polynomial<Coefficient> >::parse(std::istream& is) {
    Resetter resetter{is};
    auto failure = [](std::string const& cause) {
	return Failure("Parsing " + name() + " failed: " + cause);
    };
    bool const inParens = consume(is,'(');

    std::vector<Coefficient> coefficients;

    Optional<Coefficient> c(Failure("Nothing parsed yet"));
    while (true) {
	bool minus = false;
	if (!consume(is,'+'))
	    minus = consume(is,'-');
	c = Class<Coefficient>::parse(is);
	std::size_t i = 0;
	bool const x = consume(is,'x');
	if (!c && !x)
	    break;
	if (x) {
	    i = 1;
	    if (consume(is,'^')) {
		auto exp = Class<std::size_t>::parse(is);
		if (!exp)
		    return failure("Invalid exponent");
		i = *exp;
	    }
	}
	if (i >= coefficients.size())
	    coefficients.resize(i+1);
	if (c) {
	    if (minus) {
		coefficients[i] -= std::move(*c);
	    } else {
		coefficients[i] += std::move(*c);
	    }
	} else {
	    if (minus) {
		coefficients[i] -= Coefficient(1);
	    } else {
		coefficients[i] += Coefficient(1);
	    }
	}
    }

    if (coefficients.size() == 0)
	return failure("No valid terms found: " + c.cause());

    if (inParens && !consume(is,')'))
	return failure("No matching ')' found");

    resetter.ignore();
    return Polynomial<Coefficient>(std::move(coefficients));
}

template<class Coefficient>
std::string Class<Polynomial<Coefficient> >::name() {
    return "Polynomial<" + Class<Coefficient>::name() + ">";
}

template<class Coefficient>
std::string Class<Polynomial<Coefficient> >::format() {
    return "(" + Class<Coefficient>::format() + " x{^i} + ...)";
}

