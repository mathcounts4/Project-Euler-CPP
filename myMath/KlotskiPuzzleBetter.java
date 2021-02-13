package myMath;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Set;

public class KlotskiPuzzleBetter {

	// 58 moves
	
	// issue with which index is stored (a gamestate is reached in multiple ways on the same move
	// the second works in a combo, but the first one's index is saved)
	
	static int[][] choose;
	static int[][][][] gameStateBefore;
	static boolean won;
	static int states;
	static int moves;
	static long start;
	static ArrayList<int[]> tempPast;
	static ArrayList<int[]> tempAdd;
	static boolean useMoveNumbers;
	static HashMap<int[],Integer> myQueue;
	
	public static void main(String[] args) {
		start = System.nanoTime();
		makeChoose();
		gameStateBefore = new int[20][choose[16][4]][choose[8][4]][3];
		int[] initial = {1,0,3,8,11,13,14,16,19,9,17,18};
		states = 0;
		moves = 0;
		useMoveNumbers = false;
		usePositions(initial,-1);
		ArrayList<int[]> states1 = new ArrayList<int[]>();
		ArrayList<int[]> states2 = new ArrayList<int[]>();
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
				int useIndex = getOneIndex(positions);
				for (int piece = 0; piece != 10; piece++) {
					myQueue = new HashMap<int[],Integer>();
					attemptMoves(positions,piece,useIndex,4);
				}
			}
			System.out.println(moves + " moves so far and " + states + " game states");
		}
	}
	
	public static void attemptMoves(int[] positions, int piece, int index, int remaining) {
		if (remaining == 0) {
			return;
		}
		int[] move1;
		if ((move1 = canMoveUp(positions,piece,index,true)) != null) {
			attemptMoves(move1,piece,getOneIndex(move1),remaining-1);
		}
		if ((move1 = canMoveRight(positions,piece,index,true)) != null) {
			attemptMoves(move1,piece,getOneIndex(move1),remaining-1);
		}
		if ((move1 = canMoveDown(positions,piece,index,true)) != null) {
			attemptMoves(move1,piece,getOneIndex(move1),remaining-1);
		}
		if ((move1 = canMoveLeft(positions,piece,index,true)) != null) {
			attemptMoves(move1,piece,getOneIndex(move1),remaining-1);
		}
	}
	
	public static void printTo(int[] positions) {
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

		int previousIndex = gameStateBefore[bigIndex][tallIndex][smallIndex][wideIndex];
		int miniMoves = 0;
		int tempIndex = previousIndex;
		while (tempIndex != -1) {
			int[] temps = undoOneIndex(tempIndex);
			tempIndex = gameStateBefore[temps[0]][temps[1]][temps[2]][temps[3]];
			miniMoves++;
		}
		System.out.println("\n" + miniMoves + " miniMoves");
		
		int[][] finalStates = new int[miniMoves+1][];
		int[] finalIndices = new int[miniMoves+1];
		int index = miniMoves;
		finalStates[index] = positions;
		finalIndices[index] = getOneIndex(positions);
		index--;
		while (index != -1) {
			int[] temp = undoOneIndex(previousIndex);
			finalStates[index] = inverse(temp[0],temp[1],temp[2],temp[3]);
			finalIndices[index] = previousIndex;
			previousIndex = gameStateBefore[temp[0]][temp[1]][temp[2]][temp[3]];
			index--;
		}
		
		useMoveNumbers = true;
		for (int a1 = 19; a1 != -1; a1--) {
			for (int b1 = choose[16][4]-1; b1 != -1; b1--) {
				for (int c1 = choose[8][4]-1; c1 != -1; c1--) {
					for (int d1 = 2; d1 != -1; d1--) {
						gameStateBefore[a1][b1][c1][d1] = -1;
					}
				}
			}
		}

		int[] initial = {1,0,3,8,11,13,14,16,19,9,17,18};
		int myMoves = moves;
		moves = 0;
		usePositions(initial,-1);
		ArrayList<int[]> states1 = new ArrayList<int[]>();
		ArrayList<int[]> states2 = new ArrayList<int[]>();
		states2.add(initial);
		for (moves = 1; moves <= myMoves; moves++) {
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
				int[] pos = tempPast.get(i);
				usePositions(pos,-1);
				int useIndex = getOneIndex(pos);
				for (int piece = 0; piece != 10; piece++) {
					attemptMoves(pos,piece,useIndex,4);
				}
			}
		}
		
		System.out.println("square\ttall1\ttall2\ttall3\ttall4\tsmall1\tsmall2\tsmall3\tsmall4\twide\tspace1\tspace2");
		for (int j = 0; j != miniMoves; j++) {
			for (int i = 0; i != 12; i++) {
				System.out.print(finalStates[j][i] + "\t");
			}
			int[] temp = undoOneIndex(finalIndices[j+1]);
			System.out.println("\t" + gameStateBefore[temp[0]][temp[1]][temp[2]][temp[3]]
					+ "\t" + whatMove(finalStates[j],finalStates[j+1]));
		}
		for (int i = 0; i != 12; i++) {
			System.out.print(finalStates[miniMoves][i] + "\t");
		}
	}
	
	public static int[] getBoard(int[] position) {
		int[] board = new int[20];
		board[position[0]] = 0;
		board[position[0]+1] = 0;
		board[position[0]+4] = 0;
		board[position[0]+5] = 0;
		board[position[1]] = 1;
		board[position[1]+4] = 1;
		board[position[2]] = 2;
		board[position[2]+4] = 2;
		board[position[3]] = 3;
		board[position[3]+4] = 3;
		board[position[4]] = 4;
		board[position[4]+4] = 4;
		board[position[5]] = 5;
		board[position[6]] = 6;
		board[position[7]] = 7;
		board[position[8]] = 8;
		board[position[9]] = 9;
		board[position[9]+1] = 9;
		board[position[10]] = -1;
		board[position[11]] = -1;
		return board;
	}
	
	public static int[] canMoveUp(int[] position, int piece, int index, boolean doNow) {
		if (position == null) {
			if (doNow) {
				myQueue = new HashMap<int[],Integer>();
			}
			return null;
		}
		int[] board = getBoard(position);
		int[] newPositions;
		if (piece == 0) {
			if (position[piece] > 3) {
				if (board[position[piece]-4] == -1) {
					if (board[position[piece]-3] == -1) {
						newPositions = Arrays.copyOf(position,12);
						newPositions[piece] -= 4;
						newPositions[10] += 8;
						newPositions[11] += 8;
						if (doNow) {
							addQueue();
							myQueue = new HashMap<int[],Integer>();
							if (usePositions(newPositions,index)) {
								tempAdd.add(newPositions);
							}
						}
						else {
							myQueue.put(newPositions,index);
						}
						return newPositions;
					}
					else {
						newPositions = canMoveUp(position,board[position[piece]-3],index,false);
						int newIndex = getOneIndex(newPositions);
						return canMoveUp(newPositions,piece,newIndex,doNow);
					}
				}
				else {
					newPositions = canMoveUp(position,board[position[piece]-4],index,false);
					int newIndex = getOneIndex(newPositions);
					return canMoveUp(newPositions,piece,newIndex,doNow);
				}
			}
			else {
				return null;
			}
		}
		else if (piece < 5) {
			if (position[piece] > 3) {
				if (board[position[piece]-4] == -1) {
					newPositions = Arrays.copyOf(position,12);
					newPositions[piece] -= 4;
					if (newPositions[piece] == newPositions[10]) {
						newPositions[10] += 8;
					}
					else {
						newPositions[11] +=8;
					}
					
					if (doNow) {
						addQueue();
						myQueue = new HashMap<int[],Integer>();
						if (usePositions(newPositions,index)) {
							tempAdd.add(newPositions);
						}
					}
					else {
						myQueue.put(newPositions,index);
					}
					return newPositions;
				}
				else {
					newPositions = canMoveUp(position,board[position[piece]-4],index,false);
					int newIndex = getOneIndex(newPositions);
					return canMoveUp(newPositions,piece,newIndex,doNow);
				}
			}
			else {
				return null;
			}
		}
		else if (piece == 9) {
			if (position[piece] > 3) {
				if (board[position[piece]-4] == -1) {
					if (board[position[piece]-3] == -1) {
						newPositions = Arrays.copyOf(position,12);
						newPositions[piece] -= 4;
						newPositions[10] += 4;
						newPositions[11] += 4;
						if (doNow) {
							addQueue();
							myQueue = new HashMap<int[],Integer>();
							if (usePositions(newPositions,index)) {
								tempAdd.add(newPositions);
							}
						}
						else {
							myQueue.put(newPositions,index);
						}
						return newPositions;
					}
					else {
						newPositions = canMoveUp(position,board[position[piece]-3],index,false);
						int newIndex = getOneIndex(newPositions);
						return canMoveUp(newPositions,piece,newIndex,doNow);
					}
				}
				else {
					newPositions = canMoveUp(position,board[position[piece]-4],index,false);
					int newIndex = getOneIndex(newPositions);
					return canMoveUp(newPositions,piece,newIndex,doNow);
				}
			}
			else {
				return null;
			}
		}
		else {
			if (position[piece] > 3) {
				if (board[position[piece]-4] == -1) {
					newPositions = Arrays.copyOf(position,12);
					newPositions[piece] -= 4;
					if (newPositions[piece] == newPositions[10]) {
						newPositions[10] += 4;
					}
					else {
						newPositions[11] +=4;
					}
					
					if (doNow) {
						addQueue();
						myQueue = new HashMap<int[],Integer>();
						if (usePositions(newPositions,index)) {
							tempAdd.add(newPositions);
						}
					}
					else {
						myQueue.put(newPositions,index);
					}
					return newPositions;
				}
				else {
					newPositions = canMoveUp(position,board[position[piece]-4],index,false);
					int newIndex = getOneIndex(newPositions);
					return canMoveUp(newPositions,piece,newIndex,doNow);
				}
			}
			else {
				return null;
			}
		}
	}
	
	public static int[] canMoveRight(int[] position, int piece, int index, boolean doNow) {
		if (position == null) {
			return null;
		}
		int[] board = getBoard(position);
		int[] newPositions;
		if (piece == 0) {
			if (position[piece]%4 < 2) {
				if (board[position[piece]+2] == -1) {
					if (board[position[piece]+6] == -1) {
						newPositions = Arrays.copyOf(position,12);
						newPositions[piece]++;
						newPositions[10] -= 2;
						newPositions[11] -= 2;
						if (doNow) {
							addQueue();
							myQueue = new HashMap<int[],Integer>();
							if (usePositions(newPositions,index)) {
								tempAdd.add(newPositions);
							}
						}
						else {
							myQueue.put(newPositions,index);
						}
						return newPositions;
					}
					else {
						newPositions = canMoveRight(position,board[position[piece]+6],index,false);
						int newIndex = getOneIndex(newPositions);
						return canMoveRight(newPositions,piece,newIndex,doNow);
					}
				}
				else {
					newPositions = canMoveRight(position,board[position[piece]+2],index,false);
					int newIndex = getOneIndex(newPositions);
					return canMoveRight(newPositions,piece,newIndex,doNow);
				}
			}
			else {
				return null;
			}
		}
		else if (piece == 9) {
			if (position[piece]%4 < 2) {
				if (board[position[piece]+2] == -1) {
					newPositions = Arrays.copyOf(position,12);
					if (newPositions[piece]+2 == newPositions[10]) {
						newPositions[10] -= 2;
					}
					else {
						newPositions[11] -= 2;
					}
					newPositions[piece]++;
					
					if (doNow) {
						addQueue();
						myQueue = new HashMap<int[],Integer>();
						if (usePositions(newPositions,index)) {
							tempAdd.add(newPositions);
						}
					}
					else {
						myQueue.put(newPositions,index);
					}
					return newPositions;
				}
				else {
					newPositions = canMoveRight(position,board[position[piece]+2],index,false);
					int newIndex = getOneIndex(newPositions);
					return canMoveRight(newPositions,piece,newIndex,doNow);
				}
			}
			else {
				return null;
			}
		}
		else if (piece < 5) {
			if (position[piece]%4 != 3) {
				if (board[position[piece]+1] == -1) {
					if (board[position[piece]+5] == -1) {
						newPositions = Arrays.copyOf(position,12);
						newPositions[piece]++;
						newPositions[10]--;
						newPositions[11]--;
						if (doNow) {
							addQueue();
							myQueue = new HashMap<int[],Integer>();
							if (usePositions(newPositions,index)) {
								tempAdd.add(newPositions);
							}
						}
						else {
							myQueue.put(newPositions,index);
						}
						return newPositions;
					}
					else {
						newPositions = canMoveRight(position,board[position[piece]+5],index,false);
						int newIndex = getOneIndex(newPositions);
						return canMoveRight(newPositions,piece,newIndex,doNow);
					}
				}
				else {
					newPositions = canMoveRight(position,board[position[piece]+1],index,false);
					int newIndex = getOneIndex(newPositions);
					return canMoveRight(newPositions,piece,newIndex,doNow);
				}
			}
			else {
				return null;
			}
		}
		else {
			if (position[piece]%4 != 3) {
				if (board[position[piece]+1] == -1) {
					newPositions = Arrays.copyOf(position,12);
					newPositions[piece]++;
					if (newPositions[piece] == newPositions[10]) {
						newPositions[10]--;
					}
					else {
						newPositions[11]--;
					}
					
					if (doNow) {
						addQueue();
						myQueue = new HashMap<int[],Integer>();
						if (usePositions(newPositions,index)) {
							tempAdd.add(newPositions);
						}
					}
					else {
						myQueue.put(newPositions,index);
					}
					return newPositions;
				}
				else {
					newPositions = canMoveRight(position,board[position[piece]+1],index,false);
					int newIndex = getOneIndex(newPositions);
					return canMoveRight(newPositions,piece,newIndex,doNow);
				}
			}
			else {
				return null;
			}
		}
	}
	
	public static int[] canMoveDown(int[] position, int piece, int index, boolean doNow) {
		if (position == null) {
			return null;
		}
		int[] board = getBoard(position);
		int[] newPositions;
		if (piece == 0) {
			if (position[piece] < 12) {
				if (board[position[piece]+8] == -1) {
					if (board[position[piece]+9] == -1) {
						newPositions = Arrays.copyOf(position,12);
						newPositions[piece] += 4;
						newPositions[10] -= 8;
						newPositions[11] -= 8;
						if (doNow) {
							addQueue();
							myQueue = new HashMap<int[],Integer>();
							if (usePositions(newPositions,index)) {
								tempAdd.add(newPositions);
							}
						}
						else {
							myQueue.put(newPositions,index);
						}
						return newPositions;
					}
					else {
						newPositions = canMoveDown(position,board[position[piece]+9],index,false);
						int newIndex = getOneIndex(newPositions);
						return canMoveDown(newPositions,piece,newIndex,doNow);
					}
				}
				else {
					newPositions = canMoveDown(position,board[position[piece]+8],index,false);
					int newIndex = getOneIndex(newPositions);
					return canMoveDown(newPositions,piece,newIndex,doNow);
				}
			}
			else {
				return null;
			}
		}
		else if (piece < 5) {
			if (position[piece] < 12) {
				if (board[position[piece]+8] == -1) {
					newPositions = Arrays.copyOf(position,12);
					if (newPositions[piece]+8 == newPositions[10]) {
						newPositions[10] -= 8;
					}
					else {
						newPositions[11] -=8;
					}
					newPositions[piece] += 4;
					
					if (doNow) {
						addQueue();
						myQueue = new HashMap<int[],Integer>();
						if (usePositions(newPositions,index)) {
							tempAdd.add(newPositions);
						}
					}
					else {
						myQueue.put(newPositions,index);
					}
					return newPositions;
				}
				else {
					newPositions = canMoveDown(position,board[position[piece]+8],index,false);
					int newIndex = getOneIndex(newPositions);
					return canMoveDown(newPositions,piece,newIndex,doNow);
				}
			}
			else {
				return null;
			}
		}
		else if (piece == 9) {
			if (position[piece] < 16) {
				if (board[position[piece]+4] == -1) {
					if (board[position[piece]+5] == -1) {
						newPositions = Arrays.copyOf(position,12);
						newPositions[piece] += 4;
						newPositions[10] -= 4;
						newPositions[11] -= 4;
						if (doNow) {
							addQueue();
							myQueue = new HashMap<int[],Integer>();
							if (usePositions(newPositions,index)) {
								tempAdd.add(newPositions);
							}
						}
						else {
							myQueue.put(newPositions,index);
						}
						return newPositions;
					}
					else {
						newPositions = canMoveDown(position,board[position[piece]+5],index,false);
						int newIndex = getOneIndex(newPositions);
						return canMoveDown(newPositions,piece,newIndex,doNow);
					}
				}
				else {
					newPositions = canMoveDown(position,board[position[piece]+4],index,false);
					int newIndex = getOneIndex(newPositions);
					return canMoveDown(newPositions,piece,newIndex,doNow);
				}
			}
			else {
				return null;
			}
		}
		else {
			if (position[piece] < 16) {
				if (board[position[piece]+4] == -1) {
					newPositions = Arrays.copyOf(position,12);
					newPositions[piece] += 4;
					if (newPositions[piece] == newPositions[10]) {
						newPositions[10] -= 4;
					}
					else {
						newPositions[11] -=4;
					}
					
					if (doNow) {
						addQueue();
						myQueue = new HashMap<int[],Integer>();
						if (usePositions(newPositions,index)) {
							tempAdd.add(newPositions);
						}
					}
					else {
						myQueue.put(newPositions,index);
					}
					return newPositions;
				}
				else {
					newPositions = canMoveDown(position,board[position[piece]+4],index,false);
					int newIndex = getOneIndex(newPositions);
					return canMoveDown(newPositions,piece,newIndex,doNow);
				}
			}
			else {
				return null;
			}
		}
	}
	
	public static int[] canMoveLeft(int[] position, int piece, int index, boolean doNow) {
		if (position == null) {
			return null;
		}
		int[] board = getBoard(position);
		int[] newPositions;
		if (piece == 0) {
			if (position[piece]%4 != 0) {
				if (board[position[piece]-1] == -1) {
					if (board[position[piece]+3] == -1) {
						newPositions = Arrays.copyOf(position,12);
						newPositions[piece]--;
						newPositions[10] += 2;
						newPositions[11] += 2;
						if (doNow) {
							addQueue();
							myQueue = new HashMap<int[],Integer>();
							if (usePositions(newPositions,index)) {
								tempAdd.add(newPositions);
							}
						}
						else {
							myQueue.put(newPositions,index);
						}
						return newPositions;
					}
					else {
						newPositions = canMoveLeft(position,board[position[piece]+3],index,false);
						int newIndex = getOneIndex(newPositions);
						return canMoveLeft(newPositions,piece,newIndex,doNow);
					}
				}
				else {
					newPositions = canMoveLeft(position,board[position[piece]-1],index,false);
					int newIndex = getOneIndex(newPositions);
					return canMoveLeft(newPositions,piece,newIndex,doNow);
				}
			}
			else {
				return null;
			}
		}
		else if (piece == 9) {
			if (position[piece]%4 != 0) {
				if (board[position[piece]-1] == -1) {
					newPositions = Arrays.copyOf(position,12);
					if (newPositions[piece]-1 == newPositions[10]) {
						newPositions[10] += 2;
					}
					else {
						newPositions[11] += 2;
					}
					newPositions[piece]--;
					
					if (doNow) {
						addQueue();
						myQueue = new HashMap<int[],Integer>();
						if (usePositions(newPositions,index)) {
							tempAdd.add(newPositions);
						}
					}
					else {
						myQueue.put(newPositions,index);
					}
					return newPositions;
				}
				else {
					newPositions = canMoveLeft(position,board[position[piece]-1],index,false);
					int newIndex = getOneIndex(newPositions);
					return canMoveLeft(newPositions,piece,newIndex,doNow);
				}
			}
			else {
				return null;
			}
		}
		else if (piece < 5) {
			if (position[piece]%4 != 0) {
				if (board[position[piece]-1] == -1) {
					if (board[position[piece]+3] == -1) {
						newPositions = Arrays.copyOf(position,12);
						newPositions[piece]--;
						newPositions[10]++;
						newPositions[11]++;
						if (doNow) {
							addQueue();
							myQueue = new HashMap<int[],Integer>();
							if (usePositions(newPositions,index)) {
								tempAdd.add(newPositions);
							}
						}
						else {
							myQueue.put(newPositions,index);
						}
						return newPositions;
					}
					else {
						newPositions = canMoveLeft(position,board[position[piece]+3],index,false);
						int newIndex = getOneIndex(newPositions);
						return canMoveLeft(newPositions,piece,newIndex,doNow);
					}
				}
				else {
					newPositions = canMoveLeft(position,board[position[piece]-1],index,false);
					int newIndex = getOneIndex(newPositions);
					return canMoveLeft(newPositions,piece,newIndex,doNow);
				}
			}
			else {
				return null;
			}
		}
		else {
			if (position[piece]%4 != 0) {
				if (board[position[piece]-1] == -1) {
					newPositions = Arrays.copyOf(position,12);
					newPositions[piece]--;
					if (newPositions[piece] == newPositions[10]) {
						newPositions[10]++;
					}
					else {
						newPositions[11]++;
					}
					
					if (doNow) {
						addQueue();
						myQueue = new HashMap<int[],Integer>();
						if (usePositions(newPositions,index)) {
							tempAdd.add(newPositions);
						}
					}
					else {
						myQueue.put(newPositions,index);
					}
					return newPositions;
				}
				else {
					newPositions = canMoveLeft(position,board[position[piece]-1],index,false);
					int newIndex = getOneIndex(newPositions);
					return canMoveLeft(newPositions,piece,newIndex,doNow);
				}
			}
			else {
				return null;
			}
		}
	}
	
	public static void addQueue() {
		Set<int[]> keys = myQueue.keySet();
		int[][] positions = keys.toArray(new int[0][]);
		for (int i = positions.length-1; i != -1; i--) {
			int index = myQueue.get(positions[i]);
			if (usePositions(positions[i],index)) {
				tempAdd.add(positions[i]);
			}
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
		
		if (positions[10]>positions[11]) {
			int temp = positions[10];
			positions[10] = positions[11];
			positions[11] = temp;
		}
		if (positions[0] == 13 && !useMoveNumbers) {
			if (gameStateBefore[bigIndex][tallIndex][smallIndex][wideIndex] == 0) {
				gameStateBefore[bigIndex][tallIndex][smallIndex][wideIndex] = previousIndex;
			}
			System.out.println("\nWon in " + moves + " moves:");
			printTo(positions);
			long end = System.nanoTime();
			double time = (end-start)/1000000000.0;
			System.out.println("\n" + time + " seconds");
			System.exit(0);
		}
		if (useMoveNumbers) {
			if (gameStateBefore[bigIndex][tallIndex][smallIndex][wideIndex] != -1) {
				return false;
			}
			else {
				gameStateBefore[bigIndex][tallIndex][smallIndex][wideIndex] = moves;
				return true;
			}
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
	
	public static int getOneIndex(int[] positions) {
		if (positions == null) {
			return -1;
		}
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
		int myIndex = oneIndex(bigIndex,tallIndex,smallIndex,wideIndex);
		return myIndex;
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