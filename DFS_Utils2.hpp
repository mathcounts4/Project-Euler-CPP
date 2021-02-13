#pragma once

#include <memory>
#include <unordered_set>

#include "AlgorithmUtils2.hpp"
#include "AssertUtils.hpp"

namespace algorithm2
{
    namespace DFS
    {
	/* Hooks for DFS and their defaults, if they exist */
	struct Adj;
	template<class Node> struct Get_Set_Visited
	{
	    Get_Set_Visited() {
		static_assert(is_default_hashable<Node,std::hash<Node> >,"Node type does not meet std::unordered_set requirements to use Get_Set_Visited default hook");
	    }
	    std::unordered_set<Node> visited;
	    inline bool operator()(Get,Node const & n) const { return visited.count(n); }
	    inline void operator()(Set,Node const & n) { visited.insert(n); }
	};
	struct Before_Node;
	struct After_Node;
	struct Before_Edge;
	struct After_Edge;
	struct Before_CC;
	struct After_CC;
    }

    namespace DFS
    {
	namespace detail
	{
	    template<class Hooks> struct DFS
	    {
		Hooks const & hooks;

		template<class Node_t>
		void operator()(Node_t&& node) const {
		    using Node = No_cvref<Node_t>;
		    auto && adj = hooks.template unique<Adj>();
		    auto && get_set = hooks.template unique<Get_Set_Visited<Node> >();
		    auto && before_node = hooks.template matching<Before_Node>();
		    auto && after_node = hooks.template matching<After_Node>();
		    auto && before_edge = hooks.template matching<Before_Edge>();
		    auto && after_edge = hooks.template matching<After_Edge>();
		    
		    if (get_set(get,node))
			return;
		    before_node(node);
		    get_set(set,node);
		    for (auto && next : adj(node)) {
			before_edge(node,next);
			(*this)(next);
			after_edge(node,next);
		    }
		    after_node(node);
		}
	    };
	    template<class Hooks> DFS(Hooks&&) -> DFS<Hooks>;
	}
    }

    template<class Node_t, class... Hook>
    void dfs(Node_t&& start, Hooks<Hook...> const & hooks) {
	using Node = No_cvref<Node_t>;
	
        hooks.template assert_only<
	    DFS::Adj,
	    DFS::Get_Set_Visited<Node>,
	    DFS::Before_Node,
	    DFS::After_Node,
	    DFS::Before_Edge,
	    DFS::After_Edge
	    >();
	
	Hooks with_needed = hooks.template add_missing_defaults<DFS::Get_Set_Visited<Node> >();
	
	DFS::detail::DFS{with_needed}(start);
    }

    template<class Container, class... Hook>
    void dfs_all(Container const & all_nodes, Hooks<Hook...> const & hooks) {
	using Node = No_cvref<decltype(*all_nodes.begin())>;
	
	Hooks modified_hooks = hooks.
	    template not_matching<DFS::Before_CC,DFS::After_CC>().
	    template add_missing_defaults<DFS::Get_Set_Visited<Node> >();
	
	auto && get_set = modified_hooks.template unique<DFS::Get_Set_Visited<Node> >();
	auto && before_cc = hooks.template matching<DFS::Before_CC>();
	auto && after_cc = hooks.template matching<DFS::After_CC>();

	DFS::detail::DFS dfs{modified_hooks};
	
	for (auto&& node : all_nodes)
	    if (!get_set(get,node)) {
		before_cc(node);
		dfs(node);
		after_cc(node);
	    }
    }
}
