package myMath;

public class MTH230FinalProb1v {

	static int maxDenom = 0;
	
	public static void main(String[] args) {
		maxDenom = 2014;
		Node bottom = new Node();
		bottom.n = 1502;
		bottom.d = 1801;
		Node top = new Node();
		top.n = 1492;
		top.d = 1789;
		bottom.next = top;
		
		bottom.addOver();
		int count = 0;
		for (Node n = bottom; n != null && count < 5; n = n.next) {
			System.out.println(n.n + " / " + n.d);
			count++;
		}
	}
	
	public static int gcd(int a, int b) {
		if (a%b == 0) {
			return b;
		} else {
			return gcd(b,a%b);
		}
	}
	
	private static class Node {
		public int n,d;
		public Node next;
		
		public void addOver() {
			int num = n+next.n;
			int den = d+next.d;
			int gcd = gcd(num,den);
			num /= gcd;
			den /= gcd;
			if (den <= maxDenom) {
				Node insert = new Node();
				insert.n = num;
				insert.d = den;
				insert.next = next;
				next = insert;
				addOver();
				insert.addOver();
			}
		}
	}
}