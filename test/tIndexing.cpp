#include "../Indexing.hpp"
#include "harness.hpp"

TEST(Indexing,Basic) {
    using S = Indices<2,5,4>;
    using T = Indices<1,4,2>;
    using U = Indices<3,5,7,7>;
    
    assert_same_value<T::size(),3>();
    assert_same_value<S::size(),3>();
    assert_same_value<U::size(),4>();

    assert_same_type<Make_Indices<5>,Indices<0,1,2,3,4> >();

    assert_same_type<List_Element<1,int,long,char>,long>();

    assert_same_value<get_index<1,S>,5>();
    
    assert_same_type<Get_Indices<Indices<1,0,2>,S>,Indices<5,2,4> >();
    
    assert_same_value<combined_sum<T>,7>();
    
    assert_same_value<all_same_index<Indices<8,8,8> >,8>();
    
    assert_same_type<Sum_Indices<S,T,S>,Indices<5,14,10> >();
    
    assert_same_type<Subtract_Indices<S,T>,Indices<1,1,2> >();

    assert_same_type<Prefix_Sum_Inclusive<T>,Indices<1,5,7> >();

    assert_same_type<Prefix_Sum_Exclusive<T>,Indices<0,1,5> >();

    assert_same_type<Repeat_Index<8,3>,Indices<8,8,8> >();

    assert_same_type<Outer_Flatten_Indices<T>,Indices<0,1,1,1,1,2,2> >();
    assert_same_type<Outer_Flatten_Indices<Indices<0,0,0> >,Indices<> >();

    assert_same_type<Inner_Flatten_Indices<T>,Indices<0,0,1,2,3,0,1> >();

    assert_same_type<Concat_Indices<S,T,U>,Indices<2,5,4,1,4,2,3,5,7,7> >();

/*
    
    // Sum_Indices<F> seems to require |F|^2 memory for compilation.
    // This is because List_Element<i,T...> takes |F| for each i,
    // of which there are |F|.
    // Takes 2+ GB of memory for big = 1e3

    constexpr SZ big = 1e3;
    using F = Make_Indices<big>;
    assert_same_value<F::size(),big>();
    assert_same_value<get_index<34,F>,34>();
    assert_same_value<combined_sum<F>,big*(big-1)/2>();
    using G = Sum_Indices<F>;
    G g;
    (void) g;

*/
}
