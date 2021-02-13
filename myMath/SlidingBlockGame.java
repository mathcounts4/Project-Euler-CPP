package myMath;

public class SlidingBlockGame {

	// http://www.tilepuzzles.com/default.asp?p=2
	
	static short[] square;
	static int minMoves;
	
	public static void main(String[] args) {
		long start = System.nanoTime();
		square = new short[9];
		// 0 1 2
		// 3 4 5
		// 6 7 8
		
		// initial game condition
		square[0] = 3;
		square[1] = 7;
		square[2] = 1;
		square[3] = 0;
		square[4] = 4;
		square[5] = 2;
		square[6] = 6;
		square[7] = 5;
		square[8] = 8;
		
		minMoves = 40;
		move(8,100,0,"");
		long end = System.nanoTime();
		double time = (end-start)/1000000000.0;
		System.out.println(time + " seconds");
	}
	
	public static void move(int index, int lastIndex, int moveNum, String moves) {
	//	System.out.println(moveNum + " moves");
		if (moveNum>minMoves) {
			return;
		}
		boolean win = true;
		for (int i = 0; i != 9; i++) {
			win &= (square[i]==i);
		}
		if (win) {
			minMoves = moveNum;
			System.out.println("won in " + moveNum + " moves:\n" + moves);
			return;
		}
		for (int i = 0; i != 9; i++) {
			if (i != lastIndex && ((i-3 == index || i+3 == index) ||
					(i-index == 1 && i%3 != 0) || (index-i == 1 && index%3 != 0))) {
				square[index] = square[i];
				square[i] = 8;
				boolean spareMove = (2*index == i + lastIndex);
				move(i,index,moveNum+(spareMove?0:1),moves + (spareMove?"*":" ") + i);
				square[i] = square[index];
				square[index] = 8;
			}
		}
	}
}