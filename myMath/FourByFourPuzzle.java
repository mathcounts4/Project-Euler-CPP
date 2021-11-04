package myMath;

import java.awt.AWTException;
import java.awt.Rectangle;
import java.awt.Robot;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Scanner;
/*
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
*/
public class FourByFourPuzzle {

	// for screen version:
	// http://www.tilepuzzles.com/default.asp?p=14
	
	// http://www.tilepuzzles.com/default.asp?p=4

	// with dist+moveNum <= 37:
	// 31 moves:
	// 14*13*12 8*4 5*6 10*14 13 9*5*1 2*3 7*11*15 14*13
	// 9*5 6*7 11*15 14*13*12 8*4 5*6*7 11*15 14 10*6*2
	// 1*0 4 5 9 8 12 13*14 10*6*2 1*0 4*8 9*10 14 15
	// 1.036486 seconds

	// with dist*2+moveNum*3 <= 95:
	// 31 moves:
	// 14*13*12 8 9*10*11 7 6*5 9 10*11 7*3 2*1 5*9*13
	// 14*15 11 10*9*8 4 5*6*7 11 10*9 5*1 2*3 7*11 10
	// 6*2 1*0 4*8 9*10 6*2 1 5 6 10*14 15
	// 1.973888 seconds

	static Short[] square;
	static int minMoves;
	static boolean notWon;
	static HashMap<List<Short>, Integer> map;
	static int total;
	static int a, b, c;
	static long start;
	static boolean screen;
	static boolean actuallyFound;

	public static void playSound() {
		AudioPlayer.play("Shorter.wav");
	}
	
	public static void main(String[] args) {
		notWon = true;
		screen = false;
		total = 0;
		map = new HashMap<List<Short>, Integer>();
		square = new Short[16];
		Byte[] n = new Byte[16];
		ArrayList<Byte[]> gameStates = new ArrayList<Byte[]>();

		// 0 1 2 3
		// 4 5 6 7
		// 8 9 10 11
		// 12 13 14 15

		// initial game condition
		n[0] = 2;
		n[1] = 5;
		n[2] = 6;
		n[3] = 9;
		n[4] = 4;
		n[5] = 8;
		n[6] = 14;
		n[7] = 3;
		n[8] = 12;
		n[9] = 11;
		n[10] = 1;
		n[11] = 0;
		n[12] = 7;
		n[13] = 13;
		n[14] = 10;
		n[15] = 15;
		minMoves = 40;
		System.out.println("don't want to use defaults? Enter \"mine\" or \"screen\"");
		Scanner scan = new Scanner(System.in);
		boolean set = true;
		String s;
		if ((s = scan.nextLine()).equals("mine")) {
			set = false;
			for (int i = 0; i != 16; i++) {
				n[i] = scan.nextByte();
			}
		//	set = true;
		} else if (s.equals("screen")) {
			screen = true;
			Robot robot;
			try {
				robot = new Robot();
				robot.mouseMove(550,75);
				System.out.println("Make sure your browser bar is where your mouse just moved,"
						+ " with nothing in the way");
				scan.nextLine();
// THIS NEXT LINE IS IMPORTANT - WILL PAUSE WHEN IT GETS A SOLUTION LESS THAN THIS
				int realMin = 30;
				int duplicates = 0;
				
				newGameLoop:
				while (true) {
					if (minMoves < realMin && actuallyFound) {
						if (minMoves == 0) {
							minMoves = 40;
							continue newGameLoop;
						}
						Scanner myScan = new Scanner(System.in);
						realMin = minMoves;
						System.out.println("New minimum of " + realMin
								+ "! Enter \"q\" to exit or anything else to continue.");
						System.out.println("Make sure this window is out of the way "
								+ "and a NEW unsolved puzzle is up");
						System.out.println("By the way, there have now been " + gameStates.size()
								+ " games and " + duplicates + " duplicates");
						
						playSound();
						
						String string = myScan.nextLine();
						if (string.equals("q")) {
							System.exit(0);
						}
					}
					start = System.nanoTime();
					robot.mouseMove(550,75);
					int mouseButton = InputEvent.BUTTON1_MASK;
					robot.mousePress(mouseButton);
					robot.mouseRelease(mouseButton);
					robot.keyPress(KeyEvent.VK_ENTER);
					robot.keyRelease(KeyEvent.VK_ENTER);
					
					System.out.println("next one (waiting)...");
					
					Thread.sleep(2000);
					
					System.out.println("next one:");
					
					BufferedImage img = robot.createScreenCapture(new Rectangle(0,0,800,700));
					
					int[] myPixels = new int[16];
					
					int imgX = 0; // x = 213
					int imgY = 0; // y = 322
					int[] solutionPixels = {-917504, -16515035, -16777142, -16777101, -778752, -834304, -627200, -16777061, -1030400, -562176, -745984, -16777000, -16777108, -16777054, -276992, -5000269};
					/*
					int[] array = new int[16];
					for (int y = 305; y != 370; y++) {
						next:
						for (int x = 166; x != 231; x++) {
							for (int i = 0; i != 16; i++) {
								array[i] = img.getRGB(x + 64*(i%4), y + 64*(i/4));
							}
							int[] copy = Arrays.copyOf(array,16);
							Arrays.sort(copy);
							for (int j = 0; j != 15; j++) {
								if (copy[j] == copy[j+1]) {
									continue next;
								}
							}
							System.out.println(x + " " + y);
							for (int j = 0; j != 16; j++) {
								System.out.print(array[j] + " ");
							}
							System.exit(0);
						}
					}
					System.out.println("fuck");
					System.exit(0);
					*/
					
					
					
					bigLoop:
					while (true) {
						if (imgX < 800-192) {
							imgX++;
						}
						else if (imgY < 700-192) {
							imgY++;
							imgX = 0;
						}
						else {
							System.out.println("didn't find it");
							continue newGameLoop;
						}
				//		System.out.println("imgX = " + imgX + " and imgY = " + imgY);
						boolean found;
						
						int myX = imgX;
						int myY = imgY;
						boolean[] used = new boolean[16];
						for (int index = 0; index != 16; index++) {
							myPixels[index] = img.getRGB(myX,myY);
							int find = myPixels[index];
							found = false;
							for (byte j = 0; j != 16; j++) {
								if (!used[j] && solutionPixels[j] == find) {
									n[index] = j;
									used[j] = true;
									found = true;
									break;
								}
							}
							if (!found) {
								continue bigLoop;
							}
					//		System.out.println("got through " + (index+1) + " squares with imgX = "
					//				+ imgX + " and imgY = " + imgY + " and square " + n[index]);
							if (index % 4 == 3) {
								myX -= 192;
								myY += 64;
							} else {
								myX += 64;
							}
						}
						
						outerLoop:
						for (int i = gameStates.size()-1; i != -1; i--) {
							Byte[] old = gameStates.get(i);
							for (int j = 0; j != 15; j++) {
								if (old[j] != n[j]) {
									continue outerLoop;
								}
							}
							duplicates++;
							System.out.println("There have now been " + gameStates.size() + " games and "
									+ duplicates + " duplicates");
							continue newGameLoop;
						}
						gameStates.add(Arrays.copyOf(n,15));
						System.out.println("There have now been " + gameStates.size() + " games and "
								+ duplicates + " duplicates");
						
						System.out.println("Found it! imgX = " + imgX + " and imgY = " + imgY);
						break bigLoop;
					}
					
			/*		System.out.println("myPixels:");
					for (int i = 0; i != 16; i++) {
						System.out.print(myPixels[i] + ",");
						if (i%4 == 3) {
							System.out.println();
						}
					}
					
					System.out.println("solutionPixels:");
					for (int i = 0; i != 16; i++) {
						System.out.print(solutionPixels[i] + ",");
						if (i%4 == 3) {
							System.out.println();
						}
					} */
					
					for (int i = 0; i != 16; i++) {
						System.out.print(n[i] + " ");
					}
					System.out.println();
					actuallyFound = false;
					
					int trueMin = 40;
					
					a = 1;
					b = 1;
					for (int i = 0; i != 16; i++) {
						square[i] = (short)n[i];
					}
			//		System.out.println("a = " + a + ", b = " + b + ", and dist = " + dist());
					int d = dist();
					notWon = true;
					minMoves = d * 3 / 2;
					for (c = d * a + 1; notWon && System.nanoTime()-start < 10000000000L; c++) {
			//			System.out.println(c);
						int space = -1;
						for (int i = 0; i != 16; i++) {
							square[i] = (short)n[i];
							if (square[i] == 15) {
								space = i;
								break;
							}
						}
						map = new HashMap<List<Short>, Integer>();
						move(space, 100, 0, "");
					}
			//		System.out.println("c was " + c);
					if (trueMin > minMoves) {
			//			System.out.println("new min is " + minMoves);
						trueMin = minMoves;
					}
				}
			} catch (AWTException e) {
				System.out.println("robot fucked up. Exiting.");
				System.exit(0);
			} catch (InterruptedException e) {
			}
		}
		
		minMoves = 40;
		int trueMin = 40;
		if (set) {
			for (a = 4; a != 7; a++) {
				for (b = a; b != a+2; b++) {
					if (gcd(a, b) != 1 && a != 4) {
						continue;
					}
					System.out.println("a = " + a + " and b = " + b);
					notWon = true;
				//	minMoves = 40;
					long startTime = System.nanoTime();
					long endTime = System.nanoTime();
					for (c = 30 * a + 1; notWon && (endTime-startTime)/1000000000.0 < 2; c++) {
						startTime = System.nanoTime();
						for (int i = 0; i != 16; i++) {
							square[i] = (short)n[i];
						}
						map = new HashMap<List<Short>, Integer>();
						move(15, 100, 0, "");
						endTime = System.nanoTime();
					}
					System.out.println("c was " + c);
					if (trueMin > minMoves) {
				//		System.out.println("new min is " + minMoves);
						trueMin = minMoves;
					}
				}
			}
		} else {
			a = 1;
			b = 1;
			for (int i = 0; i != 16; i++) {
				square[i] = (short)n[i];
			}
	//		System.out.println("a = " + a + ", b = " + b + ", and dist = " + dist());
			int d = dist();
			notWon = true;
			minMoves = Math.max(d * 3 / 2,25);
	//		minMoves = 19;
			System.out.println("since dist = " + d + ", minMoves = " + minMoves);
			int cMax = 3*d*a;
			for (c = d * a + 1; notWon && c < cMax; c++) {
	//			System.out.println(c);
				int space = -1;
				for (int i = 0; i != 16; i++) {
					square[i] = (short)n[i];
					if (square[i] == 15) {
						space = i;
					}
				}
				map = new HashMap<List<Short>, Integer>();
				move(space, 100, 0, "");
			}
	//		System.out.println("c was " + c);
			if (trueMin > minMoves) {
	//			System.out.println("new min is " + minMoves);
				trueMin = minMoves;
			}
		}

		System.out.println("minMoves = " + minMoves);
		long end = System.nanoTime();
		double time = (end - start) / 1000000000.0;
		System.out.println(time + " seconds");
	}

	public static int dist() {
		int d = 0;
		for (int i = 0; i != 16; i++) {
			int p = square[i];
			if (p != 15) {
				if (p / 4 > i / 4) {
					d += ((p / 4) - (i / 4));
				} else {
					d += ((i / 4) - (p / 4));
				}
				if (p % 4 > i % 4) {
					d += ((p % 4) - (i % 4));
				} else {
					d += ((i % 4) - (p % 4));
				}
			}
		}
		return d;
	}

	public static void move(int index, int lastIndex, int moveNum, String moves) {
		if (/*!screen &&*/ dist() * a + moveNum * b > c) {
			// init dist = 30, and I knew it'd be <50 moves (I got 50), so
			// around 150+ here
			// had dist()*5 + moveNum*3>156

			// once I got a solution of 31 moves,
			// tried dist()+moveNum>35 (none), 40 (see above), 36 (none) 37 (see
			// above)
			return;
		}
		if (moveNum >= minMoves) {
			// printBoard();
			return;
		}
		if (System.nanoTime()-start > 10000000000L && (start != 0)) {
			return;
		}
		// System.out.println(moves);
		// System.out.println(moveNum);
		boolean win = true;
		for (int i = 0; i != 16; i++) {
			win &= (square[i] == i);
		}
		if (win) {
			notWon = false;
			actuallyFound = true;
			minMoves = moveNum;
			System.out.println("won in " + moveNum + " moves:\n" + moves);
	//		Scanner scan = new Scanner(System.in);
	//		System.out.println("enter anything to continue");
	//		scan.nextLine();
	//		System.out.println("continuing");
			return;
		}
		List<Short> list = Arrays.asList(square);
		if (map.containsKey(list) && map.get(list) <= moveNum) {
			return;
		}
		map.put(list, moveNum);
		total++;
		// System.out.println(total);
		if (index - 4 != lastIndex && index > 3
				&& square[index - 4] / 4 >= index / 4) {
			int newIndex = index - 4;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2 * index == newIndex + lastIndex);
			move(newIndex, index, moveNum + (spareMove ? 0 : 1), moves
					+ (spareMove ? "*" : " ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
		if (index - 1 != lastIndex && index % 4 != 0
				&& square[index - 1] % 4 >= index % 4) {
			int newIndex = index - 1;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2 * index == newIndex + lastIndex);
			move(newIndex, index, moveNum + (spareMove ? 0 : 1), moves
					+ (spareMove ? "*" : " ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
		if (index + 1 != lastIndex && index % 4 != 3
				&& square[index + 1] % 4 <= index % 4) {
			int newIndex = index + 1;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2 * index == newIndex + lastIndex);
			move(newIndex, index, moveNum + (spareMove ? 0 : 1), moves
					+ (spareMove ? "*" : " ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
		if (index + 4 != lastIndex && index < 12
				&& square[index + 4] / 4 <= index / 4) {
			int newIndex = index + 4;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2 * index == newIndex + lastIndex);
			move(newIndex, index, moveNum + (spareMove ? 0 : 1), moves
					+ (spareMove ? "*" : " ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
		if (index - 1 != lastIndex && index % 4 != 0
				&& square[index - 1] % 4 < index % 4) {
			int newIndex = index - 1;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2 * index == newIndex + lastIndex);
			move(newIndex, index, moveNum + (spareMove ? 0 : 1), moves
					+ (spareMove ? "*" : " ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
		if (index - 4 != lastIndex && index > 3
				&& square[index - 4] / 4 < index / 4) {
			int newIndex = index - 4;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2 * index == newIndex + lastIndex);
			move(newIndex, index, moveNum + (spareMove ? 0 : 1), moves
					+ (spareMove ? "*" : " ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
		if (index + 1 != lastIndex && index % 4 != 3
				&& square[index + 1] % 4 > index % 4) {
			int newIndex = index + 1;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2 * index == newIndex + lastIndex);
			move(newIndex, index, moveNum + (spareMove ? 0 : 1), moves
					+ (spareMove ? "*" : " ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
		if (index + 4 != lastIndex && index < 12
				&& square[index + 4] / 4 > index / 4) {
			int newIndex = index + 4;
			square[index] = square[newIndex];
			square[newIndex] = 15;
			boolean spareMove = (2 * index == newIndex + lastIndex);
			move(newIndex, index, moveNum + (spareMove ? 0 : 1), moves
					+ (spareMove ? "*" : " ") + newIndex);
			square[newIndex] = square[index];
			square[index] = 15;
		}
	}

	public static void printBoard() {
		for (int i = 0; i != 16; i++) {
			if (i % 4 == 0) {
				System.out.println();
			}
			System.out.print(square[i] + "\t");
		}
		Scanner scan = new Scanner(System.in);
		scan.nextLine();
	}

	public static int gcd(int a, int b) {
		if (b % a == 0) {
			return a;
		} else {
			return gcd(b % a, a);
		}
	}
}