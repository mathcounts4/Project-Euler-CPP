package myMath;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Scanner;

public class FourByFourPuzzleTake2 {
	
	static int[] positions; // current game piece positions
	// 0  1  2  3
	// 4  5  6  7
	// 8  9  10 11
	// 12 13 14 15
	static HashMap<Long,Integer> checked; // stores all checked gameStates so none are repeated with higher move-count
	static int blankTile; // tile number
	static int maxMoves; // moves is current, maxMoves is determined once a solution is found
	static final long[] factorialValues = {0,1,2,6,24,120,720,5040,40320,362880,3628800,
		39916800,479001600,6227020800L,87178291200L,1307674368000L,20922789888000L};
	static boolean foundSolution;
	static ArrayList<Move> finalMoves; // stored in reverse order
	static MoveHeap moveHeap;
	static int repeats = 0;
	
	public static long permutationValue() {
		long total = 0;
		int[] realValues = new int[16];
		for (int i = 0; i != 16; i++) {
			realValues[i] = i;
		}
		for (int i = 0; i != 16; i++) {
			total += (realValues[positions[i]] * factorialValues[15-i]);
			for (int j = positions[i]+1; j != 16; j++) {
				realValues[j]--;
			}
		}
		return total;
	}
	
	public static void reloadState(Move m) {
		long value = m.permValue;
		
		int[] realValues = new int[16];
		for (int i = 0; i != 16; i++) {
			realValues[i] = i;
		}
		
		positions[15] = 120; // sum from 0 through 15
		for (int i = 0; i != 15; i++) {
			int index = (int) (value/factorialValues[15-i]);
			positions[i] = realValues[index];
			positions[15] -= positions[i];
			value %= factorialValues[15-i];
			for (int j = index; j != 15; j++) {
				realValues[j] = realValues[j+1];
			}
		}
	}
	
	public static int currentDistance() {
		int distance = 0;
		for (int i = 0; i != 16; i++) {
			distance += (20-i) * (abs((i/4)-(positions[i]/4)) + abs((i%4)-(positions[i]/4)));
		}
		return distance;
	}
	
	public static int abs(int x) {
		return (x<0 ? -x : x);
	}
	
	public static void main(String[] args) {
		
		boolean[] check = new boolean[16];
		positions = new int[16];
		
		Scanner s = new Scanner(System.in);
		
		System.out.println("Which is the blank space? (0-15)");
		blankTile = s.nextInt();
		
		System.out.println("What are the positions? (Separated by a space)");
		int blankPosition = -1;
		
		for (int i = 0; i != 16; i++) {
			positions[i] = s.nextInt();
			check[positions[i]] = true;
			if (positions[i] == blankTile) {
				blankPosition = i;
			}
		}
		
		s.close();
		
		for (int i = 0; i != 16; i++) {
			if (!check[i]) {
				throw new RuntimeException("Error with positions. You should have all of them, from 0 to 15, with " + blankTile + " as blank.");
			}
		}
		
		checked = new HashMap<Long,Integer>();
		foundSolution = false;
		
		Move start = new Move(9001,blankPosition,null,permutationValue(),currentValue(0),0);
		moveHeap = new MoveHeap(start);
		
		int trials = 0;
		
		Scanner scan = new Scanner(System.in);
		
		while (moveHeap.size != 0) {
			trials++;
			Move m = moveHeap.getMin();
	/*		System.out.println();
			for (Move x = m; x != null; x = x.prev) {
				reloadState(x);
				boolean[] test = new boolean[16];
				for (int i = 0; i != 16; i++) {
					System.out.printf("%3s",positions[i] + "");
					test[positions[i]] = true;
				}
				System.out.println();
				System.out.println(x.move + "\t" + x.pieceStart + " to " + x.pieceEnd);
				
				for (int i = 0; i != 16; i++) {
					if (!test[i]) {
						System.out.println(i + " not found.");
						System.out.println("perm value = " + x.permValue);
						System.exit(0);
					}
				}
			}
			
	*/		if (trials % 10 == 0) {
				System.out.println(m.move + "\t" + m.closenessValue + "\t" + trials + "\t" + repeats);
				moveHeap.printHeap();
				System.out.println("Enter anything to continue");
				scan.nextLine();
			}
			reloadState(m);
			
	//		System.out.println();
			move(m);
		}
		
		scan.close();
	}
	
	public static void move(Move prev) {
		
		int move = prev.move;
		
		if (foundSolution && move >= maxMoves) {
			return;
		}
		
		// we check if we've already gotten here (at a lower move number)
		long value = prev.permValue;
		if (checked.containsKey(value)) {
			if (checked.get(value) <= move) {
				// hashMap stores permutationValue (key) and move number (value)
				repeats++;
				return;
			} else {
				checked.put(value,move);
			}
		} else {
			checked.put(value,move);
		}
		if (value == 0) { // a solution
			System.out.println("YAY!");
			
			foundSolution = true;
			maxMoves = move;
			checked.put(value,move);
			
			finalMoves = new ArrayList<Move>();
			Move m = prev;
			while (m != null) {
				finalMoves.add(m);
				m = m.prev;
			}
			
			for (int i = finalMoves.size()-1; i >= 0; i--) {
				Move x = finalMoves.get(i);
				System.out.println(x.pieceStart + " to " + x.pieceEnd);
			}
			
			return;
		} else {
			
			// here we do the actual moving
			
			
			int space = prev.pieceStart;
			
			if (space > 3) { // up
				int temp = positions[space];
				positions[space] = positions[space-4];
				positions[space-4] = temp;
				
				long permValue = permutationValue();
				moveHeap.insert(new Move(space,space-4,prev,permValue,currentValue(move+1),move+1));
				
				temp = positions[space];
				positions[space] = positions[space-4];
				positions[space-4] = temp;
			}
			
			if (space%4 != 3) { // right
				int temp = positions[space];
				positions[space] = positions[space+1];
				positions[space+1] = temp;
				Move next = new Move(space,space+1,prev,permutationValue(),currentValue(move+1),move+1);
				moveHeap.insert(next);
				temp = positions[space];
				positions[space] = positions[space+1];
				positions[space+1] = temp;
			}
			
			if (space < 12) { // down
				int temp = positions[space];
				positions[space] = positions[space+4];
				positions[space+4] = temp;
				Move next = new Move(space,space+4,prev,permutationValue(),currentValue(move+1),move+1);
				moveHeap.insert(next);
				temp = positions[space];
				positions[space] = positions[space+4];
				positions[space+4] = temp;
			}
			
			if (space%4 != 0) { // left
				int temp = positions[space];
				positions[space] = positions[space-1];
				positions[space-1] = temp;
				Move next = new Move(space,space-1,prev,permutationValue(),currentValue(move+1),move+1);
				moveHeap.insert(next);
				temp = positions[space];
				positions[space] = positions[space-1];
				positions[space-1] = temp;
			}
		}
	}
	
	public static int currentValue(int move) {
		return (move + currentDistance());
	}
	
	private static class Move {
		public int pieceEnd, pieceStart; // piece's start = blank's end, piece's end = blank's start
		public Move prev;
		public long permValue;
		public int closenessValue;
		public int move;
		public Move(int pieceEnd, int pieceStart, Move prev, long permValue, int closenessValue, int move) { // next piece's end will be this one's start (blank)
			this.pieceEnd = pieceEnd;
			this.pieceStart = pieceStart;
			this.prev = prev;
			this.permValue = permValue;
			this.closenessValue = closenessValue;
			this.move = move;
		}
	}
	
	private static class MoveHeap {
		
		private MoveHeapNode head;
		private int size; // next free spot is determined by the size
		/*
		 *        1
		 *      2   3
		 *     4 5 6 7
		 */
		
		public MoveHeap(Move m) {
			size = 1;
			head = new MoveHeapNode(m);
		}
		
		public void printHeap() {
			printHeap(head,0);
		}
		
		public void printHeap(MoveHeapNode n, int spacing) {
			for (int i = spacing; i != 0; i--) {
				System.out.print("\t");
			}
			System.out.println(n.data.closenessValue);
			if (n.left != null) {
				printHeap(n.left, spacing+1);
			}
			if (n.right != null) {
				printHeap(n.right, spacing+1);
			}
		}
		
		public void insert(Move m) {
			if (head.data == null) {
				head.data = m;
				size++;
				return;
			}
			MoveHeapNode parent = head;
			
			int spot = size+1;
			
			boolean left = (spot/2)*2 == spot;
			spot /= 2;
			
			ArrayList<Boolean> goRight = new ArrayList<Boolean>();
			while (spot > 1) {
				goRight.add((spot/2)*2 != spot);
				spot /= 2;
			}
			
			for (int i = goRight.size()-1; i >= 0; i--) {
				if (goRight.get(i)) {
					parent = parent.right;
				} else {
					parent = parent.left;
				}
			}
			
			if (left) {
				parent.left = new MoveHeapNode(m);
				parent.left.parent = parent;
				bubbleUp(parent.left);
			} else {
				parent.right = new MoveHeapNode(m);
				parent.right.parent = parent;
				bubbleUp(parent.right);
			}
			
			size++;
		}
		
		public void bubbleUp(MoveHeapNode n) {
			if (n.parent != null && n.data.closenessValue < n.parent.data.closenessValue) {
				Move temp = n.data;
				n.data = n.parent.data;
				n.parent.data = temp;
				bubbleUp(n.parent);
			}
		}
		
		public void bubbleDown(MoveHeapNode n) {
			if (n.left != null && n.right != null && n.data.closenessValue > n.left.data.closenessValue
					&& n.data.closenessValue > n.right.data.closenessValue) {
				if (n.left.data.closenessValue < n.right.data.closenessValue) {
					Move temp = n.data;
					n.data = n.left.data;
					n.left.data = temp;
					bubbleDown(n.left);
				} else {
					Move temp = n.data;
					n.data = n.right.data;
					n.right.data = temp;
					bubbleDown(n.right);
				}
			} else if (n.left != null && n.data.closenessValue > n.left.data.closenessValue) {
				Move temp = n.data;
				n.data = n.left.data;
				n.left.data = temp;
				bubbleDown(n.left);
			} else if (n.right != null && n.data.closenessValue > n.right.data.closenessValue) {
				Move temp = n.data;
				n.data = n.right.data;
				n.right.data = temp;
				bubbleDown(n.right);
			}
		}
		
		public Move getMin() {
			int spot = size;
			MoveHeapNode item = head;
			Move temp = head.data;
			
			ArrayList<Boolean> goRight = new ArrayList<Boolean>();
			while (spot > 1) {
				goRight.add((spot/2)*2 != spot);
				spot /= 2;
			}
			
			for (int i = goRight.size()-1; i >= 0; i--) {
				if (goRight.get(i)) {
					item = item.right;
				} else {
					item = item.left;
				}
			}
			
			if (item.parent != null) {
				head.data = item.data;
				if (goRight.get(0)) {
					item.parent.right = null;
				} else {
					item.parent.left = null;
				}
				bubbleDown(head);
			} else {
				head.data = null;
			}
			
			size--;
			return temp;
		}
		
		private class MoveHeapNode {
			public Move data;
			public MoveHeapNode parent, left, right;
			
			public MoveHeapNode(Move data) {
				this.data = data;
			}
		}
	}
}