//
//  RadixSort.c
//  
//
//  Created by David Fink on 3/22/16.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef struct long_node long_node;

struct long_node {
    long val;
    long_node* next;
};

// all values in x[size] are in [0,n^pow)
// operates in O(pow * (size + n))
long* radixSort(int size, long* x, int n, int pow) {
    
    long* ans = malloc(size * sizeof(long));
    
    long_node nodes[size];
    
    // sort by largest digit
    int ans_index_1;
    int ans_index_2;
    int next_free_node = 0;
    
    long_node* lists[n];
    long_node* firsts[n];
    int i,j;
    for (i = 0; i < n; i++) {
        firsts[i] = NULL;
    }
    
    for (ans_index_1 = 0; ans_index_1 < size; ans_index_1++) {
        long value = x[ans_index_1];
        int index = value % n;
        nodes[next_free_node].val = value;
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
        long_node* cur = firsts[i];
        while (cur != NULL) {
            ans[ans_index_1] = cur->val;
            ans_index_1++;
            cur = cur->next;
        }
    }
    
    
    int pow_i = 1; // the digit we're sorting by
    long div = 1;
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
            long value = ans[ans_index_1];
            int index = (value / div / n) % n;
            nodes[next_free_node].val = value;
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
            long_node* cur = firsts[i];
            while (cur != NULL) {
                ans[ans_index_2] = cur->val;
                ans_index_2++;
                cur = cur->next;
            }
        }
        
        pow_i++;
        div *= n;
    }
    
    return ans;
}

int randLong(int max) {
    return rand() % max;
}

void radix_sort_text() {
    
    srand(time(NULL));
    
    int n = 20;
    int i;
    
    
    long* x = malloc(n*sizeof(long));
    for (i = 0; i < n; i++) {
        x[i] = randLong(1000);
        printf("%ld ",x[i]);
    }
    printf("\n");
    
    long* sort = radixSort(n,x,10,3);
    
    for (i = 0; i < n; i++) {
        printf("%ld ",sort[i]);
    }
    printf("\n");
}
