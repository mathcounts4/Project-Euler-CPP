#include <tuple>
#include <variant>

template<class... Types>
struct Variant : public std::variant<Types...> {
  private:
    using Base = std::variant<Types...>;

    template<std::size_t... i>
    std::tuple<Types*...> splitImpl(std::index_sequence<i...>) {
	return {std::get_if<i>(this)...};
    }
    
    template<std::size_t... i>
    std::tuple<Types const*...> splitImpl(std::index_sequence<i...>) const {
	return {std::get_if<i>(this)...};
    }
    
  public:
    using Base::Base;

    std::tuple<Types*...> split() {
	return splitImpl(std::index_sequence_for<Types...>{});
    }

    std::tuple<Types const*...> split() const {
	return splitImpl(std::index_sequence_for<Types...>{});
    }
};
