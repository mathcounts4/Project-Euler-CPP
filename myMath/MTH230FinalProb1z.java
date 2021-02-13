package myMath;

import java.util.ArrayList;
import java.util.Scanner;

public class MTH230FinalProb1z {

	static long pellNum;
	
	public static void main(String[] args) {
		
		Scanner s = new Scanner(System.in);
		
		while (true) {
			try {
				pellNum = Integer.parseInt(s.nextLine());
				long[] solution = findPellSolution(pellNum);
				System.out.println(solution[0] + "^2 - " + pellNum + " * " + solution[1] + "^2 = 1");
			} catch (Exception e) {
				break;
			}
		}
	}
	
	private static long[] findPellSolution(long n) {
		
		ArrayList<Long> contFraction = new ArrayList<Long>();
		
		contFraction.add((long) Math.sqrt(n));
		
		Remainder r = new Remainder(-contFraction.get(0),1,n,1);
		
		long[] ratFrac = contFracToRat(contFraction);
		
		while (ratFrac[0]*ratFrac[0]-ratFrac[1]*ratFrac[1]*pellNum != 1) {
	//		System.out.println("Frac = " + ratFrac[0] + " " + ratFrac[1]);
			Remainder inverse = inverse(r);
	//		System.out.print("r = ");
	//		r.print();
	//		System.out.print("Inverse = ");
	//		inverse.print();
			long floor = floor(inverse);
	//		System.out.println("floor = " + floor);
			contFraction.add(floor);
			inverse.a -= inverse.d*floor;
	//		System.out.print("New inverse = ");
	//		inverse.print();
			r = inverse;
			ratFrac = contFracToRat(contFraction);
	//		System.out.print("contFrac = ");
	//		print(contFraction);
	//		System.out.println("Enter anything to continue.");
	//		s.nextLine();
		}
		
		return ratFrac;
	}
	
	public static long[] contFracToRat(ArrayList<Long> x) {
		int index = x.size()-1;
		long num = x.get(index);
		index--;
		long den = 1;
		while (index >= 0) {
			long newTop = den + x.get(index)*num;
			den = num;
			num = newTop;
			index--;
		}
		long[] array = new long[2];
		array[0] = num;
		array[1] = den;
		return array;
	}
	
	public static void print(ArrayList<Long> x) {
		int sz = x.size();
		for (int i = 0; i != sz; i++) {
			System.out.print(x.get(i) + " ");
		}
		System.out.println();
	}
	
	private static class Remainder {
	// ( a + b rt(c)) /
	//       d
		
		public long a,b,c,d;
		
		public Remainder(long a, long b, long c, long d) {
			this.a = a;
			this.b = b;
			this.c = c;
			this.d = d;
		}
		
	/*	public void print() {
			System.out.println(a + " " + b + " " + c + " " + d);
		} */
	}
	
	public static Remainder inverse(Remainder r) {
		if (r.a*r.a >= r.b*r.b*r.c) {
			long a1 = r.a*r.d;
			long b1 = -r.b*r.d;
			long d1 = (r.a*r.a-r.b*r.b*r.c);
			long gcd = gcd(gcd(a1,b1),d1);
			a1 /= gcd;
			b1 /= gcd;
			d1 /= gcd;
			Remainder next = new Remainder(a1,b1,r.c,d1);
			return next;
		} else {
			long a1 = -r.a*r.d;
			long b1 = r.b*r.d;
			long d1 = (r.b*r.b*r.c-r.a*r.a);
			long gcd = gcd(gcd(a1,b1),d1);
			a1 /= gcd;
			b1 /= gcd;
			d1 /= gcd;
			Remainder next = new Remainder(a1,b1,r.c,d1);
			return next;
		}
	}
	
	public static long floor(Remainder r) {
		return (long) ((r.a+r.b*Math.sqrt(r.c))/(double) r.d);
	}
	
	public static long gcd(long x, long y) {
		if (x == 0 || y == 0) {
			return 1;
		}
		if (y%x == 0) {
			return x;
		} else {
			return gcd(y%x,x);
		}
	}
}