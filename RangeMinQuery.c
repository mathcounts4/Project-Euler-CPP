//
//  RangeMinQuery.c
//  
//
//  Created by David Fink on 3/24/16.
//
//

#include <stdio.h>

// preprocess A[0...n-1] so we can answer queries:
// Q(i,j) that determine the smallest A[k] with i <= k <= j

// trivial solutions:

// (1)
// precompute nothing, each query is O(n)

// (2)
// precompute everything (using dynamic programming) O(n^2), each query is O(1)

// better solutions:

// (3)
// precompute answers to queries where j-i+1 = 2^r for some r
// O(n*log(n)) space and time to precompute
// O(1) time for each query (overlap two intervals of length 2^r
//     up from i, down from j, r is the largest int with 2^r <= j-i+1

// (4)
// cut array into blocks of size log(n). For each, compute the min (new array B)
// solve the problem for array B using the last algorithm
// when given a query, check the two blocks on the ends element-by-element,
// and the blocks in the middle by using the solution for B
// This takes O(log(n)) for queries
// Preprocessing is O(n) + O(n/log(n) * log(n/log(n))) = O(n)

// (5)
// modification on that: for each block in B, compute using (3)
// preprocessing is then O(n) + n/log(n) O(log(n)log(log(n))) = O(n*log(log(n)))
// query is O(1)

// (6)
// preprocessing O(n)
// query O(1)
// cut into blocks of size b = (1/2) log_4(n)
// create B as in (4), use (3) on B.
// For each block, precompute answers for every possible block-type
// There are at most 4^b <= sqrt(n) block types
// Thus computing all pairs of block types is O(n)
// Then the queries are O(1)
// Block type = one Cartesian tree of size b

// Cartesian tree:
// build a tree from the smallest element where each node has at most two leaves:
// leaf on the left is the min element left of the node,
// leaf on the right is the min element right of the node

// there are at most 4^b Cartesian trees of size b
// Q(0) = 1
// Q(n) = sum(i = 1 to n) Q(i-1) * Q(n-i)
// Q(n) = choose(2n,n)/(n+1) <= 4^n

// How to find the Cartesian tree for a block x[1...b]:
// left-to-right, have Cartesian tree for x[i...k]
// Adding x[k+1] to the tree - walk up from x[k] in the tree until we get
// to a node with value < x[k+1]
// total is linear time - whenever we walk up the right branch, that shortens it,
// reducing future computation time
// there is a binary string of length 2b to describe this process (how many times we walked up)
// producing an index into the block type table



// This RMQ problem can then solve the LCA (least common ancestor) problem in O(n) precomputing, O(1) query:
// wish to solve LCA on tree T
//
// get list of nodes in DFS (save each node in the list when it is started, between its children, and when it finishes) = O(n)
// for each node, save the start index
// make a list corresponding to that one where each node is replaced by its depth
// compute RMQ on this list
// query(X,Y) = RMQ(start(X),start(Y))
