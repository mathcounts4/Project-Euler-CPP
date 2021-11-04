package myMath;

public class PiCalculator {

	public static void main(String[] args) {
		double d = 2, prev = 0;
		int i  = 0;
		while (d != prev) {
			prev = d;
			d = d-Math.tan(d);
			i++;
		}
		System.out.println("pi = " + d + " in " + i + " iterations");
		System.out.println();
		double a = 6.0, b = 4*Math.sqrt(3);
		i = 0;
		while (a != b) {
			b = 2*a*b/(a+b);
			a = Math.sqrt(a*b);
			i++;
		}
		System.out.println("2pi = " + a + " in " + i + " iterations");
	}
}