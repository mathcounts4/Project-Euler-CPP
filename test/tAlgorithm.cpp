#include "../AlgorithmUtils2.hpp"
#include "harness.hpp"

TEST(Algorithm,Hooks) {
    using namespace algorithm2;
    auto ignore = [](){};
    auto h1 = ref_hook<int>(ignore);
    auto h2 = ref_hook<char>(ignore);
    Hooks hs1 = h1 + h2 + h2;
    Hooks hs2 = hs1.matching<char>();
    Hooks hs3{};
    Hooks hs4 = Hooks{h1};
    hs1.unique<int>()();
    static_assert(hs2.has_none<int>,"After matching char, found an int");
    static_assert(!hs2.has_none<char>,"After matching char, did not find a char");
    static_assert(hs2.size == 2,"matching failed");
}
