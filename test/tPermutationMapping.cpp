#include "../permutationMapping.hpp"
#include "harness.hpp"

TEST(permutationFromIndex, smallExamples) {
    CHECK(permutationFromIndex<0>(0), equals(std::array<UI, 0>{}));
    CHECK(permutationFromIndex<2>(1), equals(std::array<UI, 2>{1, 0}));
    CHECK(permutationFromIndex<10>(0), equals(std::array<UI, 10>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
    CHECK(permutationFromIndex<4>(1), equals(std::array<UI, 4>{0, 1, 3, 2}));
    CHECK(permutationFromIndex<4>(6), equals(std::array<UI, 4>{1, 0, 2, 3}));
    CHECK(permutationFromIndex<4>(13), equals(std::array<UI, 4>{2, 0, 3, 1}));
    CHECK(permutationFromIndex<4>(23), equals(std::array<UI, 4>{3, 2, 1, 0}));
    CHECK(permutationFromIndex<20>(constPermutations<20> - 1), equals(std::array<UI, 20>{19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0}));
}

TEST(permutationToIndex, smallExamples) {
    CHECK(permutationToIndex(std::array<UI, 0>{}), equals(0U));
    CHECK(permutationToIndex(std::array<UI, 2>{1, 0}), equals(1U));
    CHECK(permutationToIndex(std::array<UI, 10>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}), equals(0U));
    CHECK(permutationToIndex(std::array<UI, 4>{0, 1, 3, 2}), equals(1U));
    CHECK(permutationToIndex(std::array<UI, 4>{1, 0, 2, 3}), equals(6U));
    CHECK(permutationToIndex(std::array<UI, 4>{2, 0, 3, 1}), equals(13U));
    CHECK(permutationToIndex(std::array<UI, 4>{3, 2, 1, 0}), equals(23U));
    CHECK(permutationToIndex(std::array<UI, 20>{19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0}), equals(constPermutations<20> - 1));
}

TEST(permutationIndexRoundTrip, four) {
    for (UI i = 0; i < constPermutations<4>; ++i) {
	CHECK(permutationToIndex(permutationFromIndex<4>(i)), equals(i));
    }
}

TEST(combinationFromIndex, smallExamples) {
    CHECK((combinationFromIndex<0,0>)(0U), equals(std::bitset<0>{}));
    CHECK((combinationFromIndex<2,1>)(1U), equals(std::bitset<2>{"10"}));
    CHECK((combinationFromIndex<10,1>)(0U), equals(std::bitset<10>{"1"}));
    CHECK((combinationFromIndex<10,2>)(0U), equals(std::bitset<10>{"11"}));
    CHECK((combinationFromIndex<10,3>)(0U), equals(std::bitset<10>{"111"}));
    CHECK((combinationFromIndex<4,2>)(0U), equals(std::bitset<4>{"0011"}));
    CHECK((combinationFromIndex<4,2>)(1U), equals(std::bitset<4>{"0101"}));
    CHECK((combinationFromIndex<4,2>)(2U), equals(std::bitset<4>{"1001"}));
    CHECK((combinationFromIndex<4,2>)(3U), equals(std::bitset<4>{"0110"}));
    CHECK((combinationFromIndex<4,2>)(4U), equals(std::bitset<4>{"1010"}));
    CHECK((combinationFromIndex<4,2>)(5U), equals(std::bitset<4>{"1100"}));
    CHECK((combinationFromIndex<4,3>)(0U), equals(std::bitset<4>{"0111"}));
    CHECK((combinationFromIndex<4,3>)(1U), equals(std::bitset<4>{"1011"}));
    CHECK((combinationFromIndex<4,3>)(2U), equals(std::bitset<4>{"1101"}));
    CHECK((combinationFromIndex<4,3>)(3U), equals(std::bitset<4>{"1110"}));
    CHECK((combinationFromIndex<40,20>)(choose(40, 20) - 1), equals(std::bitset<40>{std::string(20, '1') + std::string(20, '0')}));
    CHECK((combinationFromIndex<40,20>)(choose(40, 20) - 2), equals(std::bitset<40>{std::string(19, '1') + "01" + std::string(19, '0')}));
}

TEST(combinationToIndex, smallExamples) {
    CHECK(combinationToIndex(std::bitset<0>{}), equals(0U));
    CHECK(combinationToIndex(std::bitset<2>{"10"}), equals(1U));
    CHECK(combinationToIndex(std::bitset<10>{"1"}), equals(0U));
    CHECK(combinationToIndex(std::bitset<10>{"11"}), equals(0U));
    CHECK(combinationToIndex(std::bitset<10>{"111"}), equals(0U));
    CHECK(combinationToIndex(std::bitset<4>{"0011"}), equals(0U));
    CHECK(combinationToIndex(std::bitset<4>{"0101"}), equals(1U));
    CHECK(combinationToIndex(std::bitset<4>{"1001"}), equals(2U));
    CHECK(combinationToIndex(std::bitset<4>{"0110"}), equals(3U));
    CHECK(combinationToIndex(std::bitset<4>{"1010"}), equals(4U));
    CHECK(combinationToIndex(std::bitset<4>{"1100"}), equals(5U));
    CHECK(combinationToIndex(std::bitset<4>{"0111"}), equals(0U));
    CHECK(combinationToIndex(std::bitset<4>{"1011"}), equals(1U));
    CHECK(combinationToIndex(std::bitset<4>{"1101"}), equals(2U));
    CHECK(combinationToIndex(std::bitset<4>{"1110"}), equals(3U));
    CHECK(combinationToIndex(std::bitset<40>{std::string(20, '1') + std::string(20, '0')}), equals(choose(40, 20) - 1));
    CHECK(combinationToIndex(std::bitset<40>{std::string(19, '1') + "01" + std::string(19, '0')}), equals(choose(40, 20) - 2));
}

TEST(combinationIndexRoundTrip, four) {
    constexpr UI n = 6;

#define LOOP_ALL(chosen)						\
    do {								\
	for (UL i = 0, num = choose(n, chosen); i < num; ++i) {		\
	    CHECK(combinationToIndex(combinationFromIndex<n, chosen>(i)), equals(i)); \
	}								\
    } while (0)
    
    LOOP_ALL(0);
    LOOP_ALL(1);
    LOOP_ALL(2);
    LOOP_ALL(3);
    LOOP_ALL(4);
    LOOP_ALL(5);
    LOOP_ALL(6);
#undef LOOP_ALL
    
}
