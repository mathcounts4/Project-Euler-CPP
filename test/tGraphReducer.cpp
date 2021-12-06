#include "../GraphReducer.hpp"
#include "../Vec.hpp"
#include "harness.hpp"

TEST(GraphReducer,Identity) {
    Vec<Vec<UI>> adj{{0},{1},{2}};
    auto const& [numel, mapping] = graphReduction(adj);
    CHECK(mapping.size(), equals(3U));
    CHECK(numel, equals(1U));
    for (auto group : mapping) {
	CHECK(group, equals(0U));
    }
}

TEST(GraphReducer,AllToOne) {
    Vec<Vec<UI>> adj{{1},{1},{1}};
    auto const& [numel, mapping] = graphReduction(adj);
    CHECK(mapping.size(), equals(3U));
    CHECK(numel, equals(1U));
    for (auto group : mapping) {
	CHECK(group, equals(0U));
    }
}

TEST(GraphReducer,OneToAll) {
    Vec<Vec<UI>> adj{{},{0,1,2},{}};
    auto const& [numel, mapping] = graphReduction(adj);
    CHECK(mapping.size(), equals(3U));
    CHECK(numel, equals(2U));
    CHECK(mapping[0], equals(mapping[2]));
    CHECK(mapping[0], isNotEqualTo(mapping[1]));
}

TEST(GraphReducer,ProjectEuler774Example) {
    UI n = 5;
    Vec<Vec<UI>> adj(n+1);
    for (UI i = 1; i <= n; ++i) {
	for (UI j = 1; j <= n; ++j) {
	    if (i & j) {
		adj[i].push_back(j);
	    }
	}
    }
    // 1 -> {1,3,5}
    // 2 -> {2,3}
    // 3 -> {1,2,3,5}
    // 4 -> {4,5}
    // 5 -> {1,3,4,5}

    // A = {1}
    // B = {2,4}
    // C = {3,5}
    auto const& [numel, mapping] = graphReduction(adj);
    CHECK(mapping.size(), equals(6U));
    CHECK(numel, equals(4U)); // includes 0 -> 0
    CHECK(mapping[0], equals(0U));
    CHECK(mapping[1], equals(1U));
    CHECK(mapping[2], equals(2U));
    CHECK(mapping[3], equals(3U));
    CHECK(mapping[4], equals(2U));
    CHECK(mapping[5], equals(3U));
}

