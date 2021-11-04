package myMath;

public class ProbTwoRandomIntsGCD1 { // for primes up to 5000000

	public static void main(String[] args) {
		
		// 0.6079271092786613
		
		int[] primes = FindPrime.primesUntilPlusOne(5000000);
		int l = primes.length-1;
		System.out.println(l + " primes");
		double p = 0;
		double temp = 1;
		int index = 0;
		while (temp != p && index<l) {
			p = temp;
			int prime = primes[index];
			temp *= (prime+1);
			temp *= (prime-1);
			temp /= prime;
			temp /= prime;
			index++;
		}
		System.out.println(p);
		System.out.println(index);
	}
}