#include <array>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../Blabber.hpp"
#include "../ContainerTransformer.hpp"
#include "../DFS_Utils2.hpp"
#include "../ExitUtils.hpp"
#include "harness.hpp"
#include "Section.hpp"

using namespace std;
using namespace algorithm2;
using namespace DFS;

struct A_Adj {
    Blabber b;
    template<class T>
    vector<T> operator()(T const &) const {
	b.check();
	return {};
    }
};

TEST(DFS,All) {
    {
	// Error: "Multiple hooks provided for unique hook"
	//dfs(3,ref_hook<Adj>(A_Adj{})+ref_hook<Adj>(A_Adj{}));

	// Error: "Extra hooks of unknown type specified"
	//dfs(3,ref_hook<Adj>(A_Adj{})+ref_hook<int>(A_Adj{}));

	// Error: "Node type does not meet unordered_set requirements to use default Get_Set_Visited default hook"
	//dfs(Blabber{},hooks(ref_hook<Adj>(A_Adj{})));
    }
    
    {
	Section s("rvalue(rvalue)");
	dfs(0,hooks(ref_hook<Adj>(A_Adj{})));
    }
    {
	Section s("rvalue(lvalue)");
        A_Adj f;
	dfs(0,hooks(ref_hook<Adj>(f)));
    }

    
    
    {
	Section s("lvalue(lvalue)");
        A_Adj f;
	auto a = ref_hook<Adj>(f);
	dfs(0,Hooks{a});
    }
    {
	Section s("lvalue(rvalue)");
	auto a = owned_hook<Adj,A_Adj>();
	dfs(0,Hooks{a});
    }
    

    
    {
	Section s("dfs on a real unordered_map");
	unordered_map<int,vector<int> > const m
	{
	    {4,{5,3}},
	    {5,{6,4,7}},
	    {2,{9,10}},
	    {3,{4,1,0}}
	};
	SZ sp = 0;
	auto bn = owned_hook<Before_Node>([&sp](int n){ cout << string(sp++,' ') << n << endl; });
	auto an = owned_hook<After_Node>([&sp](int n){ cout << string(--sp,' ') << n << endl; });
	auto a = owned_hook<Adj>([&m,e=vector<int>{}](int n)->decltype(m.at(n)){ if (m.count(n)) return m.at(n); return e; });
        
	{
	    dfs(4,a+bn+an);
	    sp = 0;
	}
	{
	    vector<int> keys;
	    for (auto&& [key,value] : m)
		keys.push_back(key);
	    dfs_all(keys,a+bn+an+ref_hook<Before_CC>([](int node){cout<<"CC start "<<node<<endl;})+ref_hook<After_CC>([](int node){cout<<"CC end "<<node<<endl;}));
	    sp = 0;
	}
    }


    
    {
	Section s("dfs on a custom pointer-based graph");
	struct Node
	{
	    string data;
	    vector<Node*> next;
	    Node(string_view const & str) : data(str) {}
	};
	
	Node a("a"),b("b"),c("c"),d("d"),e("e"),f("f");
	// e <-> a -> d <-> b <- c -> f
	//        ^        /
	//         `------/
	a.next = {&e,&d};
	b.next = {&a,&d};
	c.next = {&b,&f};
	d.next = {&b};
	e.next = {&a};
	f.next = {};

	for (Node* n : {&a,&b,&c,&d,&e,&f}) {
	    cout << "[edges] " << n->data << " ->";
	    for (Node* x : n->next)
		cout << " " << x->data;
	    cout << endl;
	}
	
	SZ sp = 0;
	
	auto bn = owned_hook<Before_Node>([&sp](Node const & n){ cout << string(sp++,' ') << n.data << endl; });
	auto an = owned_hook<After_Node>([&sp](Node const & n){ cout << string(--sp,' ') << n.data << endl; });
	auto adj = owned_hook<Adj>([](Node const & n){ return ContainerTransformer{n.next,[](Node* np){return *np;}}; });

        struct Get_Set
	{
	    unordered_set<string_view> visited;
	    bool operator()(Get,Node const & node) const
		{ return visited.count(node.data); }
	    void operator()(Set,Node const & node)
		{ visited.insert(node.data); }
	};
	
	{
	    // Error: Node type does not meet std::unordered_set requirements to use Get_Set_Visited default hook
	    //dfs(a,bn+an+adj);
	    
	    auto visited = owned_hook<Get_Set_Visited<Node>,Get_Set>();
	    dfs(a,bn+an+adj+visited);
	    sp = 0;
	}

	{
	    cout << "dfs_all with vector{&d,&c}:" << endl;
	    auto visited = owned_hook<Get_Set_Visited<Node>,Get_Set>();
	    dfs_all(ContainerTransformer{std::array<Node*,2>{&d,&c},[](auto&& n)->decltype(auto){return *n;}},bn+an+adj+visited);
	}
    }
}
