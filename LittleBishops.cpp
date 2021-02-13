#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

// how many ways can you place b bishops on an nxn board that aren't attacking?
long LittleBishops(int n, int b) {
    if (n == 1)
        return b == 0 || b == 1;
    if (n == 0)
        return b == 0;
    if (n < 0)
        return 0;
    if (b > 2*n-2)
        return 0;
    // diagonals of lengths:
    // L_1 = n,n-2,n-2,n-4,n-4,... with l_1 = |L_1| = (n-1)/2*2+1
    // L_2 = n-1,n-1,n-3,n-3,... with l_2 = |L_2| = n/2*2
    // dp arrays (x=1,2): dp_x[i][j][k] =
    //    i terms at beginning of L_x
    //    j of them have bishops on them
    //    k cross-diagonals are invalid
    // dims:
    //    dp_1[0...l_1][0...l_1][0...l_1]
    //    dp_2[0...l_2][0...l_2][0...l_2]
    // dp_x[0][0][k] = 1
    // dp_x[i][j][k] = dp_x[i-1][j][k] + (diag_x(i)-k) * dp_x[i-1][j-1][k+1]
    // with
    // diag_1(i) = n-i/2*2 (L_1 is indexed from 1 to match dp formula)
    // diag_2(i) = n+1-(i+1)/2*2
    //
    // pick j bishops for L_1, b-j bishops for L_2
    // 0 <= j <= l_1, 0 <= b-j <= l_2
    // 0 <= j <= l_1, b-l_2 <= j <= b
    //
    // ans = sum(j = max(0,b-l_2) to min(l_1,b)) dp_1[l_1][j][0] * dp_2[l_2][b-j][0]
    int l_1 = (n-1)/2*2+1;
    int l_2 = n/2*2;
#define v vector
    v<v<v<long> > > dp_1(l_1+1,v<v<long> >(l_1+1,v<long>(l_1+1,0)));
    v<v<v<long> > > dp_2(l_2+1,v<v<long> >(l_2+1,v<long>(l_2+1,0)));
#undef v
    for (int k = 0; k <= l_1; ++k)
        dp_1[0][0][k] = 1;
    for (int k = 0; k <= l_2; ++k)
        dp_2[0][0][k] = 1;
    for (int i = 1; i <= l_1; ++i) {
        int diag_1 = n-i/2*2;
        for (int j = 0; j <= l_1; ++j)
            for (int k = 0; k <= l_1; ++k) {
                dp_1[i][j][k] = dp_1[i-1][j][k];
                if (j > 0 && diag_1 > k && k < l_1)
                    dp_1[i][j][k] += (diag_1-k) * dp_1[i-1][j-1][k+1];
            }
    }
    for (int i = 1; i <= l_2; ++i) {
        int diag_2 = n+1-(i+1)/2*2;
        for (int j = 0; j <= l_2; ++j)
            for (int k = 0; k <= l_2; ++k) {
                dp_2[i][j][k] = dp_2[i-1][j][k];
                if (j > 0 && diag_2 > k && k < l_2)
                    dp_2[i][j][k] += (diag_2-k) * dp_2[i-1][j-1][k+1];
            }
    }
    long ans = 0;
    for (int j = max(0,b-l_2); j <= min(l_1,b); ++j)
        ans += dp_1[l_1][j][0] * dp_2[l_2][b-j][0];
    return ans;
}

int main() {
    cout << LittleBishops(5,8) << endl; // 32
    cout << LittleBishops(8,6) << endl; // 5599888
    cout << LittleBishops(4,4) << endl; // 260
    return 0;
}
