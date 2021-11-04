/*
 * My AI for this kind of sucks - would be much better to do a mini-max algorithm,
 * or to include real strategy like moving to create potential wins that hav space under them,
 * so the opponent can no longer play in that column. Then also parity for this for a second column,
 * that way if both players are forced into the two columns, the computer wins
 */

package myMath;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.GridLayout;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.util.ArrayList;

import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;

public class ConnectFour {
	
	static Window window;
	
	public static void main(String[] args) {
		window = new Window();
		window.setSize(660,533);
		window.setResizable(false);
		window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		window.setVisible(true);
	}
	
	@SuppressWarnings("serial")
	private static class Window extends JFrame {
		static String hvh, hvc, cvh, cvc;
		static String currentPlayType;
		static JComboBox dropDown, dropDown2;
		static int border = 4;
		static boolean computerMove, animations;
		
		public Window() {
			animations = true;
			this.setLayout(new BorderLayout());
			Board board = new Board();
			JButton restart = new JButton("Restart");
			restart.addActionListener(new ActionListener() {

				@Override
				public void actionPerformed(ActionEvent e) {
					window.setVisible(false);
					window = new Window();
					window.setSize(660,533);
					window.setResizable(false);
					window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
					window.setVisible(true);
				}
			});
			Options multiplayer = new Options(board);
			this.add(board,BorderLayout.CENTER);
			this.add(restart,BorderLayout.EAST);
			this.add(multiplayer,BorderLayout.NORTH);
		}
		
		private static class Board extends JPanel {
			
			static ArrayList<Move> moves;
			static boolean[][] red, blue;
			static boolean potential, animating, won;
			static int potentialX, potentialY;
			static Color nextMoveColor, lightBlue, lightRed;
			static int animateX, animateY;
			static ArrayList<Line>[][] linesAt;
			static ArrayList<Line> potentialBlueLines, potentialRedLines;
			static int[] highestFree;
			static final int[] pow = {1,14,196,2744,38416,537824,7529536,105413504};
			// each spot can have a max of 13 lines through it
			
			@SuppressWarnings({ "unchecked", "unused" })
			public Board() {
				red = new boolean[7][6];
				blue = new boolean[7][6];
				moves = new ArrayList<Move>();
				potential = false;
				animating = false;
				won = false;
				lightRed = new Color((Color.RED.getRed()+2*255)/3,(Color.RED.getGreen()+2*255)/3,(Color.RED.getBlue()+2*255)/3);
				lightBlue = new Color((Color.BLUE.getRed()+2*255)/3,(Color.BLUE.getGreen()+2*255)/3,(Color.BLUE.getBlue()+2*255)/3);
				nextMoveColor = Color.BLUE;
				computerMove = false;
				highestFree = new int[7];
				for (int i = 0; i != 7; i++) {
					highestFree[i] = 5;
				}
				
				linesAt = new ArrayList[7][6];
				for (int i = 0; i != 7; i++) {
					for (int j = 0; j != 6; j++) {
						linesAt[i][j] = new ArrayList<Line>();
					}
				}
				potentialBlueLines = new ArrayList<Line>();
				potentialRedLines = new ArrayList<Line>();
				
				for (int i = 0; i != 7; i++) {
					for (int j = 0; j != 6; j++) {
						if (i < 4) {
							Line l = new Line(i,j,i+1,j,i+2,j,i+3,j);
							if (j < 3) {
								Line m = new Line(i,j,i+1,j+1,i+2,j+2,i+3,j+3);
							} else {
								Line m = new Line(i,j,i+1,j-1,i+2,j-2,i+3,j-3);
							}
						}
						if (j < 3) {
							Line l = new Line(i,j,i,j+1,i,j+2,i,j+3);
						}
					}
				}
				this.addMouseMotionListener(new MouseMotionListener() {

					@Override
					public void mouseDragged(MouseEvent e) {
						if (!computerMove && !won) {
							int x = e.getX()/80;
							if (x < 0 || x > 6) {
								return;
							}
							
							if (!red[x][0] && !blue[x][0]) {
								if (!animating) {
									potential = true;
								}
								potentialX = x;
								potentialY = 0;
								for (int i = 1; i != 6; i++) {
									if (!red[x][i] && !blue[x][i]) {
										potentialY = i;
									} else {
										break;
									}
								}
								if (!animating) {
									repaint();
								}
							} else {
								potential = false;
								if (!animating) {
									repaint();
								}
							}
						}
					}

					@Override
					public void mouseMoved(MouseEvent e) {
						if (!computerMove && !won) {
							int x = e.getX()/80;
							if (x < 0 || x > 6) {
								return;
							}
							
							if (!red[x][0] && !blue[x][0]) {
								if (!animating) {
									potential = true;
								}
								potentialX = x;
								potentialY = 0;
								for (int i = 1; i != 6; i++) {
									if (!red[x][i] && !blue[x][i]) {
										potentialY = i;
									} else {
										break;
									}
								}
								if (!animating) {
									repaint();
								}
							} else {
								potential = false;
								if (!animating) {
									repaint();
								}
							}
						}
					}
				});
				this.addMouseListener(new MouseListener() {

					@Override
					public void mouseClicked(MouseEvent e) {}

					@Override
					public void mouseEntered(MouseEvent e) {}

					@Override
					public void mouseExited(MouseEvent e) {
						potential = false;
						if (!animating) {
							repaint();
						}
					}

					@Override
					public void mousePressed(MouseEvent e) {}
					
					@Override
					public void mouseReleased(MouseEvent e) {
						if (won || animating) {
							return;
						}
						
						int x = e.getX()/80;
						if (x < 0 || x > 6) {
							return;
						}
						
						if (!red[x][0] && !blue[x][0]) {
							int moveX = x;
							int moveY = 0;
							for (int i = 1; i != 6; i++) {
								if (!red[x][i] && !blue[x][i]) {
									moveY = i;
								} else {
									break;
								}
							}
							move(moveX,moveY,nextMoveColor);
						}
					}
				});
			}
			
			public void paintComponent(Graphics g) {
				super.paintComponent(g);
				g.setColor(Color.BLACK);
				g.fillRect(0, 0, 560, 480);
				g.setColor(Color.WHITE);
				for (int i = 0; i != 7; i++) {
					for (int j = 0; j != 6; j++) {
						g.fillOval(80*i+border, 80*j+border, 80-2*border, 80-2*border);
					}
				}
				int moveNum = moves.size();
				for (int i = 0; i != moveNum; i++) {
					Move m = moves.get(i);
					g.setColor(m.color);
					g.fillOval(80*m.x+border, 80*m.y+border, 80-2*border, 80-2*border);
				}
				if (animating) {
					int radius = 40-border;
					int radSq = radius*radius;
					g.setColor(nextMoveColor);
					int yTopCenter = (animateY/80)*80 + 40;
					int yBottomCenter = yTopCenter + 80;
					for (int i = border; i != 41; i++) {
						int x1 = animateX + i;
						int x2 = animateX + 80 - i;
						int rt = (int) Math.sqrt(radSq - (40-i)*(40-i));
						int yBotTopCircle = yTopCenter + rt;
						int yTopBotCircle = yBottomCenter - rt;
						for (int j = animateY+40-rt; j <= yBotTopCircle; j++) {
							g.drawLine(x1,j,x1,j);
							g.drawLine(x2,j,x2,j);
						}
						for (int j = animateY+40+rt; j >= yTopBotCircle; j--) {
							g.drawLine(x1,j,x1,j);
							g.drawLine(x2,j,x2,j);
						}
					}
				} else if (potential && !won) {
					g.setColor(lighter(nextMoveColor));
					g.fillOval(80*potentialX+border, 80*potentialY+border, 80-2*border, 80-2*border);
				}
			}
			
			public void changePlayType(String s) {
				if (currentPlayType.equals(s)) {
					return;
				} else {
					currentPlayType = s;
					if (won) {
						return;
					}
					if (s.equals(hvh)) {
						computerMove = false;
					} else if (s.equals(hvc)) {
						if (animating) {
							if (nextMoveColor == Color.BLUE) {
								computerMove = true;
							} else {
								computerMove = false;
							}
						} else {
							if (nextMoveColor == Color.BLUE) {
								computerMove = false;
							} else {
								computerMove = true;
								makeComputerMove();
							}
						}
					} else if (s.equals(cvh)) {
						if (animating) {
							if (nextMoveColor == Color.RED) {
								computerMove = true;
							} else {
								computerMove = false;
							}
						} else {
							if (nextMoveColor == Color.RED) {
								computerMove = false;
							} else {
								computerMove = true;
								makeComputerMove();
							}
						}
					} else if (s.equals(cvc)) {
						computerMove = true;
						if (!animating) {
							makeComputerMove();
						}
					}
				}
			}
			
			public void makeComputerMove() {
				if (!currentPlayType.equals(cvc)) {
					computerMove = false;
				}
				
				Point p = getMove(red,blue,nextMoveColor);
				move(p.x,p.y,nextMoveColor);
			}
			
			public void move(int x, int y, Color c) {
				MoveThread t = new MoveThread(x,y,c);
				t.start();
			}
			
			public Color lighter(Color c) {
				if (c == Color.RED) {
					return lightRed;
				} else {
					return lightBlue;
				}
			}
			
			public Point getMove(boolean[][] oldRed, boolean[][] oldBlue, Color c) {
				
				int maxLinesScore = -1;
				ArrayList<Integer> bestColumns = new ArrayList<Integer>();
				
				if (c == Color.BLUE) {
					for (int i = 0; i != 7; i++) {
						if (highestFree[i] == -1) {
							continue;
						}
						int linesScore = 0;
						ArrayList<Line> list = linesAt[i][highestFree[i]];
						for (int j = list.size()-1; j != -1; j--) {
							Line l = list.get(j);
							if (l.bluePossible) {
								linesScore += pow[2*l.blueTotal+1];
							} else if (l.redPossible) {
								linesScore += pow[2*l.redTotal];
							}
						}
						if (highestFree[i] != 0 && checkForWin(red,i,highestFree[i]-1)) {
							linesScore = -1;
						}
						if (maxLinesScore < linesScore) {
							maxLinesScore = linesScore;
							bestColumns = new ArrayList<Integer>();
							bestColumns.add(i);
						} else if (maxLinesScore == linesScore) {
							bestColumns.add(i);
						}
					}
				} else {
					for (int i = 0; i != 7; i++) {
						if (highestFree[i] == -1) {
							continue;
						}
						int linesScore = 0;
						ArrayList<Line> list = linesAt[i][highestFree[i]];
						for (int j = list.size()-1; j != -1; j--) {
							Line l = list.get(j);
							if (l.redPossible) {
								linesScore += pow[2*l.redTotal+1];
							} else if (l.bluePossible) {
								linesScore += pow[2*l.blueTotal];
							}
						}
						if (highestFree[i] != 0 && checkForWin(blue,i,highestFree[i]-1)) {
							linesScore = -1;
						}
						if (maxLinesScore < linesScore) {
							maxLinesScore = linesScore;
							bestColumns = new ArrayList<Integer>();
							bestColumns.add(i);
						} else if (maxLinesScore == linesScore) {
							bestColumns.add(i);
						}
					}
				}
				
				int q = (int)(Math.random()*bestColumns.size());
				int column = bestColumns.get(q);
				return new Point(column,highestFree[column]);
			}
			
			private class Line {
				private boolean bluePossible, redPossible;
				private int blueTotal, redTotal;
				
				public Line (int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3) {
					bluePossible = true;
					redPossible = true;
					blueTotal = 0;
					redTotal = 0;
					linesAt[x0][y0].add(this);
					linesAt[x1][y1].add(this);
					linesAt[x2][y2].add(this);
					linesAt[x3][y3].add(this);
					potentialBlueLines.add(this);
					potentialRedLines.add(this);
				}
			}
			
			private class MoveThread extends Thread {
				private int x,y;
				private Color c;
				public MoveThread(int x, int y, Color c) {
					this.x = x;
					this.y = y;
					this.c = c;
				}
				
				public void run() {
					if (c == Color.RED) {
						red[x][y] = true;
						for (int i = linesAt[x][highestFree[x]].size()-1; i != -1; i--) {
							Line l = linesAt[x][highestFree[x]].get(i);
							l.bluePossible = false;
							l.redTotal++;
						}
					} else {
						blue[x][y] = true;
						for (int i = linesAt[x][highestFree[x]].size()-1; i != -1; i--) {
							Line l = linesAt[x][highestFree[x]].get(i);
							l.redPossible = false;
							l.blueTotal++;
						}
					}
					
					
					highestFree[x]--;
					
					if (x == potentialX) {
						if (y == 0) {
							potential = false;
						} else {
							potentialY--;
						}
					}
					if (animations) {
						animating = true;
						int z = 80*y;
						animateX = 80*x;
						for (animateY = 0; animateY != z; animateY += 2) {
							try {
								sleep(5);
							} catch (InterruptedException e) {}
							repaint();
						}
						animating = false;
					}
					Move m = new Move(x,y,c);
					moves.add(m);
					repaint();
					if (c == Color.RED) {
						if (checkForWin(red,x,y)) {
							won = true;
							repaint();
							JOptionPane.showMessageDialog(null,"Red has won");
						}
					} else {
						if (checkForWin(blue,x,y)) {
							won = true;
							repaint();
							JOptionPane.showMessageDialog(null,"Blue has won");
						}
					}
					if (moves.size() == 42 && !won) {
						won = true;
						repaint();
						JOptionPane.showMessageDialog(null,"No one won! It's a tie!");
					}
					
					if (!won) {
						if (nextMoveColor == Color.BLUE) {
							nextMoveColor = Color.RED;
							
						} else {
							nextMoveColor = Color.BLUE;
						}
						
						if (currentPlayType.equals(cvc)) {
							makeComputerMove();
						} else if (currentPlayType.equals(hvc) && nextMoveColor == Color.RED) {
							makeComputerMove();
						} else if (currentPlayType.equals(cvh) && nextMoveColor == Color.BLUE) {
							makeComputerMove();
						}
					}
				}
			}
			
			private static class Move {
				
				public int x, y;
				public Color color;
				
				public Move(int x, int y, Color color) {
					this.x = x;
					this.y = y;
					this.color = color;
				}
			}
			
			public static boolean checkForWin(boolean[][] pieces, int x, int y) {
				if (x < 4) {
					if (pieces[x+1][y] && pieces[x+2][y] && pieces[x+3][y]) {
						return true;
					}
					if (y < 3) {
						if (pieces[x+1][y+1] && pieces[x+2][y+2] && pieces[x+3][y+3]) {
							return true;
						}
					} else {
						if (pieces[x+1][y-1] && pieces[x+2][y-2] && pieces[x+3][y-3]) {
							return true;
						}
					}
				}
				if (x > 0 && x < 5) {
					if (pieces[x-1][y] && pieces[x+1][y] && pieces[x+2][y]) {
						return true;
					}
					if (y > 0 && y < 4) {
						if (pieces[x-1][y-1] && pieces[x+1][y+1] && pieces[x+2][y+2]) {
							return true;
						}
					}
					if (y > 1 && y < 5) {
						if (pieces[x-1][y+1] && pieces[x+1][y-1] && pieces[x+2][y-2]) {
							return true;
						}
					}
				}
				if (x > 1 && x < 6) {
					if (pieces[x-2][y] && pieces[x-1][y] && pieces[x+1][y]) {
						return true;
					}
					if (y > 1 && y < 5) {
						if (pieces[x-2][y-2] && pieces[x-1][y-1] && pieces[x+1][y+1]) {
							return true;
						}
					}
					if (y > 0 && y < 4) {
						if (pieces[x-2][y+2] && pieces[x-1][y+1] && pieces[x+1][y-1]) {
							return true;
						}
					}
				}
				if (x > 2) {
					if (pieces[x-3][y] && pieces[x-2][y] && pieces[x-1][y]) {
						return true;
					}
					if (y > 2) {
						if (pieces[x-3][y-3] && pieces[x-2][y-2] && pieces[x-1][y-1]) {
							return true;
						}
					} else {
						if (pieces[x-3][y+3] && pieces[x-2][y+2] && pieces[x-1][y+1]) {
							return true;
						}
					}
				}
				if (y < 3) {
					if (pieces[x][y+1] && pieces[x][y+2] && pieces[x][y+3]) {
						return true;
					}
				}
				return false;
			}
		}
		
		private class Options extends JPanel {
			private Board board;
			
			public Options(Board board) {
				this.board = board;
				this.setLayout(new GridLayout(1,4));
				JLabel typeLabel = new JLabel("Players (Blue vs Red)");
				typeLabel.setHorizontalAlignment(JLabel.RIGHT);
				String[] choices = {"Human vs Human","Human vs Comp","Comp vs Human","Comp vs Comp"};
				hvh = choices[0];
				currentPlayType = hvh;
				hvc = choices[1];
				cvh = choices[2];
				cvc = choices[3];
				dropDown = new JComboBox(choices);
				dropDown.addActionListener(new DropDownListener());
				JLabel animationsLabel = new JLabel("Animations");
				animationsLabel.setHorizontalAlignment(JLabel.RIGHT);
				String[] choices2 = {"On","Off"};
				dropDown2 = new JComboBox(choices2);
				dropDown2.addActionListener(new AnimationDropDownListener());
				this.add(typeLabel);
				this.add(dropDown);
				this.add(animationsLabel);
				this.add(dropDown2);
			}
			
			private class DropDownListener implements ActionListener {
				@Override
				public void actionPerformed(ActionEvent e) {
					String s = dropDown.getSelectedItem().toString();
					board.changePlayType(s);
				}
			}
			
			private class AnimationDropDownListener implements ActionListener {
				@Override
				public void actionPerformed(ActionEvent e) {
					String s = dropDown2.getSelectedItem().toString();
					if (s.equals("On")) {
						animations = true;
					} else {
						animations = false;
					}
				}
			}
		}
	}
}