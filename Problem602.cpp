/*

Product of Head Counts

Alice enlists the help of some friends to generate a random number, using a single unfair coin. She and her friends sit around a table and, starting with Alice, they take it in turns to toss the coin. Everyone keeps a count of how many heads they obtain individually. The process ends as soon as Alice obtains a Head. At this point, Alice multiplies all her friends' Head counts together to obtain her random number.

As an illustration, suppose Alice is assisted by Bob, Charlie, and Dawn, who are seated round the table in that order, and that they obtain the sequence of Head/Tail outcomes THHH—TTTT—THHT—H beginning and ending with Alice. Then Bob and Charlie each obtain 2 heads, and Dawn obtains 1 head. Alice's random number is therefore 2×2×1=42×2×1=4.

Define e(n,p) to be the expected value of Alice's random number, where n is the number of friends helping (excluding Alice herself), and p is the probability of the coin coming up Tails.

It turns out that, for any fixed nn, e(n,p)e(n,p) is always a polynomial in p. For example, e(3,p)=p^3+4p^2+p.

Define c(n,k) to be the coefficient of p^k in the polynomial e(n,p). So c(3,1)=1, c(3,2)=4, and c(3,3)=1.

You are given that c(100,40)≡986699437 (mod 10^9+7).

Find c(10000000,4000000) mod 10^9+7.

*/



/*

Given that you play exactly R rounds, let X be the expected value of one player = R(1-p)
With n helpers, the expected value of their product, since they’re independent, is X^n

The probability that we get exactly R rounds for the helpers is (1-p)p^R

This, summed over the rounds, gives a total expected value of:
S = sum(R=0 to inf) (1-p)p^R(R(1-p))^n = sum(R=0 to inf) (1-p)^(n+1) * R^n * p^R

coef of p^k is:
sum (R=0 to k) R^n * choose(n+1,k-R) * (-1)^(k-R)

k>n ->
sum(R=k-n-1 to k) R^n * choose(n+1,k-R) * (-1)^(k-R) =
sum(R=0 to n+1) (k-R)^n * choose(n+1,R) * (-1)^R =
sum(R=0 to n+1, S=0 to n) k^S * (-R)^(n-S) * choose(n,S) * choose(n+1,R) * (-1)^R =
sum(S=0 to n) k^S * choose(n,S) * sum(R=0 to n+1) (-R)^(n-S) * choose(n+1,R) * (-1)^R
0 <= w < z:
Q =
sum(R=0 to z) (-R)^w * choose(z,R) * (-1)^R = 
sum(R=0 to z-1) (-R)^w * choose(z-1,R) * (-1)^R + sum(R=1 to z) (-R)^w * choose(z-1,R-1) * (-1)^R =
sum(R=0 to z-1) (-R)^w * choose(z-1,R) * (-1)^R + sum(R=0 to z-1) (-R-1)^w * choose(z-1,R) * (-1)^(R+1) =
sum(R=0 to z-1) p(R) deg<w * choose(z-1,R) * (-1)^R =
…
sum(R=0 to z-w) p(R) deg<=0 * choose(z-w,R) * (-1)^R = 
const * sum(R=0 to z-w) choose(z-w,R) * (-1)^R =
const * [sum(R=0 to z-w-1) choose(z-w-1,R) * (-1)^R + sum(R=1 to z-w) choose(z-w-1,R-1) * (-1)^R] =
const * sum(R=0 to z-w-1) choose(z-w-1,R) * (-1)^R + choose(z-w-1,R) * (-1)^(R+1) =
const * sum(R=0 to z-w-1) 0 =
0

As expected, we produce 0 when k>n.

*/

#include <iostream>
#include "ModInv.c"

using namespace std;

const long mod = 1e9 + 7;

long pow(long b, long e) {
	long ans = 1;
	for (; e; e/=2) {
		if (e&1)
			ans = ans*b%mod;
		b = b*b%mod;
	}
	return ans;
}

long c(long n, long k) {
	if (k>n)
		return 0;
	long a = 0;
	long chooser = 1;
	for (long R = k; R >= 0; --R) {
		a += pow(R,n) * chooser % mod;
		chooser = chooser * -1 * (n+1+R-k) % mod * getInv(k-R+1,mod) % mod;
	}
	a %= mod;
	return a + (a<0)*mod;
}

int main() {
	long n = 1e7;
	long k = 4e6;
	cout << "c(" << n << "," << k << ") = " << c(n,k) << endl;
	return 0;
}
