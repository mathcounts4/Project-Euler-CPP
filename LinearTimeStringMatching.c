//
//  LinearTimeStringMatching.c
//  
//
//  Created by David Fink on 3/15/16.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

int max(int x, int y) {
    return (x > y ? x : y);
}

// in O(length) time, computes the array z[0...length-1]
// z[i] = length of longest prefix X of p[i...length-1] such that
// X is a prefix of p[0...length-1]
// index 0 is not used
//
// runtime analysis:
// In the main loop, each branch (and each inner loop failing) can only execute up to length times (duh)
//
// Considering both inner while loops, each one succeeding increments r (by incrementing j)
// In the first branch, since i > r, and r_new = i + j - 1, new_r >= i - 1 >= r
// In the final branch, new_r = i + j - 1, where j >= r - i + 1, so new_r >= r
// Thus those loops can succeed at most length times in total, since r starts at -1 and
// is bounded above by length, so can attain {0,1,...,length-1} at each increment
//
// Thus the outer loop and the two inner loops take a total of O(length) time.
int* z(char* p, int length) {
    
    int* z = malloc(length * sizeof(int));
    
    // we have a window p[l...r] that matches p[0...r-l], with p[r+1] != p[r-l+1]
    int l = 0;
    int r = -1;
    
    // i = current index for which we're calculating z[i]
    int i;
    
    for (i = 1; i < length; i++) {
        
        if (i > r) {
            
            // we're past the current window, so just search forward for matching characters
            
            int j = 0;
            while (i + j < length && p[i+j] == p[j]) {
                j++;
            }
            z[i] = j;
            l = i;
            r = i + j - 1;
        } else {
            // i' = i shifted to its location at the beginning of the string
            // shift [l...i...r] to
            //       /   /   /
            //      /   /   /
            //     /   /   /
            //    /   /   /
            //   /   /   /
            // [0...i'..r-l]
            //
            // where the bracketed substrings of p are the same by the rule on our window
            // note that l > 0 after the first iteration of the loop (taking the (i>r) case)
            // and subsequently l can only be set to i >= 1, so l > 0, and i' < i
            // Since on each iteration l either remains the same or is set to i,
            // on iteration i, l can be at most i-1, so i' > 0
            int i_prime = i-l;
            
            // (A) p[0...r-l] = p[l...r] and i <= r
            // (B) p[r-l+1] != p[r+1]
            // Let q = z[i']. This is computed already as 0 < i' < i
            // (C) Then p[0...q-1] = p[i'...i'+q-1]
            // (D) and p[q] != p[i'+q]
            
            if (z[i_prime] <= r-i) {
                
                // q <= r-i
                // i'+q <= r+i'-i
                // (E) i'+q <= r-l
                
                // From (A) and (E), p[0...i'+q] = p[l...l+i'+q]
                // Shortening since i' > 0, p[i'...i'+q] = p[l+i'...l+i'+q] = p[i...i+q] (since l+i'=i)
                // (F) from the previous line, p[i'...i'+q] = p[i...i+q]
                
                // Combining (C) with (F), p[0...q-1] = p[i...i+q-1]
                // Combining (D) with (F), p[q] != p[i'+q] = p[i+q], so p[q] != p[i+q]
                
                // Thus starting at i, we match exactly q characters
                // Thus z[i] = z[i']
                
                z[i] = z[i_prime];
            } else if (z[i_prime] > r-i+1) {
                
                // (G) q > r-i+1, so q-1 >= r-i+1
                
                // using (G) to shorten (C),
                // (H) p[0...r-i+1] = p[i'...i'+r-i+1] = p[i'...r-l+1]
                
                // i' > 0, so using that to shorten (A),
                // (I) p[i'...r-l] = p[l+i'...r] = p[i...r]
                
                // Combining (H) and (I),
                // (J) p[0...r-i] = p[i...r]
                
                // Also, combining (B) and (H), p[r-i+1] = p[r-l+1] != p[r+1], so
                // (K) p[r-i+1] != p[r+1]
                
                // Thus we match r-i+1 characters and no more
                z[i] = r-i+1;
            } else {
                
                // (L) q = r-i+1, so q-1 = r-i
                
                // using (L) and (C),
                // (M) p[0...r-i] = p[i'...i'+r-i] = p[i'...r-l]
                
                // i' > 0, so using that to shorten (A),
                // (N) p[i'...r-l] = p[l+i'...r] = p[i...r]
                
                // Combining (M) and (N),
                // (O) p[0...r-i] = p[i...r]
                
                // By (D) and (L), p[r-i+1] != p[i-l+r-i+1] = p[r-l+1]
                // By (B), p[r-l+1] != p[r+1]
                
                // Thus we match r-i+1 characters, but could match more
                // since the characters after satisfy:
                // p[r-i+1] != p[r-l+1] and p[r-l+1] != p[r+1]
                
                // but this says nothing about p[r-i+1] vs p[r+1]
                
                // continue checking from here
                int j = r-i+1;
                while (i + j < length && p[i+j] == p[j]) {
                    j++;
                }
                z[i] = j;
                l = i;
                r = i + j - 1;
            }
        }
    }
    
    return z;
}

// in O(length) time, computes the array sp[0...length-1]
// sp[i] = length of longest proper suffix X of p[0...i] such that
// X is a prefix of p[0...length-1]
//
// All outer loops clearly have O(length) time
// The only inner loop only increments old_right, and can only succeed length times
int* sp(char* p, int length) {
    int* sp = malloc(length*sizeof(int));
    // see z() above
    int* my_z = z(p,length);
    
    // for any suffix X of p[0...i], X looks like p[j...i] for some 1 <= j <= i
    // Then z[j] >= i-j+1
    
    // so for each i, sp[i] = i-j+1 for the smallest j satisfying 1 <= j <= i, z[j] >= i-j+1
    
    // thus j (j>=1) satisfies all i with j <= i <= z[j]+j-1
    
    // since we're looking for minimal j, we can set a right boundary
    // and move it outwards for higher j
    // (no need for a left boundary as it would always be increasing =
    // not expanding the range for i
    
    int i,j,right;
    for (i = 0; i < length; i++) {
        // set all failures to 0
        sp[i] = 0;
    }
    
    j = 1;
    while (j < length && my_z[j] == 0) {
        // locate smallest j with at least one i
        j++;
    }
    
    if (j == length) {
        // no matches to the first letter elsewhere in the string
        free(my_z);
        return sp;
    }
    
    right = my_z[j]+j-1;
    for (i = j; i <= right; i++) {
        sp[i] = i-j+1;
    }
    int old_right = right;
    j++;
    while (j < length && old_right < length-1) {
        // while we haven't computed all the values of sp
        right = my_z[j]+j-1;
        if (right > old_right) {
            for (i = max(j,old_right+1); i <= right; i++) {
                sp[i] = i-j+1;
            }
            old_right = right;
        }
        j++;
    }
    
    free(my_z);
    
    return sp;
}

// in O(length) time, computes the array sp_prime[0...length-1]
// sp_prime[i] = length of longest proper suffix X of p[0...i] such that
// X is a prefix of p[0...length-1] and p[i+1] != p[length(X)]
// (the next character after that suffix doesn't match the next starting char)
//
// pretty clear O(length) time by the loops
int* sp_prime(char* p, int length) {
    int* sp_prime = malloc(length*sizeof(int));
    // see z() above
    int* my_z = z(p,length);
    
    // for any satisfying suffix X of p[0...i], X looks like p[j...i] for some 1 <= j <= i
    // with p[i+1] != p[i-j+1]
    // Then z[j] = i-j+1
    
    // so for each i, sp_prime[i] = i-j+1 for the smallest j satisfying 1 <= j <= i, z[j] = i-j+1
    
    // thus j (j>=1) satisfies the i with j <= i = z[j]+j-1, so z[j] > 0
    
    // since we're looking for minimal j, we can set a right boundary
    // and move it outwards for higher j
    // (no need for a left boundary as it would always be increasing =
    // not expanding the range for i
    
    int i,j,right;
    for (i = 0; i < length; i++) {
        // set all failures to 0
        sp_prime[i] = 0;
    }
    
    for (j = 1; j < length; j++) {
        if (my_z[j] > 0) {
            i = my_z[j]+j-1;
            if (sp_prime[i] == 0) {
                // if two j's produce the same i, the smaller j will have the higher z[j]
                // so we need only replace values that are 0
                sp_prime[i] = my_z[j];
            }
        }
    }
    
    free(my_z);
    
    return sp_prime;
}

// finds all occurrences of p in s
// prints out p, s, and all occurrences of p in s (lined up nicely with s)
//
// runtime (ignoring printing):
// Each branch in the big while loop takes constant time (ignoring printing)
// Only the final innermost branch in the while loop doesn't increment s_index
// Consider the index Q in s of the current start of the partial match to p
// Since sp'[x] <= x, the final innermost branch decreases p_index,
// thus moving Q forward in s. (Q = s_index - p_index)
// The number of times this can excute is limited by the length of s, as
// Q can only move forward s_len times, and the other branches don't move Q backwards.
// Thus each branch is executed O(s_len) times, so since the time to find sp_prime_p is O(p_len),
// Our total runtime is O(p_len + s_len).
void find_occurrences(char* p, int p_len, char* s, int s_len) {
    int* sp_prime_p = sp_prime(p,p_len);
    
    printf("p = ");
    int i;
    for (i = 0; i < p_len; i++) {
        printf("%c",p[i]);
    }
    printf("\n");
    
    printf("s = ");
    for (i = 0; i < s_len; i++) {
        printf("%c",s[i]);
    }
    printf("\n");
    
    int p_index = 0;
    int s_index = 0;
    
    // iterate through the characters in s
    while (s_index < s_len) {
        if (p[p_index] == s[s_index]) {
            // if the characters match, increment the indices being checked
            p_index++;
            s_index++;
            if (p_index == p_len) {
                // if we finished matching the pattern, print it, and... (X)
                
                // okay, this printing messes with the runtime. I don't count it.
                printf("    ");
                int start = s_index - p_len;
                for (i = 0; i < s_len; i++) {
                    if (start <= i && i < s_index) {
                        printf("%c",s[i]);
                    } else {
                        printf("_");
                    }
                }
                printf("\n");
                
                // (X) move back the least possible in p to try to match again
                p_index = sp_prime_p[p_len-1];
            }
        } else {
            // characters didn't match
            if (p_index == 0) {
                // If we're at the beginning of p, this character in s is bad news. More on in s.
                s_index++;
            } else {
                // If we're in the middle of p, the current character in s could be right
                // as part of p, but earlier in p than in the current match.
                // so we shift back our place in p the least amount where the next character is different,
                // but the previous ones match the start of p
                p_index = sp_prime_p[p_index-1];
            }
        }
    }
    
    free(sp_prime_p);
}

double my_rand() {
    return (rand()/(double) RAND_MAX);
}

// call with two arguments, pattern and string, otherwise default sizes are used and random strings generated
int main(int argc, char** argv) {
    
    if (argc != 3) {
        // since calling it counts as the first argument, we need two more
        
        printf("For use with your own pattern and string, run this with the two arguments \"pattern\" and \"string\"\n");
        
        srand((unsigned) time(NULL));
        
        int p_len = 4;
        int s_len = 100;
        
        char* p = malloc(p_len*sizeof(char));
        int i;
        for (i = 0; i < p_len; i++) {
            if (my_rand() < 0.4) {
                p[i] = '1';
            } else {
                p[i] = '0';
            }
        }
        
        char* s = malloc(s_len*sizeof(char));
        for (i = 0; i < s_len; i++) {
            if (my_rand() < 0.4) {
                s[i] = '1';
            } else {
                s[i] = '0';
            }
        }
        
        find_occurrences(p,p_len,s,s_len);
    } else {
        find_occurrences(argv[1],strlen(argv[1]),argv[2],strlen(argv[2]));
    }
}
