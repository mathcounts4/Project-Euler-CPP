#pragma once

#include <array>
#include <chrono>
#include <iostream>
#include <istream>
#include <string>
#include <string_view>
#include <vector>

#include "../Class.hpp"
#include "../Function.hpp"
#include "../Indexing.hpp"
#include "../Optional.hpp"
#include "../StringUtils.hpp"
#include "../Tuple.hpp"
#include "../TypeUtils.hpp"
#include "../Vec.hpp"

#include "../ProjectEuler.hpp"

class Mod;

namespace PE {
    template<class In, SZ... i>
    std::string to_input_format(
	std::array<char const *,In::size> const &   arg_names,
	Indices<i...>) {
	std::vector<std::string> names {
	    std::string(arg_names[i] ? arg_names[i] : "[no_name]") +
		": " +
		Class<No_cvref<typename In::template Element<i> > >::format()...};
	return to_string(names);
    }
	
    template<class Out, class... In>
    Optional<Vec<Case<Out,In...> > >
    get_cases(
	Argc                          const   argc,
	Argv                          const   argv,
	Arg_Names<In...>              const & arg_names,
	Vec<Test<Out,In...> >         const & tests,
	Input<In...>                  const & real_input) {
        Vec<Case<Out,In...> > cases;

	constexpr auto ans_unknown = "Answer unknown for real input";
	
	// no arguments: use the real input
	if (argc <= 1) {
	    cases.emplace_back(real_input,Failure{ans_unknown});
	    return std::move(cases);
	}

	if ("test" == std::string_view(argv[1])) {
	    if (tests.size() == 0)
		return Failure{"No tests were provided"};
	    
	    // special case: test all
	    if (argc == 2 || (argc == 3 && "all" == std::string_view(argv[2]))) {
		for (auto const & test : tests)
		    cases.emplace_back(test.template get<0>(),test.template get<1>());
		return std::move(cases);
	    }
		
	    // special case: test x y z
	    using Index = UI;
	    for (int arg = 2; arg < argc; ++arg) {
		Optional<Index> index = from_string<Index>(argv[arg]);
		if (!index)
		    return Failure("Couldn't identify valid test index, caused by:\n" + index.cause());
		    
		if (*index <= 0 || *index > tests.size())
		    return Failure("Invalid test index " + to_string(*index) +
				   " for range [1," + to_string(tests.size()) + "]");

		auto const & test = tests[*index-1];
		cases.emplace_back(test.template get<0>(),test.template get<1>());
	    }
	    return std::move(cases);
	}

	// general case: combine all argv into a single stream
	std::string input = argv[1];
	for (int arg = 2; arg < argc; ++arg) {
	    input += ' ';
	    input += argv[arg];
	}
	std::istringstream iss(input);

	while (std::ws(iss).good()) {
	    auto value = Class<Input<In...> >::parse(iss);
	    if (!value) {
		return Failure(
		    "Invalid input:\n" + input + "\n" +
		    value.cause() + "\n" +
		    "Expected one of the following:\n" +
		    "[nothing]\n" +
		    "[exactly] test all\n" +
		    "[exactly] test #\n" +
		    "[one or more] " + to_input_format<Input<In...> >(arg_names,typename Input<In...>::Indices{}));
	    }
		
	    cases.emplace_back(std::move(*value),Failure{ans_unknown});
	}
    
	return std::move(cases);
    }

    template<class Out, class... In>
    bool run_problem(
	Argc                                   const   argc,
	Argv                                   const   argv,
	Function<Optional<Out>,In...>          const   fcn,
	Fcn_Names<In...>                       const & fcn_names,
	Vec<Test<Out,In...> >                  const & tests,
	Input<In...>                           const & input_problem)
    {
	static_assert(all<is_pure<In>...>,"All function arguments must have no cvref qualifiers");

	auto const & fcn_name = fcn_names.template get<0>();
	auto const & input_names = fcn_names.template get<1>();
	
	auto cases = get_cases(argc,argv,input_names,tests,input_problem);
	if (!cases) {
	    std::cerr << cases.cause() << std::endl;
	    return false;
	}
    
	bool ran_ok = true;
	for (auto const & case1 : *cases) {
	    auto const & input = case1.template get<0>();
	    using Input_Tuple_Base = typename No_cvref<decltype(input)>::Base;
	    auto const & expected = case1.template get<1>();
	    std::cout << fcn_name << "(" << static_cast<Input_Tuple_Base const &>(input) << ")";
	    if (expected)
		std::cout << " [expected " << *expected << "]";
	    std::cout << std::flush;
	    auto start = std::chrono::high_resolution_clock::now();
	    try {
		if (auto output = apply_tuple(fcn,input)) {
		    auto end = std::chrono::high_resolution_clock::now();
		    std::chrono::duration<double> diff = end-start;
		    auto eq = "=";
		    if constexpr (is_same<Mod, No_cvref<decltype(*output)>>) {
			if (output->get_mod() != 0) {
			    eq = "≡";
			}
		    }
		    std::cout << " " << eq << " [" << diff.count() << " seconds] " << *output << std::endl;
		} else {
		    ran_ok = false;
		    auto end = std::chrono::high_resolution_clock::now();
		    std::chrono::duration<double> diff = end-start;
		    std::cout << " failed [" << diff.count() << " seconds]" <<
			" due to:\n" << output.cause() << std::endl;
		}
	    } catch (std::exception const & ex) {
		ran_ok = false;
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end-start;
		std::cout << " failed [" <<  diff.count() << " seconds]" <<
		    " due to exception:\n" << ex.what() << std::endl;
	    }
	}
    
	return ran_ok;
    }
} /* namespace PE */
