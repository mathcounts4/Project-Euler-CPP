#pragma once

#include "Memoized.hpp"
#include "TypeUtils.hpp"

namespace detail {
    template<UI n, UI k>
    constexpr UL constChoose();
    
    template<UI n>
    constexpr UL constPermutations();
}

template<UI n, UI k>
constexpr UL constChoose = detail::constChoose<n, k>();

template<UI n>
constexpr UL constPermutations = detail::constPermutations<n>();

namespace detail {
    template<UI n, UI k>
    constexpr UL constChoose() {
	if constexpr (k > n) {
	    return 0;
	} else if constexpr (k == 0) {
	    return 1;
	} else if constexpr (n-k < k) {
	    return ::constChoose<n, n-k>;
	} else {
	    return ::constChoose<n-1, k> + ::constChoose<n-1, k-1>;
	}
    }

    template<UI n>
    constexpr UL constPermutations() {
	if constexpr (n == 0) {
	    return 1;
	} else {
	    return ::constPermutations<n-1> * n;
	}
    }
}

struct MemoizedChoose : public RecursiveMemoized<MemoizedChoose, UL, UI, UI> {
  public:
    UL rawFcn(UI n, UI k) {
	if (k > n) {
	    return 0;
	} else if (k == 0) {
	    return 1;
	} else if (n-k < k) {
	    return (*this)(n, n-k);
	} else {
	    return (*this)(n-1, k) + (*this)(n-1, k-1);
	}
    }
};

inline UL choose(UI n, UI k) {
    return MemoizedChoose{}(n, k);
}

inline UL permutations(UI n) {
    UL result = 1;
    for (UI i = 2; i <= n; ++i) {
	result *= i;
    }
    return result;
}

