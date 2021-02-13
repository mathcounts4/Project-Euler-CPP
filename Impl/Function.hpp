#pragma once

#include "../Tuple.hpp"
#include "../TypeUtils.hpp"

namespace detail
{
    template<class Output, class... Inputs>
    using Function = Output(*)(Inputs...);
    
    template<class T> struct Function_Traits;
    template<class Output_T, class... Inputs>
    struct Function_Traits<Function<Output_T,Inputs...> >
    {
	using Input_Tuple = BasicTuple<Inputs...>;
	using Output = Output_T;
    };
    template<auto f> using Input_Tuple = typename Function_Traits<decltype(f)>::Input_Tuple;
    template<auto f> using Output = typename Function_Traits<decltype(f)>::Output;

    template<class Functor, class... Args>
    using Invoke_Result = decltype(declval<Functor>()(declval<Args>()...));
    
}
