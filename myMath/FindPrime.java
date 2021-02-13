package myMath;

import java.util.Scanner;

public class FindPrime {

	public static void main(String[] args) {
		// 10,000,000th prime is 179424673
		// 1,000,001th prime is 15485867
		// 10,001th prime is 104743
		Scanner scanner = new Scanner(System.in);
		System.out.println("What prime do you want?");
		int a = scanner.nextInt();
		int[] array = new int[a];
		array[0] = 2;
		array[1] = 3;
		int x = 2;
		int p = 5;
		boolean prime = true;
		while (x<a) {
			prime = true;
			for (int y = 0; array[y]<=Math.sqrt(p); y++) {
				if (p%array[y]==0) {
					prime = false;
					break;
				}
			}
			if (prime) {
				array[x] = p;
				x++;
				// System.out.println((x) + "th prime is " + array[x-1]);
			}
			p+=2;
		}
		System.out.println((x) + "th prime is " + array[x-1]);
	}
	
	public static int[] primesUntilPlusOne(int a) {
		int[] array = new int[a+1];
		array[0] = 2;
		array[1] = 3;
		int x = 2;
		int p = 5;
		boolean prime = true;
		while (p<a) {
			prime = true;
			for (int y = 0; array[y]<=Math.sqrt(p); y++) {
				if (p%array[y]==0) {
					prime = false;
					break;
				}
			}
			if (prime) {
				array[x] = p;
				x++;
				// System.out.println((x) + "th prime is " + array[x-1]);
			}
			p+=2;
		}
		
		boolean oneMore = true;
		while(oneMore) {
			prime = true;
			for (int y = 0; array[y]<=Math.sqrt(p); y++) {
				if (p%array[y]==0) {
					prime = false;
					break;
				}
			}
			if (prime) {
				array[x] = p;
				x++;
				oneMore = false;
				// System.out.println((x) + "th prime is " + array[x-1]);
			}
			p+=2;
		}
		
		int[] smaller = new int[x];
		for (int i = 0; i<x; i++) {
			smaller[i] = array[i];
		}
		return smaller;
	}
	
	public static int[] makeNprimes(int n) {
		int[] array = new int[n];
		array[0] = 2;
		array[1] = 3;
		int x = 2;
		int p = 5;
		boolean prime = true;
		while (x<n) {
			prime = true;
			for (int y = 0; array[y]<=Math.sqrt(p); y++) {
				if (p%array[y]==0) {
					prime = false;
					break;
				}
			}
			if (prime) {
				array[x] = p;
				x++;
				// System.out.println((x) + "th prime is " + array[x-1]);
			}
			p+=2;
		}
		
		return array;
	}
}