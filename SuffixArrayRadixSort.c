//
//  SuffixArrayRadixSort.c
//  
//
//  Created by David Fink on 3/23/16.
//
//

#include <stdio.h>
#include <stdlib.h>

typedef struct node node;

struct node {
    int val;
    int original_index;
    node* next;
};

// all values in x[size] are in [0,n^pow)
// operates in O(pow * (size + n))
// sorts x in place, and generates a table of the original indices
// so the values at those indices are sorted
// (basiclaly it just saves each index with each value)
// The table of indices is the return value
int* radixSort(int size, int* x, int n, int pow) {
    
    int* sortedIndices = malloc(size * sizeof(int));
    
    node nodes[size];
    
    // sort by largest digit
    int ans_index_1;
    int ans_index_2;
    int next_free_node = 0;
    
    node* lists[n];
    node* firsts[n];
    int i,j;
    for (i = 0; i < n; i++) {
        firsts[i] = NULL;
    }
    
    for (ans_index_1 = 0; ans_index_1 < size; ans_index_1++) {
        int value = x[ans_index_1];
        int index = value % n;
        nodes[next_free_node].val = value;
        nodes[next_free_node].original_index = ans_index_1;
        nodes[next_free_node].next = NULL;
        if (firsts[index] == NULL) {
            firsts[index] = &nodes[next_free_node];
        } else {
            lists[index]->next = &nodes[next_free_node];
        }
        lists[index] = &nodes[next_free_node];
        next_free_node++;
    }
    
    ans_index_1 = 0;
    for (i = 0; i < n; i++) {
        node* cur = firsts[i];
        while (cur != NULL) {
            x[ans_index_1] = cur->val;
            sortedIndices[ans_index_1] = cur->original_index;
            ans_index_1++;
            cur = cur->next;
        }
    }
    
    
    int pow_i = 1; // the digit we're sorting by
    int div = 1;
    while (pow_i < pow) {
        // the numbers are already sorted by the smaller digits (<pow_i)
        
        ans_index_1 = 0;
        ans_index_2 = 0;
        
        for (i = 0; i < n; i++) {
            firsts[i] = NULL;
        }
        
        next_free_node = 0;
        while (ans_index_1 < size) {
            // still have values with next digit = i
            int value = x[ans_index_1];
            int index = (value / div / n) % n;
            nodes[next_free_node].val = value;
            nodes[next_free_node].original_index = sortedIndices[ans_index_1];
            nodes[next_free_node].next = NULL;
            if (firsts[index] == NULL) {
                firsts[index] = &nodes[next_free_node];
            } else {
                lists[index]->next = &nodes[next_free_node];
            }
            lists[index] = &nodes[next_free_node];
            next_free_node++;
            ans_index_1++;
        }
        
        for (i = 0; i < n; i++) {
            node* cur = firsts[i];
            while (cur != NULL) {
                x[ans_index_2] = cur->val;
                sortedIndices[ans_index_2] = cur->original_index;
                ans_index_2++;
                cur = cur->next;
            }
        }
        
        pow_i++;
        div *= n;
    }
    
    return sortedIndices;
}
