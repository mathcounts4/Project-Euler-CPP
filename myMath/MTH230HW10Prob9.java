package myMath;

public class MTH230HW10Prob9 {

	static boolean[] primes;
	
	public static void main(String[] args) {
		
		primes = new boolean[1002];
		for (int i = 2; i != 1002; i++) {
			primes[i] = true;
		}
		
		for (int i = 2; i != 1002; i++) {
			if (primes[i]) {
				for (int j = 2*i; j < 1002; j += i) {
					primes[j] = false;
				}
			}
		}
		
		compute(10);
		compute2(10);
		compute(100);
		compute2(100);
		compute(1000);
		compute2(1000);
		compute(10000);
		compute2(10000);
		compute(100000);
		compute2(100000);
		compute(1000000);
		compute2(1000000);
		compute2(10000000);
		compute2(100000000);
		compute2(1000000000);
		compute2(10000000000L);
		compute2(100000000000L);
		compute2(1000000000000L);
		compute2(10000000000000L);
		compute2(100000000000000L);
		compute2(1000000000000000L);
		compute2(10000000000000000L);
		compute2(100000000000000000L);
		compute2(1000000000000000000L);
	}
	
	public static void compute(int n) {
		long total = 1; // since 0 = 0^2 = 0^2
		for (int i = 1; i <= n; i++) {
			total += function(i);
		}
		double result = total / (double) n;
		System.out.println(n + "\t" + result);
	}
	
	public static void compute2(long n) {
		long total = 0;
		for (long i = (long) Math.sqrt(n); i != 0; i--) {
			total += (long) Math.sqrt(n-i*i)+1;
		}
		total *= 4;
		total++;
		System.out.println(n + "\t" + (total/(double) n));
	}
	
	public static long function(int n) {
		
		while (n%2 == 0) {
			n /= 2;
		}
		
		long result = 4;
		int power = 0;
		
		for (int i = 3; i != 1002 && n != 1; i++) {
			if (primes[i]) {
				power = 0;
				while (n%i == 0) {
					n /= i;
					power++;
				}
				if (i%4 == 3) {
					if (power%2 == 1) {
						return 0;
					}
				} else {
					result *= (power+1);
				}
			}
		}
		
		if (n != 1) {
			if (n%4 == 3) {
				return 0;
			} else {
				result *= 2;
			}
		}
		
		return result;
		
	}
}