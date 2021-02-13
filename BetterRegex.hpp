#pragma once

#include "TypeUtils.hpp"

#include <memory>
#include <regex>
#include <sstream>
#include <utility>

namespace regex {

    /* Quantities */
    struct AtLeast {
      public:
	AtLeast(UI count)
	    : fCount(count) {
	}
	AtLeast(SI count)
	    : fCount(static_cast<UI>(std::max(0,count))) {
	}
	void print(std::ostream& os) const {
	    os << '{' << fCount << ",}";
	}
      private:
	UI fCount;
    };
    struct Between {
      public:
	Between(UI x, UI y)
	    : fLow(std::min(x,y))
	    , fHigh(std::max(x,y)) {
	}
	Between(SI x, SI y)
	    : fLow(static_cast<UI>(std::max(0,std::min(x,y))))
	    , fHigh(static_cast<UI>(std::max({0,x,y}))) {
	}
	void print(std::ostream& os) const {
	    os << '{' << fLow << "," << fHigh << '}';
	}
      private:
	UI fLow;
	UI fHigh;
    };
    struct Exactly {
      public:
	Exactly(UI count)
	    : fCount(count) {
	}
	Exactly(SI count)
	    : fCount(static_cast<UI>(std::max(0,count))) {
	}
	void print(std::ostream& os) const {
	    os << '{' << fCount << '}';
	}
      private:
	UI fCount;
    };
    struct Optional {
	void print(std::ostream& os) const {
	    os << '?';
	}
    };
    extern Optional optional;

    template<class T> constexpr B is_quantity = any_same<T,AtLeast,Between,Exactly,Optional,UI>;

    

    template<class RegexType> struct is_regex_S {
	static constexpr B value = false;
    };
    template<class RegexType> constexpr B is_regex = is_regex_S<RegexType>::value;
    template<class RegexType> constexpr B is_cvref_regex = is_regex<No_cvref<RegexType> >;
    template<class... RegexTypes> constexpr B are_regex = (true && ... && is_regex<RegexTypes>);
    template<class... RegexTypes> constexpr B are_cvref_regex = (true && ... && is_cvref_regex<RegexTypes>);
    
    template<class RegexType> struct Regex {
	// T : Regex<T>
      public:
	operator std::regex() const {
	    std::ostringstream oss;
	    printRaw(oss);
	    return std::regex{oss.str()};
	}
	void printEnclosed(std::ostream& os) const {
	    os << "(?:";
	    printRaw(os);
	    os << ')';
	}
	auto asGroup(std::string group) const &;
	auto asGroup(std::string group) &&;
      private:
	void printRaw(std::ostream& os) const {
	    base().print(os);
	}
	RegexType const & base() const & {
	    static_assert(std::is_base_of_v<Regex,RegexType>,"Regex<T> should be a base of T");
	    return static_cast<RegexType const &>(*this);
	}
	RegexType && base() && {
	    static_assert(std::is_base_of_v<Regex,RegexType>,"Regex<T> should be a base of T");
	    return static_cast<RegexType &&>(std::move(*this));
	}
    };

    template<class RegexType>
    struct GroupRegex : Regex<GroupRegex<RegexType> > {
	static_assert(is_regex<GroupRegex>,"GroupRegex should only have a regex argument");
	using Regex<GroupRegex>::operator std::regex;
	RegexType fRegex;
	std::string fGroup;
	void print(std::ostream& os) const {
	    os << '(';
	    fRegex.print(os);
	    os << ')';
	}
    };
    template<class RegexType>
    struct is_regex_S<GroupRegex<RegexType> > {
	static constexpr B value = true;
    };

    template<class RegexType> auto Regex<RegexType>::asGroup(std::string group) const & {
	return GroupRegex<RegexType>{base(),std::move(group)};
    }

    template<class RegexType> auto Regex<RegexType>::asGroup(std::string group) && {
	return GroupRegex<RegexType>{std::move(*this).base(),std::move(group)};
    }

    template<class X, class Y>
    struct OrRegex : Regex<OrRegex<X,Y> > {
	static_assert(are_regex<X,Y>,"OrRegex should only have regex arguments");
	using Regex<OrRegex>::operator std::regex;
	X fLhs;
	Y fRhs;
	void print(std::ostream& os) const {
	    fLhs.print(os);
	    os << '|';
	    fRhs.print(os);
	}
    };
    template<class X, class Y>
    struct is_regex_S<OrRegex<X,Y> > {
	static constexpr B value = true;
    };
    template<class X, class Y, class = verify<are_cvref_regex<X,Y> > >
    OrRegex<No_cvref<X>,No_cvref<Y> > operator|(X&& x, Y&& y) {
	return {fwd<X>(x),fwd<Y>(y)};
    }

    template<class X, class Y>
    struct CombinedRegex : Regex<CombinedRegex<X,Y> > {
	static_assert(are_regex<X,Y>,"CombinedRegex should only have regex arguments");
	using Regex<CombinedRegex>::operator std::regex;
	X fLhs;
	Y fRhs;
	void print(std::ostream& os) const {
	    fLhs.printEnclosed(os);
	    fRhs.printEnclosed(os);
	}
    };
    template<class X, class Y>
    struct is_regex_S<CombinedRegex<X,Y> > {
	static constexpr B value = true;
    };
    template<class X, class Y, class = verify<are_cvref_regex<X,Y> > >
    CombinedRegex<No_cvref<X>,No_cvref<Y> > operator+(X&& x, Y&& y) {
	return {fwd<X>(x),fwd<Y>(y)};
    }

    template<class Quantity, class RegexType>
    struct MultipleRegex : Regex<MultipleRegex<Quantity,RegexType> > {
      public:
	static_assert(is_quantity<Quantity> && is_regex<RegexType>,"CombinedRegex should only have regex arguments");
	using Regex<MultipleRegex>::operator std::regex;
	MultipleRegex(Quantity quantity, RegexType regex)
	    : fQuantity(std::move(quantity))
	    , fRegex(std::move(regex)) {
	}
	void print(std::ostream& os) const {
	    fRegex.printEnclosed(os);
	    fQuantity.print(os);
	}
      private:
	Quantity fQuantity;
	RegexType fRegex;
    };
    template<class Quantity, class Regex>
    struct is_regex_S<MultipleRegex<Quantity,Regex> > {
	static constexpr B value = true;
    };
    template<class Q, class R> constexpr B is_cvref_Q_R =
	is_quantity<No_cvref<Q> > && is_cvref_regex<R>;
    template<class Q, class R, class = verify<is_cvref_Q_R<Q,R> > >
    MultipleRegex<No_cvref<Q>,No_cvref<R> > operator*(Q&& quantity, R&& regex) {
	return {fwd<Q>(quantity),fwd<R>(regex)};
    }
    template<class Q, class R, class = verify<is_cvref_Q_R<Q,R> > >
    MultipleRegex<No_cvref<Q>,No_cvref<R> > operator*(R&& regex, Q&& quantity) {
	return {fwd<Q>(quantity),fwd<R>(regex)};
    }
    template<class Regex, class = verify<is_cvref_regex<Regex> > >
    MultipleRegex<Exactly,No_cvref<Regex> > operator*(UI quantity, Regex&& regex) {
	return {quantity,fwd<Regex>(regex)};
    }
    template<class Regex, class = verify<is_cvref_regex<Regex> > >
    MultipleRegex<Exactly,No_cvref<Regex> > operator*(Regex&& regex, UI quantity) {
	return {quantity,fwd<Regex>(regex)};
    }

    /* Literals */
    struct LiteralRegex : Regex<LiteralRegex> {
	using Regex<LiteralRegex>::operator std::regex;
      public:
	LiteralRegex(std::string literal)
	    : fLiteral(std::move(literal)) {
	}
	void print(std::ostream& os) const;
      private:
	std::string fLiteral;
    };
    template<>
    struct is_regex_S<LiteralRegex> {
	static constexpr B value = true;
    };
    
    namespace literals {
	inline LiteralRegex operator""_verbatum(char const * str, std::size_t const n) {
	    return {{str,n}};
	}
    } /* namespace literals */
    
    namespace short_literals {
	inline LiteralRegex operator""_v(char const * str, std::size_t const n) {
	    return {{str,n}};
	}
    } /* namespace short_literals */
    
} /* namespace regex */
