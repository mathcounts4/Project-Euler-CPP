#pragma once

#include "Function.hpp"
#include "Optional.hpp"
#include "Tuple.hpp"
#include "TypeUtils.hpp"

namespace PE {
    
    template<auto Function>
    Output<Function> SumFcn(std::vector<Input_Tuple<Function> > inputs) {
	static_assert(is_a<Optional,Output<Function> >, "Function output should be Optional");
	Output<Function> result(Failure("No inputs provided in list"));
	for (auto const& input : inputs) {
	    if (auto output = apply_tuple(Function,input)) {
		if (result) {
		    *result += std::move(*output);
		} else {
		    result = std::move(*output);
		}
	    } else {
		return Failure(std::move(output.cause()));
	    }
	}
	return result;
    }
    
} /* namespace PE */
