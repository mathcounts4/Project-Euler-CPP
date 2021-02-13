package myMath;

import java.util.ArrayList;
import java.util.Arrays;

public class FiveByFivePuzzle {

	// http://www.tilepuzzles.com/default.asp?p=6
	
	static int[] positions;
	static int minMoves;
	static Node root;
	static ArrayList<Integer> distances;
	static int max;
	static long start;
	
	public static void main(String[] args) {
		start = System.nanoTime();
		
	//	System.out.println(distance(positions));
		initialize();
		root = new Node(positions);
		int moves = 0;
		while (true) {
			distances = new ArrayList<Integer>();
			makeMoves(root,moves);
			Integer[] x = new Integer[0];
			x = distances.toArray(x);
			Arrays.sort(x);
			System.out.println(x[0] + " is smallest distance");
			
			int n = 68; // 68 is fastest for 75-move solution. Tried with higher (longer) and lower (unsuccessful) values
			
			if (distances.size() <= n) {
				max = 100000;
			}
			else {
				max = x[n];
			}
			moves++;
			System.out.println("finished " + moves + " moves");
			System.out.println(root.totalChildren() + " total children");
		}
	}
	
	public static void makeMoves(Node x, int moves) {
		if (x.moves() < moves-1) {
			for (int i = x.children.size()-1; i != -1; i--) {
				makeMoves(x.children.get(i),moves);
			}
		}
		else if (x.moves() == moves-1) {
			for (int i = x.children.size()-1; i != -1; i--) {
				if (distance(x.children.get(i).data) > max) {
					continue;
				}
				makeMoves(x.children.get(i),moves);
			}
		}
		else {
			int[] locations = Arrays.copyOf(x.data,25);
			int[] locationsCopy = Arrays.copyOf(locations,25);
			int i = 0;
			while (locations[i] != 24) {
				i++;
			}
			if (i%5 == 0) {
				locations[i] = locations[i+1];
				locations[i+1] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i+1] = locations[i+2];
				locations[i+2] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i+2] = locations[i+3];
				locations[i+3] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i+3] = locations[i+4];
				locations[i+4] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
			}
			else if (i%5 == 1) {
				locations[i] = locations[i-1];
				locations[i-1] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i-1] = locations[i];
				locations[i] = locations[i+1];
				locations[i+1] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i+1] = locations[i+2];
				locations[i+2] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i+2] = locations[i+3];
				locations[i+3] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
			}
			else if (i%5 == 2) {
				locations[i] = locations[i-1];
				locations[i-1] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i-1] = locations[i-2];
				locations[i-2] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i-2] = locations[i-1];
				locations[i-1] = locations[i];
				locations[i] = locations[i+1];
				locations[i+1] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i+1] = locations[i+2];
				locations[i+2] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
			}
			else if (i%5 == 3) {
				locations[i] = locations[i+1];
				locations[i+1] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i+1] = locations[i];
				locations[i] = locations[i-1];
				locations[i-1] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i-1] = locations[i-2];
				locations[i-2] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i-2] = locations[i-3];
				locations[i-3] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
			}
			else if (i%5 == 4) {
				locations[i] = locations[i-1];
				locations[i-1] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i-1] = locations[i-2];
				locations[i-2] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i-2] = locations[i-3];
				locations[i-3] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i-3] = locations[i-4];
				locations[i-4] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
			}
			
			locations = locationsCopy;
			if (i/5 == 0) {
				locations[i] = locations[i+5];
				locations[i+5] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i+5] = locations[i+10];
				locations[i+10] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i+10] = locations[i+15];
				locations[i+15] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i+15] = locations[i+20];
				locations[i+20] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
			}
			else if (i/5 == 1) {
				locations[i] = locations[i-5];
				locations[i-5] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i-5] = locations[i];
				locations[i] = locations[i+5];
				locations[i+5] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i+5] = locations[i+10];
				locations[i+10] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i+10] = locations[i+15];
				locations[i+15] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
			}
			else if (i/5 == 2) {
				locations[i] = locations[i-5];
				locations[i-5] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i-5] = locations[i-10];
				locations[i-10] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i-10] = locations[i-5];
				locations[i-5] = locations[i];
				locations[i] = locations[i+5];
				locations[i+5] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i+5] = locations[i+10];
				locations[i+10] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
			}
			else if (i/5 == 3) {
				locations[i] = locations[i+5];
				locations[i+5] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i+5] = locations[i];
				locations[i] = locations[i-5];
				locations[i-5] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i-5] = locations[i-10];
				locations[i-10] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i-10] = locations[i-15];
				locations[i-15] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
			}
			else if (i/5 == 4) {
				locations[i] = locations[i-5];
				locations[i-5] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i-5] = locations[i-10];
				locations[i-10] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i-10] = locations[i-15];
				locations[i-15] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
				locations[i-15] = locations[i-20];
				locations[i-20] = 24;
				if (!root.matches(locations)) {
					new Node(x,locations);
				}
			}
		}
	}
	
	public static int distance(int[] board) {
		int total = 0;
		int square;
		for (int i = 0; i != 25; i++) {
			if ((square = board[i]) == 24) {
				continue;
			}
			if (square/5 > i/5) {
				total += (square/5) - (i/5);
			}
			else {
				total += (i/5) - (square/5);
			}
			if (square%5 > i%5) {
				total += (square%5) - (i%5);
			}
			else {
				total += (i%5) - (square%5);
			}
		}
		return total;
	}
	
	public static void initialize() {
		positions = new int[25];
		positions[0] = 15;
		positions[1] = 18;
		positions[2] = 14;
		positions[3] = 17;
		positions[4] = 4;
		positions[5] = 22;
		positions[6] = 3;
		positions[7] = 9;
		positions[8] = 13;
		positions[9] = 7;
		positions[10] = 12;
		positions[11] = 23;
		positions[12] = 1;
		positions[13] = 2;
		positions[14] = 8;
		positions[15] = 21;
		positions[16] = 6;
		positions[17] = 11;
		positions[18] = 16;
		positions[19] = 19;
		positions[20] = 0;
		positions[21] = 10;
		positions[22] = 5;
		positions[23] = 20;
		positions[24] = 24;
	}
	
	private static class Node {
		private ArrayList<Node> children;
		private Node parent;
		private int[] data;
		private int moves;
		
		public Node(int[] rootData) {
			parent = null;
			data = Arrays.copyOf(rootData,25);
			children = new ArrayList<Node>();
			moves = 0;
		}
		
		public Node(Node parent, int[] childData) {
			int dist = distance(childData);
			if (dist == 0) {
				System.out.println("hooray");
				for (int i = 0; i != 25; i++) {
					System.out.printf("%3s","" + childData[i]);
					if (i%5 == 4) {
						System.out.println();
					}
				}
				System.out.println();
				for (int i = 0; i != 25; i++) {
					System.out.printf("%3s","" + parent.data[i]);
					if (i%5 == 4) {
						System.out.println();
					}
				}
				System.out.println();
				while (parent.parent != null) {
					parent = parent.parent;
					for (int i = 0; i != 25; i++) {
						System.out.printf("%3s","" + parent.data[i]);
						if (i%5 == 4) {
							System.out.println();
						}
					}
					System.out.println();
				}
				long end = System.nanoTime();
				double time = (end-start)/1000000000.0;
				System.out.println(time + " seconds");
				System.exit(0);
			}
			
			this.parent = parent;
			data = Arrays.copyOf(childData,25);
			moves = parent.moves + 1;
			children = new ArrayList<Node>();
			parent.add(this);
			distances.add(distance(data));
		}
		
		public void add(Node child) {
			children.add(child);
		}
		
		public boolean matches(int[] someData) {
			if (someData[0] == data[0] && someData[1] == data[1] && someData[2] == data[2] && someData[3] == data[3] &&
					someData[4] == data[4] && someData[5] == data[5] && someData[6] == data[6] && someData[7] == data[7] &&
					someData[8] == data[8] && someData[9] == data[9] && someData[10] == data[10] && someData[11] == data[11] &&
					someData[12] == data[12] && someData[13] == data[13] && someData[14] == data[14] && someData[15] == data[15] &&
					someData[16] == data[16] && someData[17] == data[17] && someData[18] == data[18] && someData[19] == data[19] &&
					someData[20] == data[20] && someData[21] == data[21] && someData[22] == data[22] && someData[23] == data[23] &&
					someData[24] == data[24]) {
				return true;
			}
			else {
				for (int i = children.size()-1; i != -1; i--) {
					if (children.get(i).matches(someData)) {
						return true;
					}
				}
			}
			return false;
		}
		
		public int moves() {
			return moves;
		}
		
		public int totalChildren() {
			if (children.size() == 0) {
				return 0;
			}
			else {
				int total = children.size();
				for (int i = total-1; i != -1; i--) {
					total += children.get(i).totalChildren();
				}
				return total;
			}
		}
	}
}