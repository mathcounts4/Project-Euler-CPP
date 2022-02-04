#pragma once

#include "Function.hpp"

#include <utility>

template<class F>
struct FunctionWrapper : F {
    FunctionWrapper(F f)
	: F(std::forward<F>(f)) {
    }
    
    using F::operator();
};

template<class Out, class... In>
struct FunctionWrapper<Function<Out, In...>> {
    Function<Out, In...> f;
    
    Out operator()(In... in) {
	return f(std::forward<In>(in)...);
    }
};
template<class T> FunctionWrapper(T) -> FunctionWrapper<T>;

template<class... Functors>
struct Overload : public Functors... {
    template<class... Args>
    Overload(Args&&... args)
	: Functors{std::forward<Args>(args)}... {
    }
    using Functors::operator()...;
};
template<class... Ts> Overload(Ts...) -> Overload<FunctionWrapper<Ts>...>;

