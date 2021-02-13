//
//  SuffixArray.c
//  
//
//  Created by David Fink on 3/22/16.
//
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SuffixArrayRadixSort.h"

int* suffixArray(char* str);
int* suffixIntArray(int* str, int n);
void reduce(int* str, int n);

int main() {
    char input[101];
    int i;
    for (i = 0; i < 101; i++) {
        input[i] = '\0';
    }
    scanf("%100[^\n]",input);
    
    printf("string (chars): ");
    int len = strlen(input);
    for (i = 0; i < len; i++) {
        printf(" %c ",input[i]);
    }
    printf("\n");
    
    int* x = suffixArray(input);
    
    int location[len];
    
    for (i = 0; i < len; i++) {
        location[x[i]] = i;
    }
    
    printf("Suffix ranks:   ");
    for (i = 0; i < len; i++) {
        printf("%2d ",location[i]);
    }
    printf("\n");
    
    printf("Suffix array:   ");
    for (i = 0; i < len; i++) {
        printf("%2d ",x[i]);
    }
    printf("\n");
}

int* suffixArray(char* str) {
    
    int len = strlen(str);
    int text[len];
    
    int i;
    for (i = 0; i < len; i++) {
        text[i] = str[i];
    }
    
    return suffixIntArray(text,len);
}

// returns an array x[0...n-1] from str[0...n-1]
// such that the suffixes of str str[i...n-1] are lexicographically ordered by
// x[i], where the values in x are 0,...,n-1
int* suffixIntArray(int* str, int n) {
    
    int i;
    
    if (n == 2) {
        int* ans = malloc(2 * sizeof(int));
        if (str[0] < str[1]) {
            ans[0] = 0;
            ans[1] = 1;
        } else {
            ans[0] = 1;
            ans[1] = 0;
        }
        return ans;
    }
    
    reduce(str,n);
    
    int max = str[0];
    for (i = 1; i < n; i++) {
        if (str[i] > max) {
            max = str[i];
        }
    }
    max++;
    // now max is greater than any element of str
    
    if (max == n) {
        // we have n distinct integers now, so just return the suffixes of str sorted by first char
        int* ans = malloc(n * sizeof(int));
        for (i = 0; i < n; i++) {
            ans[str[i]] = i;
        }
        return ans;
    }
    
    
    
    // compute relative ordering for suffixes starting at indices = 1,2 mod 3
    // convert to an alphabet with 3 old characters = 1 new character
    // find ordering of the suffixes of str[1...n-1]$$$str[2...n-1] (grouped by 3 letters)
    int numMod1 = (n+1)/3;
    int numMod2 = n/3;
    int recursive[numMod1 + 1 + numMod2];
    
    for (i = 0; i < numMod1; i++) {
        // i -> 3i+1
        int index = 3*i + 1;
        recursive[i] = max * max * str[index];
        if (index + 1 < n) {
            recursive[i] += max * str[index+1];
            if (index + 2 < n) {
                recursive[i] += str[index+2];
            }
        }
    }
    
    recursive[numMod1] = -1;
    // space buffer between the sections:
    // str[1...n-1]$$$str[2...n-1]
    // so that a substring starting near the end with an index = 1 mod 3
    // will get a lower substring score when compared to others
    // that start with the same characters
    
    for (i = 0; i < numMod2; i++) {
        // i -> 3i+2
        int index = 3*i + 2;
        recursive[i + numMod1 + 1] = max * max * str[index];
        if (index + 1 < n) {
            recursive[i + numMod1 + 1] += max * str[index+1];
            if (index + 2 < n) {
                recursive[i + numMod1 + 1] += str[index+2];
            }
        }
    }
    
    reduce(recursive,numMod1+numMod2+1);
    
    
    // get the order of the suffixes starting at 1 or 2 mod 3
    // suffixOrder12Mod3[i] = index of the (i+1)'st lexicographical suffix of recursive
    int* suffixOrder12Mod3 = suffixIntArray(recursive,numMod1 + numMod2 + 1);
    
    // precedence[i] will store the position of the suffix recursive[i...n-1] in the sorted array
    int precedence[n+2];
    
    // helpful for when we go off the end - these have lowest precedence
    precedence[n] = -1;
    precedence[n+1] = -1;
    
    for (i = 0; i < numMod1 + numMod2 + 1; i++) {
        int one_two_index = suffixOrder12Mod3[i];
        if (one_two_index < numMod1) {
            int index = 3 * one_two_index + 1;
            precedence[index] = i;
        } else if (one_two_index > numMod1) {
            one_two_index -= (numMod1 + 1);
            int index = 3 * one_two_index + 2;
            precedence[index] = i;
        }
    }
    
    
    // order the suffixes of indices (0 mod 3) inside precedence
    int numMod0 = (n+2)/3;
    int zero_precedence[numMod0];
    for (i = 0; i < numMod0; i++) {
        zero_precedence[i] = (numMod1 + numMod2 + 1) * str[3*i];
        if (3*i + 1 < n) {
            zero_precedence[i] += precedence[3*i+1];
        }
    }
    
    reduce(zero_precedence,numMod0);
    // Note that we're guaranteed all distinct entries in zero_precedence,
    // since one_two_precedence contains distinct values mod (numMod1 + numMod2 + 1)
    
    int suffixOrder0Mod3[numMod0];
    for (i = 0; i < numMod0; i++) {
        suffixOrder0Mod3[zero_precedence[i]] = i;
    }
    
    
    // now we have two lists sorted (1,2 mod 3) (0 mod 3): suffixOrder0Mod3 and suffixOrder12Mod3
    // merge them using linear comparison time (check either 1 or 2 characters then a previous order from the (1,2 mod 3) ordering
    
    int zero_index = 0;
    int one_two_index = 0;
    int bigIndex = 0;
    
    int* ans = malloc(n * sizeof(int));
    
    while (bigIndex < n) {
        if (zero_index == numMod0) {
            int rec_index = suffixOrder12Mod3[one_two_index];
            if (rec_index < numMod1) {
                // i -> 3i+1
                int index = 3 * rec_index + 1;
                ans[bigIndex] = index;
            } else if (rec_index > numMod1) {
                // i -> 3i+2
                rec_index -= (numMod1 + 1);
                int index = 3 * rec_index + 2;
                ans[bigIndex] = index;
            } else {
                // ignore the extra -1 we added
                bigIndex--;
            }
            one_two_index++;
        } else if (one_two_index == numMod1 + numMod2 + 1) {
            int suffix_index = suffixOrder0Mod3[zero_index];
            // i -> 3i
            int index = 3 * suffix_index;
            ans[bigIndex] = index;
            zero_index++;
        } else {
            // indices of current suffixes in their own arrays
            int rec_index = suffixOrder12Mod3[one_two_index];
            int other_index = suffixOrder0Mod3[zero_index];
            
            // modify them to be the suffix index in str
            if (rec_index < numMod1) {
                rec_index = 3 * rec_index + 1;
            } else if (rec_index > numMod1) {
                rec_index -= (numMod1 + 1);
                rec_index = 3 * rec_index + 2;
            } else {
                // ignore the extra -1 we added
                one_two_index++;
                continue;
            }
            other_index = 3 * other_index;
            
            if (str[rec_index] < str[other_index]) {
                ans[bigIndex] = rec_index;
                one_two_index++;
            } else if (str[rec_index] > str[other_index]) {
                ans[bigIndex] = other_index;
                zero_index++;
            } else {
                if (rec_index % 3 == 1) {
                    if (precedence[rec_index + 1] < precedence[other_index + 1]) {
                        ans[bigIndex] = rec_index;
                        one_two_index++;
                    } else if (precedence[rec_index + 1] > precedence[other_index + 1]) {
                        ans[bigIndex] = other_index;
                        zero_index++;
                    } else {
                        // if precedence was set up properly, this should never be reached
                        printf("MISTAKE1 %d %d\n",rec_index,other_index);
                    }
                } else {
                    if (rec_index == n-1 || str[rec_index + 1] < str[other_index + 1]) {
                        ans[bigIndex] = rec_index;
                        one_two_index++;
                    } else if (other_index == n-1 || str[rec_index + 1] > str[other_index + 1]) {
                        ans[bigIndex] = other_index;
                        zero_index++;
                    } else {
                        if (precedence[rec_index + 2] < precedence[other_index + 2]) {
                            ans[bigIndex] = rec_index;
                            one_two_index++;
                        } else if (precedence[rec_index + 2] > precedence[other_index + 2]) {
                            ans[bigIndex] = other_index;
                            zero_index++;
                        } else {
                            // if precedence was set up properly, this should never be reached
                            printf("MISTAKE2 %d %d\n",rec_index,other_index);
                        }
                    }
                }
            }
            
        }
        bigIndex++;
    }
    
    
    free(suffixOrder12Mod3);
    
    
    return ans;
}

// This method preserves order of the numbers in str, but makes them smaller (starting from 0)
// Operates in O(n)
void reduce(int* str, int n) {
    
    int max = str[0];
    int min = str[0];
    int i;
    for (i = 0; i < n; i++) {
        if (str[i] > max) {
            max = str[i];
        } else if (str[i] < min) {
            min = str[i];
        }
    }
    
    for (i = 0; i < n; i++) {
        str[i] -= min;
    }
    max -= min;
    
    int base = 16;
    int exp = 0;
    while (max > 0) {
        max /= base;
        exp++;
    }
    
    int* indices = radixSort(n, str, base, exp);
    // indices[i] = index of (i+1)st smallest element in str
    // str is now sorted
    
    // Then copy is also sorted
    int copy[n];
    for (i = 0; i < n; i++) {
        copy[i] = str[i];
    }
    
    // this sets str[i] = position of unsorted str[i] in the sorted str array
    int val = 0;
    int last = copy[0];
    str[indices[0]] = val;
    for (i = 1; i < n; i++) {
        if (last < copy[i]) {
            // have a new value
            val++;
            last = copy[i];
        }
        str[indices[i]] = val;
    }
    
    free(indices);
    
    return;
}
