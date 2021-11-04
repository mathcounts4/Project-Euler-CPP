#pragma once

#include "Impl/Function.hpp"

/* Function<Output,Inputs...> */
template<class Output, class... Inputs>
using Function = detail::Function<Output,Inputs...>;

/* Input_Tuple<F> and Output<F> */
template<auto f> using Input_Tuple = detail::Input_Tuple<f>;
template<auto f> using Output = detail::Output<f>;

/* Invoke_Result<Functor,Args...> */
template<class Functor, class... Args>
using Invoke_Result = detail::Invoke_Result<Functor,Args...>;
