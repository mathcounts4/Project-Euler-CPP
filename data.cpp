#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    int n = 20;
    vector<int> primes{2,3,5,7,11,13,17,19};
    vector<vector<int> > pascal(n+1);
    for (int i = 0; i <= n; ++i) {
	pascal[i].push_back(1);
	for (int j = 1; j < i; ++j)
	    pascal[i].push_back(pascal[i-1][j-1]+pascal[i-1][j]);
	if (i > 0)
	    pascal[i].push_back(1);
    }
    for (int i = 0; i <= n; ++i)
	cout << setw(8) << i;
    cout << endl;
    for (int p_i = 0; p_i < primes.size(); ++p_i) {
	for (int i = 0; i <= n; ++i)
	{
	    int total = 0;
	    for (int x : pascal[i])
	    {
		int res = 1;
		for (int p_j = 0; p_j <= p_i; ++p_j)
		{
		    int p = primes[p_j];
		    while (x%p == 0)
		    {
			x /= p;
			res *= p;
		    }
		}
		total += res;
	    }
	    cout << setw(8) << total;
	}
	cout << endl;
    }
}
