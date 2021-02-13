#pragma once

#include <unordered_set>
#include <vector>

#include "AlgorithmUtils.hpp"

namespace algorithm
{
    namespace DFS
    {
	template<class Node> struct Adj : No_Default<Adj,std::vector<Node> > {};
	
	template<class Node> struct Adj_To_Node : Identity {};

	template<class Node> struct Has_Adj : Constant<true> {};
	
	template<class Node> struct Get_Set_Visited :
	    Constant<true>
	{
	    static_assert(is_default_hashable<Node>,
	    		  "Node type does not meet unordered_set requirements to use default Get_Set_Visited hook");
	    
	    std::unordered_set<Node> visited;
	    
	    inline bool operator()(Node const & n, Get) const
		{ return visited.count(n); }
	    
	    inline void operator()(Node const & n, Set)
		{ visited.insert(n); }
	    
	};
	
	template<class Node> struct Before_Node : Ignore {};
	
	template<class Node> struct After_Node : Ignore {};
	
	template<class Node> struct Before_Edge : Ignore {};

	template<class Node> struct After_Edge : Ignore {};

	template<class Node> struct Before_CC : Ignore {};

	template<class Node> struct After_CC : Ignore {};
    }
    
    namespace detail
    {
	using namespace ::algorithm::DFS;
	
	template<class Node, class... Hooks>
	using DFS_Hooks =
	    Validate<Needed<Adj,
			    Adj_To_Node,
			    Has_Adj,
			    Get_Set_Visited,
			    Before_Node,
			    After_Node,
			    Before_Edge,
			    After_Edge>,
		     Used<Hooks...>,
		     Base<Node> >;
	
	template<class Node, class... Hooks>
	struct DFS : DFS_Hooks<Node,Hooks...>
	{
	    using typename DFS_Hooks<Node,Hooks...>::Deduced_Hooks;
	    DFS(Hooks const & ... hooks) : DFS_Hooks<Node,Hooks...>(hooks...) {}
	    
	    void operator()(Node const & node)
		{
		    if (Match<Get_Set_Visited,Deduced_Hooks>::operator()(node,Get{}))
			return;
		    Match<Before_Node,Deduced_Hooks>::operator()(node);
		    Match<Get_Set_Visited,Deduced_Hooks>::operator()(node,Set{});
		    if (Match<Has_Adj,Deduced_Hooks>::operator()(node))
			for (auto&& next_adj : Match<Adj,Deduced_Hooks>::operator()(node))
			{
			    Node const & next = Match<Adj_To_Node,Deduced_Hooks>::operator()(next_adj);
			    Match<Before_Edge,Deduced_Hooks>::operator()(node,next);
			    DFS::operator()(next);
			    Match<After_Edge,Deduced_Hooks>::operator()(node,next);
			}
		    Match<After_Node,Deduced_Hooks>::operator()(node);
		}
	};
    }
    

    template<class Node, class... Hooks>
    void dfs(Node const & start, Hooks const & ... hooks) {
	detail::DFS<Node,Hooks...>(hooks...)(start);
    }

    template<class Node_Container, class... Hooks>
    void dfs_all(Node_Container const & all_nodes, Hooks const & ... hooks) {
	
	using namespace DFS;
	using Node = typename Node_Container::value_type;
	using CC_Hooks = Validate<Needed<Before_CC,
					 After_CC>,
				  Used<Select_Matching<Hooks,Before_CC,After_CC>...>,
				  Base<Node> >;

	using Deduced = typename CC_Hooks::Deduced_Hooks;
	using Before = Match<Before_CC,Deduced>;
	using After = Match<After_CC,Deduced>;
	
	detail::DFS<Node,Ignore_Matching<Hooks,Before_CC,After_CC>...> dfs(hooks...);
	
	using Visited = Match<Get_Set_Visited,typename decltype(dfs)::Deduced_Hooks>;

	CC_Hooks cc_hooks(hooks...);
	for (Node const & node : all_nodes)
	{
	    if (!static_cast<Visited&>(dfs)(node,Get{}))
	    {
		static_cast<Before&>(cc_hooks)(node);
		dfs(node);
		static_cast<After&>(cc_hooks)(node);
	    }
	}
    }
}
