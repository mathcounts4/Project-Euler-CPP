package myMath;

import java.util.ArrayList;

public class MTH230HW2Prob1 {

	public static void main(String[] args) {
		for (int a = 1; a != 40; a++) {
			for (int b = a+1; b != 40; b++) {
				ArrayList<Integer> possibleM = getMs(b*b-a*a);
				for (int index = possibleM.size()-1; index != -1; index--) {
					int m = possibleM.get(index);
					if ((b-a)%m != 0 && (b+a)%m != 0) {
						System.out.println("a = " + a + ", b = " + b + ", m = " + m);
					}
				}
			}
		}
	}
	
	public static ArrayList<Integer> getMs(int x) {
		double rt = Math.sqrt(x);
		ArrayList<Integer> list = new ArrayList<Integer>();
		for (int i = 2; i <= rt; i++) {
			if (x%(i*i)==0) {
				list.add(i);
			}
		}
		return list;
	}
}