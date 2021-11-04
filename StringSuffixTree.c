//
//  StringSuffixTree.c
//  
//
//  Created by David Fink on 3/17/16.
//
//

#include <stdio.h>

typedef struct node node;
typedef struct edge_list edge_list;
typedef struct edge edge;

struct node {
    int length; // length of the string to get to this node
    edge_list* edges;
};

struct edge {
    int start; // index of first character for this edge in the string
    int end; // index of last character + 1 for this edge in the string
    // the length of the string on this edge is end-start
    node* parent;
    node* child;
};

struct edge_list {
    edge* e;
    edge_list* next;
};

node* str_suffix_tree(char* str) {
    
}

/*
 Complicated idea:
 
 We use: RMQ to calculate LCA in O(n) processing, O(1) query
 
 We have: binary search for pattern P in string T with a suffix array for T:
 O(|P|*log(|T|)) to produce left and right bounds, then we take all suffixes between them
 This can be improved to O(|P|+log(|T|))
 
 LCP = longest common prefix
 Given a suffix array S[i] = i'th index of sorted suffixes, look up LCP(i,j) in O(1)
 Compute for neighboring elements Q[i] = LCP(i,i+1)
 Calculate Q in O(n):
 calculate S_inv
 
 h = 0
 for (i = 1 to n) {
     if (S_inv[i] > 1) {
         l = S[S_inv[i]-1]
         while (T[i+h] == T[l+h] {h++}
         Q[S_inv[i]-1] = h
         h--
     } else {
         
     }
 }
 
 Then LCP(i,j) = min(k = i to j-1) Q[k] (use RMQ on Q)
 
 binary search using LCP yields O(|P|+log(|T|))
 
 
 Suffix tree = Castesian tree for Q
 
*/
