package myMath;

import java.util.ArrayList;
import java.util.Arrays;

public class KlotskiPuzzle {

	static int[][] choose;
	static int[][][][] gameStateBefore;
	static boolean won;
	static int states;
	static int moves;
	static int myOneIndex;
	static long start;
	
	public static void main(String[] args) {
		start = System.nanoTime();
		makeChoose();
		gameStateBefore = new int[20][choose[16][4]][choose[8][4]][3];
		int[] initial = {1,0,3,8,11,13,14,16,19,9,17,18};
		states = 0;
		usePositions(initial,-1);
		ArrayList<int[]> states1 = new ArrayList<int[]>();
		ArrayList<int[]> states2 = new ArrayList<int[]>();
		ArrayList<int[]> tempPast;
		ArrayList<int[]> tempAdd;
		states2.add(initial);
		for (moves = 1; !won; moves++) {
			if (moves%2 == 0) {
				tempPast = states1;
				states2 = new ArrayList<int[]>();
				tempAdd = states2;
			}
			else {
				tempPast = states2;
				states1 = new ArrayList<int[]>();
				tempAdd = states1;
			}
			for (int i = tempPast.size()-1; i != -1; i--) {
				int[] positions = tempPast.get(i);
				usePositions(positions,-1);
				int thisIndex = myOneIndex;
				int free1 = positions[10];
				int free2 = positions[11];
				if (free1 + 4 == free2) {
					if (free1%4 != 0) {
						if (positions[0]+2 == free1) {
							int[] newPositions = Arrays.copyOf(positions,12);
							newPositions[0]++;
							newPositions[10]-=2;
							newPositions[11]-=2;
							if (usePositions(newPositions,thisIndex)) {
								tempAdd.add(newPositions);
							}
						}
						for (int j = 1; j != 5; j++) {
							if (positions[j]+1 == free1) {
								int[] newPositions = Arrays.copyOf(positions,12);
								newPositions[j]++;
								newPositions[10]--;
								newPositions[11]--;
								if (usePositions(newPositions,thisIndex)) {
									tempAdd.add(newPositions);
								}
								break;
							}
						}
					}
					if (free1%4 != 3) {
						if (positions[0]-1 == free1) {
							int[] newPositions = Arrays.copyOf(positions,12);
							newPositions[0]--;
							newPositions[10]+=2;
							newPositions[11]+=2;
							if (usePositions(newPositions,thisIndex)) {
								tempAdd.add(newPositions);
							}
						}
						for (int j = 1; j != 5; j++) {
							if (positions[j]-1 == free1) {
								int[] newPositions = Arrays.copyOf(positions,12);
								newPositions[j]--;
								newPositions[10]++;
								newPositions[11]++;
								if (usePositions(newPositions,thisIndex)) {
									tempAdd.add(newPositions);
								}
								break;
							}
						}
					}
				}
				if (free1 + 1 == free2) {
					if (positions[0] + 8 == free1) {
						int[] newPositions = Arrays.copyOf(positions,12);
						newPositions[0] += 4;
						newPositions[10] -= 8;
						newPositions[11] -= 8;
						if (usePositions(newPositions,thisIndex)) {
							tempAdd.add(newPositions);
						}
					}
					if (positions[9] + 4 == free1) {
						int[] newPositions = Arrays.copyOf(positions,12);
						newPositions[9] += 4;
						newPositions[10] -= 4;
						newPositions[11] -= 4;
						if (usePositions(newPositions,thisIndex)) {
							tempAdd.add(newPositions);
						}
					}
					if (positions[9] - 4 == free1) {
						int[] newPositions = Arrays.copyOf(positions,12);
						newPositions[9] -= 4;
						newPositions[10] += 4;
						newPositions[11] += 4;
						if (usePositions(newPositions,thisIndex)) {
							tempAdd.add(newPositions);
						}
					}
				}
				if (free1%4 != 0) {
					if (positions[9] + 2 == free1) {
						int[] newPositions = Arrays.copyOf(positions,12);
						newPositions[9]++;;
						newPositions[10] -= 2;
						if (usePositions(newPositions,thisIndex)) {
							tempAdd.add(newPositions);
						}
					}
					for (int j = 5; j != 9; j++) {
						if (positions[j] + 1 == free1) {
							int[] newPositions = Arrays.copyOf(positions,12);
							newPositions[j]++;
							newPositions[10]--;
							if (usePositions(newPositions,thisIndex)) {
								tempAdd.add(newPositions);
							}
							break;
						}
					}
				}
				if (free1%4 != 3) {
					if (positions[9] - 1 == free1) {
						int[] newPositions = Arrays.copyOf(positions,12);
						newPositions[9]--;
						newPositions[10] += 2;
						if (usePositions(newPositions,thisIndex)) {
							tempAdd.add(newPositions);
						}
					}
					for (int j = 5; j != 9; j++) {
						if (positions[j] - 1 == free1) {
							int[] newPositions = Arrays.copyOf(positions,12);
							newPositions[j]--;
							newPositions[10]++;
							if (usePositions(newPositions,thisIndex)) {
								tempAdd.add(newPositions);
							}
							break;
						}
					}
				}
				for (int j = 1; j != 9; j++) {
					if (positions[j] - 4 == free1) {
						int[] newPositions = Arrays.copyOf(positions,12);
						newPositions[j]-=4;
						newPositions[10]+=4*((12-j)/4);
						if (usePositions(newPositions,thisIndex)) {
							tempAdd.add(newPositions);
						}
						break;
					}
				}
				for (int j = 5; j != 9; j++) {
					if (positions[j] + 4 == free1) {
						int[] newPositions = Arrays.copyOf(positions,12);
						newPositions[j]+=4;
						newPositions[10]-=4;
						if (usePositions(newPositions,thisIndex)) {
							tempAdd.add(newPositions);
						}
						break;
					}
				}
				for (int j = 1; j != 5; j++) {
					if (positions[j] + 8 == free1) {
						int[] newPositions = Arrays.copyOf(positions,12);
						newPositions[j]+=4;
						newPositions[10]-=8;
						if (usePositions(newPositions,thisIndex)) {
							tempAdd.add(newPositions);
						}
						break;
					}
				}
				if (free2%4 != 0) {
					if (positions[9] + 2 == free2) {
						int[] newPositions = Arrays.copyOf(positions,12);
						newPositions[9]++;;
						newPositions[11] -= 2;
						if (usePositions(newPositions,thisIndex)) {
							tempAdd.add(newPositions);
						}
					}
					for (int j = 5; j != 9; j++) {
						if (positions[j] + 1 == free2) {
							int[] newPositions = Arrays.copyOf(positions,12);
							newPositions[j]++;
							newPositions[11]--;
							if (usePositions(newPositions,thisIndex)) {
								tempAdd.add(newPositions);
							}
							break;
						}
					}
				}
				if (free2%4 != 3) {
					if (positions[9] - 1 == free2) {
						int[] newPositions = Arrays.copyOf(positions,12);
						newPositions[9]--;
						newPositions[11] += 2;
						if (usePositions(newPositions,thisIndex)) {
							tempAdd.add(newPositions);
						}
					}
					for (int j = 5; j != 9; j++) {
						if (positions[j] - 1 == free2) {
							int[] newPositions = Arrays.copyOf(positions,12);
							newPositions[j]--;
							newPositions[11]++;
							if (usePositions(newPositions,thisIndex)) {
								tempAdd.add(newPositions);
							}
							break;
						}
					}
				}
				for (int j = 1; j != 9; j++) {
					if (positions[j] - 4 == free2) {
						int[] newPositions = Arrays.copyOf(positions,12);
						newPositions[j]-=4;
						newPositions[11]+=4*((12-j)/4);
						if (usePositions(newPositions,thisIndex)) {
							tempAdd.add(newPositions);
						}
						break;
					}
				}
				for (int j = 5; j != 9; j++) {
					if (positions[j] + 4 == free2) {
						int[] newPositions = Arrays.copyOf(positions,12);
						newPositions[j]+=4;
						newPositions[11]-=4;
						if (usePositions(newPositions,thisIndex)) {
							tempAdd.add(newPositions);
						}
						break;
					}
				}
				for (int j = 1; j != 5; j++) {
					if (positions[j] + 8 == free2) {
						int[] newPositions = Arrays.copyOf(positions,12);
						newPositions[j]+=4;
						newPositions[11]-=8;
						if (usePositions(newPositions,thisIndex)) {
							tempAdd.add(newPositions);
						}
						break;
					}
				}
			}
			System.out.println(moves + " moves so far and " + states + " game states");
		}
	}
	
	public static void makeChoose() {
		choose = new int[21][5];
		choose[0][0] = 1;
		choose[0][1] = 0;
		choose[0][2] = 0;
		choose[0][3] = 0;
		choose[0][4] = 0;
		for (int i = 1; i != 21; i++) {
			choose[i][0] = 1;
			choose[i][1] = choose[i-1][1] + choose[i-1][0];
			choose[i][2] = choose[i-1][2] + choose[i-1][1];
			choose[i][3] = choose[i-1][3] + choose[i-1][2];
			choose[i][4] = choose[i-1][4] + choose[i-1][3];
		}
	}
	
	public static boolean usePositions(int[] positions, int previousIndex) {
		int bigIndex = positions[0];
		int[] used = {bigIndex,bigIndex+1,bigIndex+4,bigIndex+5};
		int tallIndex = function(used,positions[1],positions[2],positions[3],positions[4]);
		used = new int[12];
		used[0] = bigIndex;
		used[1] = bigIndex+1;
		used[2] = bigIndex+4;
		used[3] = bigIndex+5;
		for (int i = 4; i != 12; i++) { // need positions 1 through 4 and the blocks below them
			used[i] = positions[(i/2)-1] + ((i%2)*4);
		}
		int smallIndex = function(used,positions[5],positions[6],positions[7],positions[8]);
		int wideIndex = positions[9];
		int wideOrig = wideIndex;
		if (wideOrig>positions[0]+4) {
			wideIndex-=4;
		}
		else if (wideOrig>positions[0]) {
			wideIndex-=2;
		}
		for (int i = 1; i != 5; i++) {
			if (wideOrig>positions[i]+4) {
				wideIndex-=2;
			}
			else if (wideOrig>positions[i]) {
				wideIndex--;
			}
		}
		for (int i = 5; i != 9; i++) {
			if (wideOrig>positions[i]) {
				wideIndex--;
			}
		}
		myOneIndex = oneIndex(bigIndex,tallIndex,smallIndex,wideIndex);
		if (positions[10]>positions[11]) {
			int temp = positions[10];
			positions[10] = positions[11];
			positions[11] = temp;
		}
		if (bigIndex == 13) {
			System.out.println("\nWon in " + moves + " moves:");
			int[][] finalStates = new int[moves+1][];
			int index = moves;
			finalStates[index] = inverse(bigIndex,tallIndex,smallIndex,wideIndex);
			index--;
			while (index != -1) {
				int[] temp = undoOneIndex(previousIndex);
				finalStates[index] = inverse(temp[0],temp[1],temp[2],temp[3]);
				previousIndex = gameStateBefore[temp[0]][temp[1]][temp[2]][temp[3]];
				index--;
			}
			System.out.println("square\ttall1\ttall2\ttall3\ttall4\tsmall1\tsmall2\tsmall3\tsmall4\twide\tspace1\tspace2");
			for (int j = 0; j != moves; j++) {
				for (int i = 0; i != 12; i++) {
					System.out.print(finalStates[j][i] + "\t");
				}
				System.out.println("\t\t" + whatMove(finalStates[j],finalStates[j+1]));
			}
			for (int i = 0; i != 12; i++) {
				System.out.print(finalStates[moves][i] + "\t");
			}
			long end = System.nanoTime();
			double time = (end-start)/1000000000.0;
			System.out.println("\n" + time + " seconds");

			System.exit(0);
		}
		if (gameStateBefore[bigIndex][tallIndex][smallIndex][wideIndex] != 0) {
			return false;
		}
		else {
			states++;
	/*		for (int i = 0; i != 12; i++) {
				System.out.print(positions[i] + " ");
			}
			System.out.println(); */
			gameStateBefore[bigIndex][tallIndex][smallIndex][wideIndex] = previousIndex;
			return true;
		}
		
	}
	
	public static String whatMove(int[] a, int[] b) {
		if (a[0] != b[0]) {
			if (a[0]-b[0]==1) {
				return ("square left");
			}
			else if (a[0]-b[0]==-1) {
				return ("square right");
			}
			else {
				return ("square down");
			}
		}
		else if (a[9] != b[9]) {
			if (a[9]-b[9]==1) {
				return ("wide left");
			}
			else if (a[9]-b[9]==-1) {
				return ("wide right");
			}
			else if (a[9]-b[9]==-4){
				return ("wide down");
			}
			else {
				return ("wide up");
			}
		}
		else if (a[1]+a[2]+a[3]+a[4] != b[1]+b[2]+b[3]+b[4]) {
			int x = 0, y = 0;
			for (int i = 1; i != 5; i++) {
				boolean xFound = false, yFound = false;
				for (int j = 1; j != 5; j++) {
					if (a[i] == b[j]) {
						xFound = true;
					}
					if (b[i] == a[j]) {
						yFound = true;
					}
				}
				if (!xFound) {
					x = a[i];
				}
				if (!yFound) {
					y = b[i];
				}
			}
			if (x-y==1) {
				return ("tall " + x + " left");
			}
			else if (x-y==-1) {
				return ("tall " + x + " right");
			}
			else if (x-y==-4){
				return ("tall " + x + " down");
			}
			else {
				return ("tall " + x + " up");
			}
		}
		else {
			int x = 0, y = 0;
			for (int i = 5; i != 9; i++) {
				boolean xFound = false, yFound = false;
				for (int j = 5; j != 9; j++) {
					if (a[i] == b[j]) {
						xFound = true;
					}
					if (b[i] == a[j]) {
						yFound = true;
					}
				}
				if (!xFound) {
					x = a[i];
				}
				if (!yFound) {
					y = b[i];
				}
			}
			if (x-y==1) {
				return ("small " + x + " left");
			}
			else if (x-y==-1) {
				return ("small " + x + " right");
			}
			else if (x-y==-4){
				return ("small " + x + " down");
			}
			else {
				return ("small " + x + " up");
			}
		}
	}
	
	public static int function(int[] used, int a, int b, int c, int d) {
		int aOrig = a;
		int bOrig = b;
		int cOrig = c;
		int dOrig = d;
		for (int i = used.length-1; i != -1; i--) {
			int p = used[i];
			if (aOrig>p) {
				a--;
			}
			if (bOrig>p) {
				b--;
			}
			if (cOrig>p) {
				c--;
			}
			if (dOrig>p) {
				d--;
			}
		}
		if (a>b) {
			if (b>c) {
				if (b>d) {
					if (c>d) {
						return (choose[a][4] + choose[b][3] + choose[c][2] + choose[d][1]);
					}
					else {
						return (choose[a][4] + choose[b][3] + choose[d][2] + choose[c][1]);
					}
				}
				else {
					if (a>d) {
						return (choose[a][4] + choose[d][3] + choose[b][2] + choose[c][1]);
					}
					else {
						return (choose[d][4] + choose[a][3] + choose[b][2] + choose[c][1]);
					}
				}
			}
			else {
				if (a>c) {
					if (c>d) {
						if (b>d) {
							return (choose[a][4] + choose[c][3] + choose[b][2] + choose[d][1]);
						}
						else {
							return (choose[a][4] + choose[c][3] + choose[d][2] + choose[b][1]);
						}
					}
					else {
						if (a>d) {
							return (choose[a][4] + choose[d][3] + choose[c][2] + choose[b][1]);
						}
						else {
							return (choose[d][4] + choose[a][3] + choose[c][2] + choose[b][1]);
						}
					}
				}
				else {
					if (a>d) {
						if (b>d) {
							return (choose[c][4] + choose[a][3] + choose[b][2] + choose[d][1]);
						}
						else {
							return (choose[c][4] + choose[a][3] + choose[d][2] + choose[b][1]);
						}
					}
					else {
						if (c>d) {
							return (choose[c][4] + choose[d][3] + choose[a][2] + choose[b][1]);
						}
						else {
							return (choose[d][4] + choose[c][3] + choose[a][2] + choose[b][1]);
						}
					}
				}
			}
		}
		else {
			if (a>c) {
				if (a>d) {
					if (c>d) {
						return (choose[b][4] + choose[a][3] + choose[c][2] + choose[d][1]);
					}
					else {
						return (choose[b][4] + choose[a][3] + choose[d][2] + choose[c][1]);
					}
				}
				else {
					if (b>d) {
						return (choose[b][4] + choose[d][3] + choose[a][2] + choose[c][1]);
					}
					else {
						return (choose[d][4] + choose[b][3] + choose[a][2] + choose[c][1]);
					}
				}
			}
			else {
				if (b>c) {
					if (c>d) {
						if (a>d) {
							return (choose[b][4] + choose[c][3] + choose[a][2] + choose[d][1]);
						}
						else {
							return (choose[b][4] + choose[c][3] + choose[d][2] + choose[a][1]);
						}
					}
					else {
						if (b>d) {
							return (choose[b][4] + choose[d][3] + choose[c][2] + choose[a][1]);
						}
						else {
							return (choose[d][4] + choose[b][3] + choose[c][2] + choose[a][1]);
						}
					}
				}
				else {
					if (b>d) {
						if (a>d) {
							return (choose[c][4] + choose[b][3] + choose[a][2] + choose[d][1]);
						}
						else {
							return (choose[c][4] + choose[b][3] + choose[d][2] + choose[a][1]);
						}
					}
					else {
						if (c>d) {
							return (choose[c][4] + choose[d][3] + choose[b][2] + choose[a][1]);
						}
						else {
							return (choose[d][4] + choose[c][3] + choose[b][2] + choose[a][1]);
						}
					}
				}
			}
		}
	}
	
	public static int oneIndex(int a, int b, int c, int d) {
		return (((b*choose[8][4]+c)*20+a)*3+d);
	}
	
	public static int[] undoOneIndex (int x) {
		int[] indices = new int[4];
		indices[3] = x%3;
		indices[0] = (x/3)%20;
		indices[2] = (x/60)%choose[8][4];
		indices[1] = (x/60/choose[8][4]);
		return indices;
	}
	
	public static int[] inverse(int bigIndex, int tallIndex, int smallIndex, int wideIndex) {
		int[] positions = new int[12];
		boolean[] taken = new boolean[20];
		positions[0] = bigIndex;
		taken[positions[0]] = true;
		taken[positions[0]+1] = true;
		taken[positions[0]+4] = true;
		taken[positions[0]+5] = true;
		for (positions[1] = 16; choose[positions[1]][4]>tallIndex; positions[1]--) {
		}
		tallIndex -= choose[positions[1]][4];
		for (positions[2] = 15; choose[positions[2]][3]>tallIndex; positions[2]--) {
		}
		tallIndex -= choose[positions[2]][3];
		for (positions[3] = 14; choose[positions[3]][2]>tallIndex; positions[3]--) {
		}
		tallIndex -= choose[positions[3]][2];
		positions[4] = tallIndex;
		for (int i = 1; i != 5; i++) {
			if (positions[i] >= positions[0]) {
				positions[i] += 2;
			}
			if (positions[i] >= positions[0]+4) {
				positions[i] += 2;
			}
			taken[positions[i]] = true;
			taken[positions[i]+4] = true;
		}
		for (positions[5] = 8; choose[positions[5]][4]>smallIndex; positions[5]--) {
		}
		smallIndex -= choose[positions[5]][4];
		for (positions[6] = 7; choose[positions[6]][3]>smallIndex; positions[6]--) {
		}
		smallIndex -= choose[positions[6]][3];
		for (positions[7] = 6; choose[positions[7]][2]>smallIndex; positions[7]--) {
		}
		smallIndex -= choose[positions[7]][2];
		positions[8] = smallIndex;
		int index = 0;
		int position = 8;
		int maxIndexPlusOne = positions[5]+1;
		for (int counter = 0; counter != maxIndexPlusOne; index++) {
			if (!taken[index]) {
				if (positions[position] == counter) {
					positions[position] = index;
					taken[index] = true;
					position--;
				}
				counter++;
			}
		}
		index = 0;
		for (int counter = 0; counter <= wideIndex; index++) {
			if (!taken[index]) {
				counter++;
			}
		}
		positions[9] = index-1;
		taken[positions[9]] = true;
		taken[positions[9]+1] = true;
		index = 0;
		while (taken[index]) {
			index++;
		}
		positions[10] = index;
		index++;
		while (taken[index]) {
			index++;
		}
		positions[11] = index;
	/*	System.out.println(positions[0] + " " + positions[1] + " " + positions[2] + " " + positions[3] + " " + positions[4] + " "
				+ positions[5] + " " + positions[6] + " " + positions[7] + " " + positions[8] + " "
				+ positions[9] + " " + positions[10] + " " + positions[11]); */
		return positions;
	}
}