package myMath;

public class PrimeFac {
	
	public static int[][] primeFac(long x) {
		if (x<2) {
			int[][] a = {{1,1}};
			return a;
		}
		int[] primes = FindPrime.primesUntilPlusOne((int)Math.ceil(Math.sqrt(x)));
		int[] howMany = new int[primes.length];
		for (int i = howMany.length-1; i != -1; i--) {
			howMany[i] = 0;
		}
		int index = 0;
		int numPrimes = 0;
		
		while (primes[index]*primes[index] <= x) {
			if (x%primes[index] == 0) {
				numPrimes++;
				while (x%primes[index] == 0) {
					howMany[index]++;
					x /= primes[index];
				}
			}
			index++;
		}
		if (x != 1) {
			numPrimes++;
		}
		int[][] result = new int[numPrimes][2];
		int counter = 0;
		index = 0;
		while (index < howMany.length) {
			if (howMany[index] != 0) {
				result[counter][0] = primes[index];
				result[counter][1] = howMany[index];
				counter++;
			}
			index++;
		}
		if (counter == numPrimes-1) {
			result[counter][0] = (int)x;
			result[counter][1] = 1;
		}
		return result;
	}
	
	public static String toString(long x) {
		int[][] array = primeFac(x);
		String s = "";
		int length = array.length;
		for (int i = 0; i<length-1; i++) {
			s += array[i][0] + "^" + array[i][1] + " * ";
		}
		s += array[length-1][0] + "^" + array[length-1][1];
		return s;
	}
}