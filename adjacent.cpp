#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

// this code solves the problem:
// Given a list of distinct (item,category),
// find the number of orderings of the items such that
// no two items of the same category are adjacent

// compile using:
// g++ -std=c++14 -o ADJACENT adjacent.cpp

class big_int {
  private:
    static const long mod = 1e8;
    std::vector<long> vals;
    
  public:
    big_int() {}
    big_int(long x) {
        while (x) {
            vals.push_back(x % mod);
            x /= mod;
        }
    }

    big_int& operator+=(const big_int& o) {
        long c = 0;
        for (int i = 0; i < o.vals.size() || c; ++i) {
            if (i < o.vals.size())
                c += o.vals[i];
            if (i < vals.size()) {
                c += vals[i];
            } else {
                vals.push_back(0);
            }
            vals[i] = c % mod;
            c /= mod;
        }
        return *this;
    }

    big_int operator+(const big_int& o) const {
        big_int ans = *this;
        return ans += o;
    }

    big_int operator*(const big_int& o) const {
        big_int ans = 0;
        long c = 0;
        for (int i = 0; i < int(o.vals.size())+int(vals.size())-1 || c; ++i) {
            for (int j = std::max<int>(0,i-int(vals.size())+1); j <= std::min<int>(i,int(o.vals.size())-1); ++j) {
                c += o.vals[j] * vals[i-j];
            }
            ans.vals.push_back(c % mod);
            c /= mod;
        }
        return ans;
    }

    std::string toString() const {
        std::string a;
        if (vals.size() == 0)
            a += "0";
        for (int i = vals.size(); i--;) {
            std::string tmp = std::to_string(vals[i]);
            a += std::string(i==vals.size()-1?0:8-tmp.size(),'0') + tmp;
        }
        return a;
    }
};

std::ostream& operator<<(std::ostream& s, const big_int& x) {
    return s << x.toString();
}

class x {
    // Total runtime is O(M^3), where there are M items in the list

// loop over x = 0 (inclusive) to v (exclusive) in reverse order
#define L(x,v) for(x = v; x--;)

/* m = counts of each type of magical nature (identified by first character)
 * 
 * We group the items by magical nature and then process their counts.
 * 
 * y = 1 + (# of items processed so far)
 * 
 * f[x] = x factorial
 * 
 * c[x][y] = x choose y
 * 
 * p[x][y] = possibilities for first x groups to have y conflicts,
 *           where a conflict is two adjacent items that have the same type
 * 
 * k, b, n, and r are indices, though they take on specific values later
 */
public:
long m['  ']{},y,k,b,n,r,M;
big_int magicRecipeOfProtection(const std::vector<std::vector<std::string> >& I) {
    
    // magical nature is (in all tests) uniquely identified by its first character
    for (auto& z : I)
        ++m[z[y = 1][0]], ++M;
    ++M;
    std::vector<big_int> f(M);
    std::vector<std::vector<big_int> > c(M,std::vector<big_int>(M));
    std::vector<std::vector<big_int> > p(M,std::vector<big_int>(M));

    f[0] = c[0][0] = p[0][0] = big_int(1);

    // fill out the f and c arrays
    for (; ++b < M; f[b] = f[b-1]*b)
        L(n,b+1)
            c[b][n] = !n ? big_int(1) : c[b-1][n-1] + c[b-1][n];
    
    /* DP with index k (# of types processed) in array p
     * 
     * b = # of old conflicts
     * y = 1 + old total number of items
     * n = # of new conflicts
     * r = # of resolved conflicts
     * x = # of items of current type
     * 
     * First we pick r of the old conflicts to fix = c[b][r]
     * 
     * Since we have n new conflicts, we will have x-n new runs of this magical type
     * We already chose r of them, so we have x-n-r left to place (that don't resolve conflicts)
     * There were y locations in total to place stuff, but we can't use conflict spots (b)
     * So from the remaining y-b locations, we choose x-n-r of them = c[y-b][x-n-r]
     * 
     * Last, we must pick where the duplicates for new conflicts go.
     * We have x-n total locations already, and n items
     * This is equivalent to putting n items into x-n bins,
     * which is the same as ordering n items and x-n-1 dividers = c[x-1][n]
     * 
     * This is (of course) multiplied by p[k][b], the number of ways
     * of placing the first k groups with b conflicts
     * 
     * We then multiply by f[x], since for the final result, all items are distinct,
     * and this process treats all items of the same type as indistinguishable.
     * Thus at the end we could multiply by f[x] for each count x,
     * or just do it as we go in the DP array (as shown here).
     * 
     * The new number of conflicts is (old) - (resolved) + (new) = b+n-r
     * 
     * From all the combinatorial terms we obtain a bunch of inequalities,
     * but I tested removing the checks for most of them and the result is the same.
     */

    // runtime for first 2 loops is M
    for (auto& x : m) {
        if (x)
        L(n,x)
            // runtime of third loop is M
            L(b,y)
                // runtime of fourth loop is M
                L(r,y)
r>b+n || b>y || n+r>x || 1>x ?:
                    p[k+1][b+n-r] += c[b][r]*c[y-b][x-n-r]*c[x-1][n]*p[k][b]*f[x];
        
        // update total with x
        y += x;
        
        // only increment k if x was nonzero (otherwise the loop L(n,x) had 0 iterations)
        k += !!x;
    }
    
    // number of ways of ordering all the groups with 0 conflicts
    return p[k][0];
}
};

int main() {
    std::vector<std::vector<std::string> > pass;
    std::vector<int> counts;
//    counts = {9,6,4,2,6,4,4,3,5,2,3,4,3,1,1,1,1,1,1,1,1,1,1};
//    counts = {2,2,1,1};
    for (int i = 15; i > 0; --i)
        for (int j = 0; j < 1; ++j)
            counts.push_back(i), std::cout << (i==15?"[":"") << i << (i==1?"]":",");
    std::cout << std::endl;
    std::string k = " ";
    k[0] = 1;
    for (auto& c : counts) {
        while (c--)
            pass.push_back({"",k});
        ++k[0];
    }
    auto start = std::chrono::system_clock::now();
    x computer;
    big_int ans = computer.magicRecipeOfProtection(pass);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> time = end-start;
    std::cout << ans << std::endl;
    std::cout << time.count() << " seconds" << std::endl;
    return 0;
}
