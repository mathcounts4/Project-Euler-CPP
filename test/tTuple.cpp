#include <iostream>
#include <string>

#include "../Blabber.hpp"
#include "../Tuple.hpp"
#include "harness.hpp"
#include "Section.hpp"

using namespace std;

template<class Tuple> void print_tuple(Tuple&& tuple) {
    bool started_printing_tuple = false;
    auto print_element = [&started_printing_tuple](auto&& element) -> void
	{
	    if (started_printing_tuple) {
		cout << ",";
	    }
	    started_printing_tuple = true;
	    using Type = decltype(element);
	    cout << fwd<Type>(element);
	};
    cout << "{";
    tuple_apply_all(print_element,fwd<Tuple>(tuple));
    cout << "}";
    cout << "\n";
}

TEST(Tuple,All) {
    cout << endl;
    {
	Section s("basic piecewise construction (1 blabber), defaultwise construction, and assignment");
	SmartTuple<int> a{1};
	SmartTuple<long> b{2};
	SmartTuple<int,long,char> def;
	a = 100;
	b.get<0>() = 100;
	SmartTuple<int,int,long,Blabber> t1{Construct::piecewise,a,100,b,Construct::defaultwise};
	print_tuple(t1);
    }

    {
	Section s("blabber + tuple_enclose (passes by ref) + SmartTuple<Blabber&> + RefTuple{b}");
	Blabber b;
	tuple_enclose(b);
	SmartTuple<Blabber&> t2{b};
	RefTuple ref{b};
    }

    {
        Section s("regular construction of {Blabber}");
	SmartTuple<int,long,Blabber> x{3,5L,Blabber{}};
    }
    
    {
	Section s("Piecewise construction of {Blabber}");
	SmartTuple<Blabber> t{Construct::piecewise,Construct::defaultwise};
	t.get<0>().check();
	t.ref().get<0>().check();
    }

    {
	Section s("Regular construction of {{Blabber}}");
	SmartTuple<SmartTuple<Blabber> > t{Blabber{}};
    }

    {
	Section s("Piecewise construction of {{Blabber}}");
	SmartTuple<SmartTuple<Blabber> > t{Construct::piecewise,RefTuple{Construct::piecewise,Construct::defaultwise}};
	t.get<0>().get<0>().check();
	t.ref().get<0>().get<0>().check();
    }

    {
	Section s("tuple_cat(tuple...) testing");
	cout << "First we construct distinct tuples (with different const-ness), using piecewise construction\n";

	char c = '2';
	SmartTuple<int const,char&> const t1{Construct::piecewise,RefTuple{1},c};
	SmartTuple<int const,int> t2{Construct::piecewise,3,4};
	SmartTuple<Blabber,int> const t3{Construct::piecewise,Construct::defaultwise,5};

	cout << "Now we concat the tuples\n";

	auto r = tuple_cat(t1,t2,t3);
	
	cout << "Now we assert that the resulting type is as expected" << endl;
	using R = decltype(r);
	using Expected = BasicTuple<int const,char&,int const,int,Blabber,int>;
	static_assert(::is_same<R,Expected>,"Incorrect type from tuple_cat");
	cout << "And here's the final tuple: ";

	print_tuple(r);
    }

    {
	Section s("tuple_cat_move(tuple...) testing");
	cout << "First we construct distinct tuples (with different const-ness), using piecewise construction\n";

	char c = '2';
	SmartTuple<int const,char&> const t1{Construct::piecewise,RefTuple{1},c};
	SmartTuple<int const,int> t2{Construct::piecewise,3,4};
	SmartTuple<Blabber,int> const t3{Construct::piecewise,Construct::defaultwise,5};

	cout << "Now we concat the tuples\n";

	auto r = tuple_cat_move(t1,t2,t3);
	
	cout << "Now we assert that the resulting type is as expected" << endl;
	using R = decltype(r);
	using Expected = BasicTuple<int const,char&,int const,int,Blabber,int>;
	static_assert(::is_same<R,Expected>,"Incorrect type from tuple_cat_move");

	cout << "And here's the final tuple: ";

	print_tuple(r);
    }

    {
	Section s("tuple_cat_refs(tuple...) testing");
	cout << "First we construct distinct tuples (with different const-ness), using piecewise construction\n";

	char c = '2';
	SmartTuple<int const,char&> const t1{Construct::piecewise,RefTuple{1},c};
	SmartTuple<int const,int> t2{Construct::piecewise,3,4};
	SmartTuple<Blabber,int> const t3{Construct::piecewise,Construct::defaultwise,5};

	cout << "Now we concat the tuples\n";

	auto r = tuple_cat_refs(::move(t1),t2,t3);
	
	cout << "Now we assert that the resulting type is as expected" << endl;
	using R = decltype(r);
	using Expected = RefTuple<int const &&,char&,int const &,int &,Blabber const &,int const &>;
        assert_same_type<R,Expected>();

	cout << "And here's the final tuple: ";

	print_tuple(r);
    }
}
