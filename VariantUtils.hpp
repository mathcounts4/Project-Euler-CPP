#pragma once

#include "Class.hpp"
#include "TypeUtils.hpp"

#include <variant>

template<class... T>
struct Class<std::variant<T...>> {
  public:
    using Type = std::variant<T...>;
    
  private:
    template<class X, class... Rest>
    struct Helper {
	static constexpr bool hasMore = sizeof...(Rest) > 0;
	static Optional<Type> parse(std::istream& is) {
	    if (auto x = Class<X>::parse(is)) {
		return Type(std::move(*x));
	    } else {
		auto xcause = Class<X>::name() + ": " + x.cause();
		if constexpr (hasMore) {
		    auto asRest = Helper<Rest...>::parse(is);
		    if (asRest) {
			return asRest;
		    }
		    xcause += ", ";
		    xcause += asRest.cause();
		}
		return Failure(std::move(xcause));
	    }
	}
	static void name(std::ostringstream& oss) {
	    oss << Class<X>::name();
	    if constexpr (hasMore) {
		oss << ", ";
		Helper<Rest...>::name(oss);
	    }
	}
	static void format(std::ostringstream& oss) {
	    oss << Class<X>::format();
	    if constexpr (hasMore) {
		oss << " | ";
		Helper<Rest...>::format(oss);
	    }
	}
    };
    
  public:
    static std::ostream& print(std::ostream& oss, Type const& t) {
	return std::visit([&oss](auto const& x) -> decltype(auto) {return Class<No_cvref<decltype(x)>>::print(oss, x);}, t);
    }
    static Optional<Type> parse(std::istream& is) {
	return Helper<T...>::parse(is);
    }
    static std::string name() {
	std::ostringstream oss;
	oss << "std::variant<";
	Helper<T...>::name(oss);
	oss << ">";
	return oss.str();
    }
    static std::string format() {
	std::ostringstream oss;
	Helper<T...>::format(oss);
	return oss.str();
    }
};

