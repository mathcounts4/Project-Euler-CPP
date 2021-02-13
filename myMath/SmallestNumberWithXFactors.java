package myMath;

import java.math.BigInteger;

public class SmallestNumberWithXFactors {

	public static BigInteger smallestNumberWithXFactors(int x) {

		int rowNumber = -1;
		double min = Double.MAX_VALUE, sum = 0;
		
		int[][] array = getFactors(x,2);
		
		int[] prime = makeNPrimes(array[0].length);
			
		for (int row = 0; row != array.length; row++) {
			sum = 0;
			int length = array[row].length;
			for (int i = 0; i!=length; i++) {
				sum += (array[row][(length-i-1)]-1) * Math.log(prime[i]);
			}
			if (sum < min) {
				min = sum;
				rowNumber = row;
			}
		}
		
		BigInteger product = BigInteger.ONE;
		int j = 0;
		for (int i = array[rowNumber].length-1; i!=-1; i--) {
			BigInteger pow = BigInteger.valueOf(prime[j]).pow(array[rowNumber][i]-1);
			product = product.multiply(pow);
			j++;
		}
		return product;
	}
	
	public static int[][] getFactors(int product, int minFactor) {
		int a = product, x = minFactor;
		if (x*x>a) {
			int[][] array = {{a}};
			return array;
		}
		else if ((a/x)*x!=a) {
			return getFactors(a, x+1);
		}
		else {
			int[][] temp1 = getFactors(a/x, x);
			int length1 = temp1.length;
			int[][] temp2 = getFactors(a, x+1);
			int length2 = temp2.length;
			
			int[][] array = new int[length1+length2][];
			
			for (int i = 0; i<length1; i++) {
				array[i] = new int [temp1[i].length+1];
				array[i][0] = x;
				for (int j = 1; j<array[i].length; j++) {
					array[i][j] = temp1[i][j-1];
				}
			}
			
			for (int i = 0; i<length2; i++) {
				array[i+length1] = new int[temp2[i].length];
				for (int j = 0; j<temp2[i].length; j++) {
					array[i+length1][j] = temp2[i][j];
				}
			}
			
			return array;
		}
	}
	
	public static int[] makeNPrimes(int n) {
		if (n < 2) {
			n = 2;
		}
		int[] tempPrimes = new int[n];
		tempPrimes[0] = 2;
		tempPrimes[1] = 3;
		int x = 2;
		int p = 5;
		boolean prime = true;
		while (x<n) {
			prime = true;
			for (int y = 0; tempPrimes[y]<=Math.sqrt(p); y++) {
				if (p%tempPrimes[y]==0) {
					prime = false;
					break;
				}
			}
			if (prime) {
				tempPrimes[x] = p;
				x++;
			}
			p+=2;
		}
		int[] array = new int[x];
		for (int i = 0; i<x; i++) {
			array[i] = tempPrimes[i];
		}
		return array;
	}
}