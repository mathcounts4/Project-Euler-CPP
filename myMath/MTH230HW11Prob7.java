package myMath;

public class MTH230HW11Prob7 {

	public static void main(String[] args) {
		double alpha = 0;
		double add;
		
		for (int i = 1; i < 9; i++) {
			add = i*i;
			for (int j = i; j != 0; j--) {
				add /= 100;
			}
			alpha += add;
		}
		
		System.out.println(alpha);
		contFrac(alpha);
	}
	
	public static void contFrac(double x) {
		System.out.print("[");
		int digits = 0;
		while (x > 0.0001 && digits < 50) {
			digits++;
			int v = (int) x;
			System.out.print(v + ",");
			x = 1/(x-v);
		}
		System.out.println("]");
	}
}