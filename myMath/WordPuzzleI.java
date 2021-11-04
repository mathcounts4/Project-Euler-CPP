package myMath;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Scanner;

public class WordPuzzleI {

	// http://www.tilepuzzles.com/default.asp?p=7
	
	// 37 moves multiple ways:
	
	// 14 10*6*2 1*0 4*8*12 13*14*15 11*7*3 2*1*0 4*8 9*10 6 7 3
	// 2*1*0 4*8 9*10*11 7 6 2 1*0 4*8 9*10 6*2 1*0 4*8 9*10 14
	// 13*12 8 9*10 6 7 11*15 14*13*12 8 9 13 14*15
	
	// 14 10*6*2 1*0 4*8*12 13*14*15 11*7*3 2*1*0 4*8 9*10*11 7*3
	// 2*1*0 4*8 9*10 6*2 1*0 4*8 9*10 14 13*12 8 9*10 6*2 1*0 4*8
	// 9*10 6*2 1*0 4*8 9*10*11 15 14*13*12 8*4*0 1*2 6*10 9 13 14*15
	
	// 11*7*3 2*1*0 4*8*12 13*14 10*6*2 3 7 6 2 1*0 4*8 9*10 6*2
	// 1*0 4*8 9*10*11 7 6 2 1*0 4*8 9*10 6*2 3 7*11*15 14 10*6*2
	// 1*0 4*8*12 13*14*15 11 10*9*8 12 13 9 10*11 15
	
	// 14 10*6*2 1*0 4*8*12 13*14*15 11*7*3 2*1*0 4*8 9*10*11 7*3
	// 2*1*0 4*8 9*10 14 13 9 10 6*2 1*0 4*8 9*10 6*2 3 7*11*15
	// 14*13*12 8 9*10 14 13 9 10 6 7 3 2*1*0 4*8*12 13*14*15
	
	static Short[] square;
	static int minMoves;
	static boolean notWon;
	static HashMap<List<Short>,Integer> map;
	static int total;
	static int a,b,c;
	
	public static void main(String[] args) {
		long start = System.nanoTime();
		notWon = true;
		total = 0;
		map = new HashMap<List<Short>,Integer>();
		square = new Short[16];
		short[] n = new short[16];
		
		// 0  1  2  3
		// 4  5  6  7
		// 8  9  10 11
		// 12 13 14 15
		
		// initial game condition
		n[0] = 3;
		n[1] = 7;
		n[2] = 4;
		n[3] = 6;
		n[4] = 12;
		n[5] = 5;
		n[6] = 0;
		n[7] = 9;
		n[8] = 10;
		n[9] = 1;
		n[10] = 14;
		n[11] = 11;
		n[12] = 2;
		n[13] = 13;
		n[14] = 8;
		n[15] = 15;
		
		minMoves = 40;
		int trueMin = 40;
		if (true) {
			for (a = 2; a != 11; a++) {
				int x = ((3*a)/2)+1;
				for (b = Math.max((4*a)/5,2); b != x; b++) {
					if (gcd(a,b) != 1) {
						continue;
					}
					System.out.println("a = " + a + " and b = " + b);
					notWon = true;
					minMoves = 40;
					for (c = 30*a+1; notWon; c++) {
						for (int i = 0; i != 16; i++) {
							square[i] = n[i];
						}
						map = new HashMap<List<Short>,Integer>();
						move(15,100,0,"");
					}
					System.out.println("c was " + c);
					if (trueMin > minMoves) {
						System.out.println("new min is " + minMoves);
						trueMin = minMoves;
					}
				}
			}
		}
		
		System.out.println("minMoves = " + minMoves);
		long end = System.nanoTime();
		double time = (end-start)/1000000000.0;
		System.out.println(time + " seconds");
	}
	
	public static int dist() {
		int d = 0;
		for (int i = 0; i != 16; i++) {
			int p = square[i];
			if (p != 15) {
				if (p/4 > i/4) {
					d += ((p/4)-(i/4));
				}
				else {
					d += ((i/4)-(p/4));
				}
				if (p%4 > i%4) {
					d += ((p%4)-(i%4));
				}
				else {
					d += ((i%4)-(p%4));
				}
			}
		}
		return d;
	}
	
	public static void move(int index, int lastIndex, int moveNum, String moves) {
		if (dist()*a + moveNum*b > c) {
			// init dist = 30, and I knew it'd be <50 moves (I got 50), so around 150+ here
			// had dist()*5 + moveNum*3>156
			
			// once I got a solution of 31 moves,
			// tried dist()+moveNum>35 (none), 40 (see above), 36 (none) 37 (see above)
			return;
		}
		if (moveNum>=minMoves) {
	//		printBoard();
			return;
		}
	//	System.out.println(moves);
	//	System.out.println(moveNum);
		boolean win = true;
		for (int i = 0; i != 16; i++) {
			win &= (square[i]==i);
		}
		if (win) {
			notWon = false;
			minMoves = moveNum;
			System.out.println("won in " + moveNum + " moves:\n" + moves);
			Scanner scan = new Scanner(System.in);
			System.out.println("enter anything to continue");
			scan.nextLine();
			System.out.println("continuing");
			return;
		}
		List<Short> list = Arrays.asList(square);
		if (map.containsKey(list) && map.get(list) <= moveNum) {
			return;
		}
		map.put(list,moveNum);
		total++;
	//	System.out.println(total);
		if (index-4 != lastIndex && index>3 && square[index-4]/4 >= index/4 && index-4 != 5) {
			int newIndex = index-4;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2*index == newIndex + lastIndex);
			move(newIndex,index,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
		if (index-1 != lastIndex && index%4 != 0 && square[index-1]%4 >= index%4 && index-1 != 5) {
			int newIndex = index-1;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2*index == newIndex + lastIndex);
			move(newIndex,index,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
		if (index+1 != lastIndex && index%4 != 3 && square[index+1]%4 <= index%4 && index+1 != 5) {
			int newIndex = index+1;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2*index == newIndex + lastIndex);
			move(newIndex,index,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
		if (index+4 != lastIndex && index<12 && square[index+4]/4 <= index/4 && index+4 != 5) {
			int newIndex = index+4;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2*index == newIndex + lastIndex);
			move(newIndex,index,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
		if (index-1 != lastIndex && index%4 != 0 && square[index-1]%4 < index%4 && index-1 != 5) {
			int newIndex = index-1;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2*index == newIndex + lastIndex);
			move(newIndex,index,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
		if (index-4 != lastIndex && index>3 && square[index-4]/4 < index/4 && index-4 != 5) {
			int newIndex = index-4;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2*index == newIndex + lastIndex);
			move(newIndex,index,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
		if (index+1 != lastIndex && index%4 != 3 && square[index+1]%4 > index%4 && index+1 != 5) {
			int newIndex = index+1;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2*index == newIndex + lastIndex);
			move(newIndex,index,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
		if (index+4 != lastIndex && index<12 && square[index+4]/4 > index/4 && index+4 != 5) {
			int newIndex = index+4;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2*index == newIndex + lastIndex);
			move(newIndex,index,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
	}
	
	public static void printBoard() {
		for (int i = 0; i != 16; i++) {
			if (i%4==0) {
				System.out.println();
			}
			System.out.print(square[i] + "\t");
		}
		Scanner scan = new Scanner(System.in);
		scan.nextLine();
	}
	
	public static int gcd(int a, int b) {
		if (b%a == 0) {
			return a;
		}
		else {
			return gcd(b%a,a);
		}
	}
}