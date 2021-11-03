#pragma once

#include <array>
#include <vector>

#include "Function.hpp"
#include "Optional.hpp"
#include "Tuple.hpp"
#include "TypeUtils.hpp"
#include "Vec.hpp"

namespace PE {
    // {arg1,arg2}
    template<class... In> using Input =
	BasicTuple<In...>;

    // {{arg1,arg2},out_expected}
    template<class Out, class... In> using Test =
	BasicTuple<Input<In...>,Out>;

    // {"arg 1","arg 2"}
    template<class... In> using Arg_Names = std::array<char const *, sizeof...(In)>;

    // {"fcn",{"arg 1","arg 2"} }
    template<class... In> using Fcn_Names =
	BasicTuple<char const *, Arg_Names<In...> >;

    // {{arg1,arg2},out_expected}
    // {{arg1,arg2},Failure{"No expected value"}}
    template<class Out, class... In> using Case =
	BasicTuple<Input<In...>,Optional<Out> >;

    /* bool ran_ok = PE::run_problem(
           argc,
	   argv,
	   fcn,
	   {"fcn",{"arg 1","arg 2"}},
	   {
	       {{t1_arg1,t1_arg2},t1_ans},
	       {{t2_arg1,t2_arg2},t2_ans},
	       {{t3_arg1,t3_arg2},t3_ans}
	   },
	   {real_a1,real_a2});
    */
    template<class Out, class... In>
    bool run_problem(
	Argc                            const  argc,
	Argv                            const  argv,
	Function<Optional<Out>,In...>   const  fcn,
	Fcn_Names<In...>                const& fcn_names,
	Vec<Test<Out,In...> >           const& tests,
	Input<In...>                    const& input);
} /* namespace PE */

#include "Impl/ProjectEuler.hpp"
