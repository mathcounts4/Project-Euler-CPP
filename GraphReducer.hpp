#pragma once

#include "MyHashVector.hpp"
#include "TypeUtils.hpp"
#include "U_Map.hpp"
#include "Vec.hpp"

struct GraphReduction {
    UI fNewNumElements;
    Vec<UI> fOldToNewIndices;
};

// For a graph with vertices 0,1,...,n-1 and adjacency map [i] that lists all nodes adjacent to i:
// Reduces the graph by merging nodes to groups such that, for any two nodes X and Y in the same group G and any group H, the number of nodes adjacent to X in H equals the number of nodes adjacent to Y in H.
// The result is the minimal reduced graph (minimum number of groups).
// The return value is {number of groups, map from i to group}.
template<class UIMap>
GraphReduction graphReduction(UIMap&& map) {
    UI n = static_cast<UI>(map.size());
    // initially map all elements to the same output
    GraphReduction attempt{1, Vec<UI>(n, 0)};
    while (true) {
	// map each index to a count of how many in each group are adj to it
	// if any in the same group have different counts, split them.
	auto attemptSize = attempt.fNewNumElements;
	auto const& group = attempt.fOldToNewIndices;
	U_Map<Vec<UI>, UI> edgeCountToNewIndex;
	Vec<UI> oldToNewIndices(n);
	for (UI i = 0; i < n; ++i) {
	    Vec<UI> edgeCounts(attemptSize, 0);
	    for (UI adj : map[i]) {
		++edgeCounts[group[adj]];
	    }
	    oldToNewIndices[i] = edgeCountToNewIndex.emplace(std::move(edgeCounts), edgeCountToNewIndex.size()).first->second;
	}
	auto newAttemptSize = static_cast<UI>(edgeCountToNewIndex.size());
	if (newAttemptSize == attemptSize) {
	    return attempt;
	}
	attempt = {newAttemptSize, std::move(oldToNewIndices)};
    }
}

