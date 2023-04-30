#include "../BulgarianSolitaire.hpp"
#include "harness.hpp"

static std::string makeSummary(std::initializer_list<UI> startingPiles, UL minIndex = 1, UI maxIndex = 10000) {
    BulgarianSolitaire solitaire;
    for (UI x : startingPiles) {
	solitaire.addElementWithCount(x);
    }
    return to_string(solitaire.summarize(minIndex, maxIndex));
}

TEST(BulgarianSolitaire, NormalCases) {
    // 5 1 1
    // 4 _3_
    // 3 2 _2_
    // 2 1 1 _3_
    // 1 2 _4_
    // 1 3 _3_
    // 2 2 _3_
    // and we cycle, so {5,1,1} -> {3,2,[3,4,3,3]*}
    CHECK(makeSummary({5,1,1}), equals("{3,2,[3,4,3,3]*}"));
    
    CHECK(makeSummary({1,2,3}), equals("{[3,3,3,3]*}"));
}

TEST(BulgarianSolitaire, DoNotRecordEarlySeq) {
    // For sequences that are slow to converge to a stable state, sometimes we don't want to record the early non-repeating values if we don't need them.
    
    CHECK(makeSummary({1000, 1}, 10000000u, 100000000u), equals("starting from 956: {[44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,45,45,45,45,45,45,45,45,45,45,45]*}"));
}

TEST(BulgarianSolitaire, SingleStartingPile) {
    CHECK(makeSummary({6}), equals("{1,2,2,[3,3,3,3]*}"));
}
