package myMath;

public class MyMath {

	public static int gcd(int a, int b) {
		int c = 0;
		while (true) {
			if (a>b) {
				c = b;
				b = a;
				a = c;
			}
			if (b%a==0) {
				c = a;
				break;
			}
			b = b%a;
		}
		return c;
	}
	public static int lcm (int a, int b) {
		int lcm = a*b/gcd(a, b);
		return lcm;
	}
}