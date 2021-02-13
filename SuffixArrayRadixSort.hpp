//
//  SuffixArrayRadixSort.h
//  
//
//  Created by David Fink on 3/23/16.
//
//

#ifndef _SuffixArrayRadixSort_h
#define _SuffixArrayRadixSort_h

// all values in x[size] are in [0,n^pow)
// operates in O(pow * (size + n))
// sorts x in place, and generates a table of the original indices
// so the values at those indices are sorted
// (basiclaly it just saves each index with each value)
// The table of indices is the return value
int* radixSort(int size, int* x, int n, int pow);

#endif
