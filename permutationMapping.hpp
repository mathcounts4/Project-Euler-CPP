#pragma once

#include "combinations.hpp"
#include "TypeUtils.hpp"

#include <array>

template<SZ i, SZ n>
void fillInPermutation(std::array<UI, n>& output, std::bitset<n>& used, UL index) {
    if constexpr (i < n) {
	UI result = 0;
	UI numToUse = static_cast<UI>(index / constPermutations<n-i-1>);
	do {
	    while (used[result]) {
		++result;
	    }
	} while (numToUse-- && ++result);
	// ++result is always true - we just want that side effect if the do...while is not stopping
    
	output[i] = result;
	used[result] = true;
    
	fillInPermutation<i+1>(output, used, index % constPermutations<n-i-1>);
    }
}

// For a nonnegative int n, there are n! permutations of length n. These can be indexed from 0 in a natural way (lexicographically).
// For example, for n=3, we have {0,1,2}, {0,2,1}, {1,0,2}, {1,2,0}, {2,0,1}, {2,1,0}.

// This function maps an n and a 0-based permutation index to the 0-based permutation at that lexicographic index.
template<UI n>
std::array<UI, n> permutationFromIndex(UL index) {
    std::bitset<n> used;
    std::array<UI, n> output{};
    fillInPermutation<0>(output, used, index);
    return output;
}


// This function maps a 0-based permutation to a 0-based permutation index.
template<class Array>
UL permutationToIndex(Array const& permutation) {
    UL factorial = 1;
    UI fMult = 0;
    UL total = 0;
    auto const sz = permutation.size();
    for (auto i = sz; i--; factorial *= ++fMult) {
	for (auto j = sz; --j > i; ) {
	    if (permutation[i] > permutation[j]) {
		total += factorial;
	    }
	}
    }
    return total;
}


// For nonnegative ints n and k, there are choose(n,k) combinations of subsets of k objects from a set of n objects.
// These can be indexed in a natural way (lexicographically).
// For example, for n=4 and k=2, we have {0,1}, {0,2}, {0,3}, {1,2}, {1,3}, {2,3}.
// These are represented by bit vectors of length n, with "true" where subset elements are selected.

// This function maps an n and k and a combination index to the bit vector combination at that lexicographic index.
template<UI n, UI k>
std::bitset<n> combinationFromIndex(UL index) {
    std::bitset<n> result;
    MemoizedChoose choose;
    auto localN = n;
    auto localK = k;
    for (SZ i = 0; i < n; ++i) {
	auto numWithThisBitSet = choose(localN-1, localK-1);
	if (index >= numWithThisBitSet) {
	    index -= numWithThisBitSet;
	} else {
	    result[i] = true;
	    --localK;
	}
	--localN;
    }
    return result;
}

// This function maps a bit vector combination to its lexicographic index.
template<class Bitset>
UL combinationToIndex(Bitset const& combination) {
    MemoizedChoose choose;
    SZ const sz = combination.size();
    UI n = static_cast<UI>(sz);
    UI k = 0;
    for (SZ i = 0; i < sz; ++i) {
	if (combination[i]) {
	    ++k;
	}
    }
    UL total = 0;
    for (SZ i = 0; i < sz; ++i) {
	if (combination[i]) {
	    --k;
	} else {
	    total += choose(n-1, k-1);
	}
	--n;
    }
    return total;
}
