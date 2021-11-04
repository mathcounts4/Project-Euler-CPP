//
//  ModInv.c
//  
//
//  Created by David Fink on 7/19/15.
//
//

#include "ModInv.h"

// finds x^(-1) mod y in [0,y) (with x,y coprime)
long getInv(long x, long y) {
    long ans = 0;
    long pprev = 1;
    long prev = 0;
    long c = y;
    long a,b;
    while ((b = x % c) != 0) {
        a = x/c;
        ans = ans * (-1) * a + pprev;
        pprev = prev;
        prev = ans;
        x = c;
        c = b;
    }
    if (ans < 0) {
        ans += y;
    }
    return ans;
}
