#include "../ContainerTransformer.hpp"
#include "../Unique.hpp"
#include "harness.hpp"

#include <vector>
#include <map>

TEST(ContainerTransformer,Basic) {
    std::vector<Unique> const v(2);
    for (Unique const * u : ContainerTransformer{v,[](Unique const& u){return &u;}})
	CHECK(u != nullptr, isTrue());
    
    auto const address_of = [](auto&& x) { return &x; };
    for (Unique * u : ContainerTransformer{std::vector<Unique>(2),address_of})
	CHECK(u != nullptr, isTrue());
}

TEST(ContainerTransformer,LambdaState) {
    std::map<int,int> const m{
	{5,4},
	{14,12},
	{-5,-178},
	{25,-1235}
    };
    
    int const one = 1;
    int prev = 0;
    auto const sum_adj_keys_plus_one = [one,&prev](auto&& x)
	{ int const val = prev + x.first + one; prev = x.first; return val; };
    
    // expected: 0+(-5)+1, -5+5+1, 5+14+1, 14+25+1
    std::vector expected{0-5+1,-5+5+1,5+14+1,14+25+1};
    std::size_t idx = 0;
    for (int x : ContainerTransformer{m,sum_adj_keys_plus_one}) {
        CHECK(x, equals(expected[idx]));
	++idx;
    }
}
