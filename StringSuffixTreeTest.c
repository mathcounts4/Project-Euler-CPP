//
//  QTest.c
//  
//
//  Created by David Fink on 3/29/16.
//
//

#include <stdio.h>

int main() {
    char* T = "abacaba";
    int S[7] = {6,4,0,2,5,1,3};
    // a
    // aba
    // abacaba
    // acaba
    // ba
    // bacaba
    // caba
    int S_inv[7];
    int i;
    for (i = 0; i < 7; i++) {
        S_inv[S[i]] = i;
    }
    
    int Q[6]; // Should obtain {1,3,3,0,2,0}
    int h = 0;
    for (i = 0; i < 7; i++) {
        if (S_inv[i] > 0) {
            int l = S[S_inv[i]-1];
            while (T[i+h] == T[l+h]) {
                h++;
            }
            Q[S_inv[i]-1] = h;
            h--;
        }
    }
    
    for (i = 0; i < 7; i++) {
        printf("%d ",Q[i]);
    }
    printf("\n");
}