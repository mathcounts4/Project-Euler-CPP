package myMath;

public class Choose {
	public static void main(String[] args) {
		for (int i = 0; i<11; i++) {
			for (int j = 0; j<11; j++) {
				System.out.printf("%5d",(int)choose(i,j));
			}
			System.out.println();
		}
	}
	
	public static double choose(int a, int b) {
		if (a<b || a<0 || b<0) {
			return 0;
		}
		else if (a==0) {
			return 1;
		}
		else {
			double[] line = {1,1};
			while (a!=1) {
				line = nextLine(line);
				a--;
			}
			return line[b];
		}
	}
	public static double[] nextLine(double[] a) {
		int x = a.length;
		double[] b = new double[x+1];
		b[0] = 1;
		b[x] = 1;
		for (int i = 1; i<x/2+1; i++) {
			b[i] = a[i-1]+a[i];
			b[x-i] = b[i];
		}
	//	for (double element : b) {
	//		System.out.print(element + " ");
	//	}
		return b;
	}
}
