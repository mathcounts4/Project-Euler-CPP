package myMath;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Scanner;

public class WordPuzzleII {

	// http://www.tilepuzzles.com/default.asp?p=9
	
	// 44 moves (from 1,1,50)
	// 14*13*12 8*4*0 1*2*3 7*11 10*9*8 4 5*6 2 1
	// 5*9 8 12 13 9 10*11 7*3 2*1*0 4*8 9*10 14 15
	// 11*7*3 2*1*0 4 5*6 10 9*8 4 5*6*7 11*15 14*13*12
	// 8 9*10 6*2 3 7*11*15 14 10*6 5 1 2 6*10 11 15
	
	// 44 moves (from 4,3,150)
	// 11*7*3 2 6 5*4 8 9*10 6*2 1*0 4 5*6*7 3 2*1 5 6
	// 10 9*8 4 5*6*7 11*15 14*13*12 8 9*10 6*2 3 7*11
	// 10 14 15 11*7*3 2 6*10*14 15 11*7 6 2 3 7*11*15
	// 14 10*6*2 3 7*11 10 14 15
	
	// 44 moves (from 5,3,185)
	// 11*7*3 2*1 5 4 8 9*10 6 7 3 2*1*0 4 5*6 10 9*8
	// 4 5*6*7 11*15 14*13*12 8 9*10 6*2 3 7*11 10 14
	// 15 11*7*3 2*1 5 6 10*14 15 11*7 6 2 3 7*11*15
	// 14 10*6*2 3 7*11 10 14 15
	
	// 44 moves (from 3,2,110)
	// 11*7*3 2 6 5*4 8 9*10 6*2 1*0 4 5*6*7 3 2*1 5 6
	// 10 9*8 4 5*6*7 11*15 14*13*12 8 9*10 6*2 3 7*11
	// 10 14 15 11*7*3 2 6*10*14 15 11*7 6 2 3 7*11*15
	// 14 10*6*2 3 7*11 10 14 15
	
	// 44 moves (from 2,1,71)
	// 11*7*3 2*1 5 4 8 9*10 6 7 3 2*1*0 4 5*6 10 9*8
	// 4 5*6*7 11*15 14*13*12 8 9*10 6*2 3 7*11 10 14
	// 15 11*7*3 2*1 5 6 10*14 15 11*7 6 2 3 7*11*15
	// 14 10*6*2 3 7*11 10 14 15
	
	// 44 moves (from 7,5,259)
	// 11*7*3 2 6 5*4 8 9*10 6*2 1*0 4 5*6*7 3 2*1 5 6
	// 10 9*8 4 5*6*7 11*15 14*13*12 8 9*10 6*2 3 7*11
	// 10 14 15 11*7*3 2 6*10*14 15 11*7 6 2 3 7*11*15
	// 14 10*6*2 3 7*11 10 14 15
	
	static Short[] square;
	static int minMoves;
	static boolean notWon;
	static HashMap<List<Short>,Integer> map;
	static int total;
	
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
		n[0] = 11;
		n[1] = 6;
		n[2] = 14;
		n[3] = 4;
		n[4] = 12;
		n[5] = 5;
		n[6] = 1;
		n[7] = 3;
		n[8] = 0;
		n[9] = 8;
		n[10] = 9;
		n[11] = 7;
		n[12] = 13;
		n[13] = 2;
		n[14] = 10;
		n[15] = 15;
		
		minMoves = 45;
		
		for (int i = 0; i != 16; i++) {
			square[i] = n[i];
		}
		map = new HashMap<List<Short>,Integer>();
		System.out.println("dist = " + dist());
		move(15,100,0,"");
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
		if (7*dist() + 5*moveNum > 259) {
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
		if (index-4 != lastIndex && index>3 && square[index-4]/4 >= index/4
				&& square[index-4] != 8 && square[index-4] != 9) {
			int newIndex = index-4;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2*index == newIndex + lastIndex);
			move(newIndex,index,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
		if (index-1 != lastIndex && index%4 != 0 && square[index-1]%4 >= index%4
				&& square[index-1] != 3 && square[index-1] != 7) {
			int newIndex = index-1;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2*index == newIndex + lastIndex);
			move(newIndex,index,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
		if (index+1 != lastIndex && index%4 != 3 && square[index+1]%4 <= index%4
				&& square[index+1] != 3 && square[index+1] != 7) {
			if (square[index+1] == 8) {
				int newIndex = index+2;
				int tempIndex = index+1;
				square[index] = 8;
				square[tempIndex] = 9;
				square[newIndex] = 15;
				boolean spareMove = (2*index == tempIndex + lastIndex);
				move(newIndex,tempIndex,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + tempIndex + "*" + newIndex);
				square[newIndex] = 9;
				square[tempIndex] = 8;
				square[index] = 15;
			}
			else {
				int newIndex = index+1;
				square[index] = square[newIndex];
				square[newIndex] = 15;
				boolean spareMove = (2*index == newIndex + lastIndex);
				move(newIndex,index,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + newIndex);
				square[newIndex] = square[index];
				square[index] = 15;
			}
		}
		if (index+4 != lastIndex && index<12 && square[index+4]/4 <= index/4
				&& square[index+4] != 8 && square[index+4] != 9) {
			if (square[index+4] == 3) {
				int newIndex = index+8;
				int tempIndex = index+4;
				square[index] = 3;
				square[tempIndex] = 7;
				square[newIndex] = 15;
				boolean spareMove = (2*index == tempIndex + lastIndex);
				move(newIndex,tempIndex,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + tempIndex + "*" + newIndex);
				square[newIndex] = 7;
				square[tempIndex] = 3;
				square[index] = 15;
			}
			else {
				int newIndex = index+4;
				square[index] = square[newIndex];
				square[newIndex] = 15;
				boolean spareMove = (2*index == newIndex + lastIndex);
				move(newIndex,index,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + newIndex);
				square[newIndex] = square[index];
				square[index] = 15;
			}
		}
		if (index-1 != lastIndex && index%4 != 0 && square[index-1]%4 < index%4
				&& square[index-1] != 3 && square[index-1] != 7) {
			if (square[index-1] == 9) {
				int newIndex = index-2;
				int tempIndex = index-1;
				square[index] = 9;
				square[tempIndex] = 8;
				square[newIndex] = 15;
				boolean spareMove = (2*index == tempIndex + lastIndex);
				move(newIndex,tempIndex,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + tempIndex + "*" + newIndex);
				square[newIndex] = 8;
				square[tempIndex] = 9;
				square[index] = 15;
			}
			else {
				int newIndex = index-1;
				square[index] = square[newIndex];
				square[newIndex] = 15;
				boolean spareMove = (2*index == newIndex + lastIndex);
				move(newIndex,index,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + newIndex);
				square[newIndex] = square[index];
				square[index] = 15;
			}
		}
		if (index-4 != lastIndex && index>3 && square[index-4]/4 < index/4
				&& square[index-4] != 8 && square[index-4] != 9) {
			if (square[index-4] == 7) {
				int newIndex = index-8;
				int tempIndex = index-4;
				square[index] = 7;
				square[tempIndex] = 3;
				square[newIndex] = 15;
				boolean spareMove = (2*index == index-4 + lastIndex);
				move(newIndex,tempIndex,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + tempIndex + "*" + newIndex);
				square[newIndex] = 3;
				square[tempIndex] = 7;
				square[index] = 15;
			}
			else {
				int newIndex = index-4;
				square[index] = square[newIndex];
				square[newIndex] = 15;
				boolean spareMove = (2*index == newIndex + lastIndex);
				move(newIndex,index,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + newIndex);
				square[newIndex] = square[index];
				square[index] = 15;
			}
		}
		if (index+1 != lastIndex && index%4 != 3 && square[index+1]%4 > index%4
				&& square[index+1] != 3 && square[index+1] != 7) {
			int newIndex = index+1;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2*index == newIndex + lastIndex);
			move(newIndex,index,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
		if (index+4 != lastIndex && index<12 && square[index+4]/4 > index/4
				&& square[index+4] != 8 && square[index+4] != 9) {
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