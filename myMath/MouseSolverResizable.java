package myMath;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.GridLayout;
import java.awt.Image;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.image.BufferedImage;
import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;

public class MouseSolverResizable {
	
	static MousePanel panel;
	
	private enum TileType {
		OneByThree, OneByTwo, TwoByOne, ThreeByOne, Mouse, Delete, DoNothing, DeleteAll
	}
	
	public static void main(String[] args) {
		javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                RunIt();
            }
        });
	}
	
	public static void RunIt() {
		JFrame frame = new JFrame();
		frame.setMinimumSize(new Dimension(575,400));
		panel = new MousePanel();
		panel.setFocusable(true);
		
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.add(panel);
		frame.setSize(800,580);
		frame.setVisible(true);
	}

	@SuppressWarnings("serial")
	private static class MousePanel extends JPanel {

		private static ArrayList<Tile> tiles, storedTiles;
		private static Tile[][] used, storedUsed;
		private static int mouseX, mouseY, moveLimit, pieces;
		private static TileType currentTileType;
		private static JTextField moveLimitField;
		private static int arrowStart;
		private static int arrowEnd;
		private static int arrowShift;
		private static JLabel moveLimitLabel;
		private static JButton twoByOne, threeByOne, oneByTwo, oneByThree,
				mouse, delete, solve, exit, deleteAll, undo;
		@SuppressWarnings("rawtypes")
		private ArrayList[] trueFinalStates;
		private int trueMoves;
		private int moveIndex;
		private static TileType lastMoveType;
		private static Tile lastMove;
		private static BufferedImage oneByTwoImg, oneByThreeImg, twoByOneImg, threeByOneImg;
		
		public MousePanel() {
			makeImages();
			used = new Tile[8][8];
			for (int i = 0; i != 7; i++) {
				used[6][i] = new Tile(TileType.DoNothing,6,i);
				used[7][i] = new Tile(TileType.DoNothing,7,i);
				used[i][6] = new Tile(TileType.DoNothing,i,6);
				used[i][7] = new Tile(TileType.DoNothing,i,7);
			}
			arrowStart = -1;
			arrowEnd = -1;
			tiles = new ArrayList<Tile>();
			this.setLayout(new BorderLayout());
			JLabel title = new JLabel("Mouse Solver");
			title.setHorizontalAlignment(JLabel.CENTER);
			title.setFont(new Font("Times New Roman",Font.PLAIN,40));
			this.add(title, BorderLayout.NORTH);
			MyBoard board = new MyBoard();
			this.add(board,BorderLayout.CENTER);
			JPanel rightSide = new JPanel();
			rightSide.setLayout(new GridLayout(6,2));
			moveLimitLabel = new JLabel("Move Limit: ");
			moveLimitLabel.setFont(new Font("Times New Roman",Font.PLAIN,24));
			moveLimitLabel.setHorizontalAlignment(JLabel.RIGHT);
			rightSide.add(moveLimitLabel);
			moveLimitField = new JTextField("100");
			moveLimitField.setHorizontalAlignment(JTextField.CENTER);
			moveLimitField.setFont(new Font("Times New Roman",Font.PLAIN,24));
			rightSide.add(moveLimitField);
			this.add(rightSide,BorderLayout.EAST);
			
			oneByThree = new JButton();
			try {
				oneByThree.setIcon(new ImageIcon(oneByThreeImg));
			} catch (Exception e) {
				oneByThree.setText("1 wide by 3 tall");
			}
			rightSide.add(oneByThree);
			oneByThree.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					currentTileType = TileType.OneByThree;
				}
			});
			
			oneByTwo = new JButton();
			try {
				oneByTwo.setIcon(new ImageIcon(oneByTwoImg));
			} catch (Exception e) {
				oneByTwo.setText("1 wide by 2 tall");
			}
			rightSide.add(oneByTwo);
			oneByTwo.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					currentTileType = TileType.OneByTwo;
				}
			});
			
			threeByOne = new JButton();
			try {
				threeByOne.setIcon(new ImageIcon(threeByOneImg));
			} catch (Exception e) {
				threeByOne.setText("3 wide by 1 tall");
			}
			rightSide.add(threeByOne);
			threeByOne.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					currentTileType = TileType.ThreeByOne;
				}
			});
			
			twoByOne = new JButton();
			try {
				twoByOne.setIcon(new ImageIcon(twoByOneImg));
			} catch (Exception e) {
				twoByOne.setText("2 wide by 1 tall");
			}
			rightSide.add(twoByOne);
			twoByOne.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					currentTileType = TileType.TwoByOne;
				}
			});
			
			mouse = new JButton();
			try {
				Image img = ImageIO.read(new File("Mouse.png"));
				mouse.setIcon(new ImageIcon(img));
			} catch (Exception e) {
				mouse.setText("Mouse");
			}
			rightSide.add(mouse);
			mouse.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					currentTileType = TileType.Mouse;
				}
			});
			
			delete = new JButton();
			delete.setText("Delete");
			rightSide.add(delete);
			delete.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					currentTileType = TileType.Delete;
				}
			});
			
			undo = new JButton();
			try {
				Image img = ImageIO.read(new File("Undo.png"));
				undo.setIcon(new ImageIcon(img));
			} catch (Exception e) {
				undo.setText("Undo");
			}
			rightSide.add(undo);
			undo.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					if (undo.getText().equals("Undo")) {
						if (lastMoveType == TileType.Delete) {
							makeMove(lastMove.type,lastMove.posX,lastMove.posY);
						} else if (lastMoveType == TileType.DeleteAll) {
							tiles = new ArrayList<Tile>();
							for (int myPiece = 0; myPiece != pieces; myPiece++) {
								tiles.add(Tile.copyOf(storedTiles.get(myPiece)));
							}
							
							used = new Tile[8][8];
							
							for (int i = 0; i != 7; i++) {
								used[6][i] = new Tile(TileType.DoNothing,6,i);
								used[7][i] = new Tile(TileType.DoNothing,7,i);
								used[i][6] = new Tile(TileType.DoNothing,i,6);
								used[i][7] = new Tile(TileType.DoNothing,i,7);
							}
							
							for (int m = 0; m != 6; m++) {
								for (int n = 0; n != 6; n++) {
									for (int k = 0; k != pieces; k++) {
										if (storedUsed[m][n] == storedTiles.get(k)) {
											used[m][n] = tiles.get(k);
											break;
										}
									}
								}
							}
						} else if (lastMoveType == TileType.DoNothing){
						} else {
							tiles.remove(lastMove);
							for (int i = 0; i != 6; i++) {
								for (int j = 0; j != 6; j++) {
									if (used[i][j] == lastMove) {
										used[i][j] = null;
									}
								}
							}
						}
						lastMoveType = TileType.DoNothing;
						undo.setText("X");
						repaint();
					} else if (undo.getText().equals("Back one move")){
						backOneMove();
					} else {}
				}
			});
			
			deleteAll = new JButton();
			try {
				Image img = ImageIO.read(new File("DeleteAll.png"));
				deleteAll.setIcon(new ImageIcon(img));
			} catch (Exception e) {
				deleteAll.setText("Delete All");
			}
			rightSide.add(deleteAll);
			deleteAll.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					if (deleteAll.getText().equals("Delete All")) {
						if (tiles.size() == 0) {
							return;
						}
						pieces = tiles.size();
						undo.setText("Undo");
						lastMoveType = TileType.DeleteAll;
						
						storedTiles = new ArrayList<Tile>();
						for (int myPiece = 0; myPiece != pieces; myPiece++) {
							storedTiles.add(Tile.copyOf(tiles.get(myPiece)));
						}
						
						storedUsed = new Tile[8][8];
						
						for (int m = 0; m != 6; m++) {
							for (int n = 0; n != 6; n++) {
								if (used[m][n] == null) {
									storedUsed[m][n] = null;
								} else {
									for (int k = 0; k != pieces; k++) {
										if (used[m][n] == tiles.get(k)) {
											storedUsed[m][n] = storedTiles.get(k);
											break;
										}
									}
								}
							}
						}
						
						tiles = new ArrayList<Tile>();
						for (int i = 0; i != 6; i++) {
							for (int j = 0; j != 6; j++) {
								used[i][j] = null;
							}
						}
						repaint();
					} else {
						reset();
					}
				}
			});
			
			solve = new JButton();
			solve.setText("SOLVE");
			rightSide.add(solve);
			solve.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					if (solve.getText().equals("SOLVE")) {
						if (used[5][2] != null && used[5][2].type == TileType.Mouse) {
							JOptionPane.showMessageDialog(null,"There is already a mouse at the exit.");
							return;
						}
						try {
							moveLimit = Integer.parseInt(moveLimitField.getText());
						} catch (NumberFormatException ex) {
							moveLimit = 100;
							moveLimitField.setText("100");
						}
						solvePuzzle();
					} else {
						showNextMove();
					}
				}
			});
			
			exit = new JButton();
			exit.setText("Exit");
			rightSide.add(exit);
			exit.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					System.exit(0);
				}
			});
		}
		
		public static void makeImages() {
			int blueRGB = Color.BLUE.getRGB();
			int greenRGB = Color.GREEN.getRGB();
			int cyanRGB = Color.CYAN.getRGB();
			int yellowRGB = Color.YELLOW.getRGB();
			
			oneByTwoImg = new BufferedImage(21,36,BufferedImage.TYPE_INT_RGB);
			for (int i = 3; i != 18; i++) {
				for (int j = 3; j != 33; j++) {
					oneByTwoImg.setRGB(i,j,blueRGB);
				}
			}
			
			oneByThreeImg = new BufferedImage(21,51,BufferedImage.TYPE_INT_RGB);
			for (int i = 3; i != 18; i++) {
				for (int j = 3; j != 48; j++) {
					oneByThreeImg.setRGB(i,j,greenRGB);
				}
			}
			
			twoByOneImg = new BufferedImage(36,21,BufferedImage.TYPE_INT_RGB);
			for (int i = 3; i != 33; i++) {
				for (int j = 3; j != 18; j++) {
					twoByOneImg.setRGB(i,j,cyanRGB);
				}
			}
			
			threeByOneImg = new BufferedImage(51,21,BufferedImage.TYPE_INT_RGB);
			for (int i = 3; i != 48; i++) {
				for (int j = 3; j != 18; j++) {
					threeByOneImg.setRGB(i,j,yellowRGB);
				}
			}
		}
		
		public static boolean isValidMove(TileType x, int xBox, int yBox) {
			if (x == TileType.Delete) {
				if (used[xBox][yBox] != null) {
					return true;
				}
				else {
					return false;
				}
			}
			else if (x == TileType.OneByTwo) {
				if (used[xBox][yBox] != null || used[xBox][yBox+1] != null) {
					return false;
				}
				else {
					return true;
				}
			}
			else if (x == TileType.OneByThree) {
				if (used[xBox][yBox] != null || used[xBox][yBox+1] != null || used[xBox][yBox+2] != null) {
					return false;
				}
				else {
					return true;
				}
			}
			else if (x == TileType.TwoByOne || x == TileType.Mouse) {
				if (used[xBox][yBox] != null || used[xBox+1][yBox] != null) {
					return false;
				}
				else {
					return true;
				}
			}
			else if (x == TileType.ThreeByOne) {
				if (used[xBox][yBox] != null || used[xBox+1][yBox] != null || used[xBox+2][yBox] != null) {
					return false;
				}
				else {
					return true;
				}
			}
			else {
				return false;
			}
		}
		
		public static void makeMove(TileType x, int xBox, int yBox) {
			lastMoveType = x;
			undo.setText("Undo");
			if (x == TileType.Delete) {
				Tile z = used[xBox][yBox];
				lastMove = z;
				xBox = z.posX;
				yBox = z.posY;
				lastMove = z;
				tiles.remove(z);
				used[xBox][yBox] = null;
				if (z.type == TileType.Mouse || z.type == TileType.TwoByOne) {
					used[xBox+1][yBox] = null;
				}
				else if (z.type == TileType.ThreeByOne) {
					used[xBox+1][yBox] = null;
					used[xBox+2][yBox] = null;
				}
				else if (z.type == TileType.OneByTwo) {
					used[xBox][yBox+1] = null;
				}
				else if (z.type == TileType.OneByThree) {
					used[xBox][yBox+1] = null;
					used[xBox][yBox+2] = null;
				}
				return;
			}
			
			Tile y = new Tile(x,xBox,yBox);
			lastMove = y;
			tiles.add(y);
			used[xBox][yBox] = y;
			if (x == TileType.Mouse || x == TileType.TwoByOne) {
				used[xBox+1][yBox] = y;
			}
			else if (x == TileType.ThreeByOne) {
				used[xBox+1][yBox] = y;
				used[xBox+2][yBox] = y;
			}
			else if (x == TileType.OneByTwo) {
				used[xBox][yBox+1] = y;
			}
			else if (x == TileType.OneByThree) {
				used[xBox][yBox+1] = y;
				used[xBox][yBox+2] = y;
			}
		}
		
		private static class Tile {
			
			public TileType type;
			public int posX, posY;
			
			public Tile(TileType type, int posX, int posY) {
				this.type = type;
				this.posX = posX;
				this.posY = posY;
			}
			
			public static Tile copyOf(Tile t) {
				if (t == null) {
					return null;
				}
				return new Tile(t.type,t.posX,t.posY);
			}
		}
		
		private class MyBoard extends JPanel {
			
			private int xPadding;
			private int yPadding;
			private int xRight;
			private int yBottom;
			private int squareWidth;
			private int squareHeight;
			
			private MyBoard() {
				xPadding = 25;
				yPadding = 25;
				squareWidth = 75;
				squareHeight = 75;
				xRight = 475;
				yBottom = 475;
				MyMouseListener mouseListener = new MyMouseListener();
				MyMouseMotionListener mouseMotionListener = new MyMouseMotionListener();
				ResizeListener resizeListener = new ResizeListener();
				this.addMouseListener(mouseListener);
				this.addMouseMotionListener(mouseMotionListener);
				this.addComponentListener(resizeListener);
			}
			
			public void paintComponent(Graphics g) {
				super.paintComponent(g);
				g.setColor(Color.BLACK);
				for (int i = xRight; i >= xPadding; i -= squareWidth) {
					g.drawLine(i-1,yPadding,i-1,yBottom);
					g.drawLine(i,yPadding,i,yBottom);
					g.drawLine(i+1,yPadding,i+1,yBottom);
				}
				for (int i = yBottom; i >= yPadding; i -= squareHeight) {
					g.drawLine(xPadding,i-1,xRight,i-1);
					g.drawLine(xPadding,i,xRight,i);
					g.drawLine(xPadding,i+1,xRight,i+1);
				}
				g.drawLine(xRight,2*squareHeight+yPadding-1,xRight+xPadding,2*squareHeight+yPadding-1);
				g.drawLine(xRight,2*squareHeight+yPadding,xRight+xPadding,2*squareHeight+yPadding);
				g.drawLine(xRight,2*squareHeight+yPadding+1,xRight+xPadding,2*squareHeight+yPadding+1);
				g.drawLine(xRight,3*squareHeight+yPadding-1,xRight+xPadding,3*squareHeight+yPadding-1);
				g.drawLine(xRight,3*squareHeight+yPadding,xRight+xPadding,3*squareHeight+yPadding);
				g.drawLine(xRight,3*squareHeight+yPadding+1,xRight+xPadding,3*squareHeight+yPadding+1);
				for (int i = tiles.size()-1; i != -1; i--) {
					Tile x = tiles.get(i);
					int w = squareWidth-3, h = squareHeight-3;
					TileType useTileType = x.type;
					if (useTileType == TileType.TwoByOne) {
						g.setColor(Color.CYAN);
						w += squareWidth;
					}
					else if (useTileType == TileType.Mouse) {
						g.setColor(Color.GRAY);
						w += squareWidth;
					}
					else if (useTileType == TileType.ThreeByOne) {
						g.setColor(Color.YELLOW);
						w += 2*squareWidth;
					}
					else if (useTileType == TileType.OneByTwo) {
						g.setColor(Color.BLUE);
						h += squareHeight;
					}
					else if (useTileType == TileType.OneByThree) {
						g.setColor(Color.GREEN);
						h += 2*squareHeight;
					}
					else {
						g.setColor(Color.BLUE);
					}
					if (useTileType != TileType.DoNothing) {
						g.fillRect(xPadding+2+squareWidth*x.posX,yPadding+2+squareHeight*x.posY,w,h);
					}
				}
				if (mouseX < xRight && mouseX >= xPadding && mouseY < yBottom && mouseY >= yPadding) {
					int xBox = (mouseX-xPadding)/squareWidth;
					int yBox = (mouseY-yPadding)/squareHeight;
					if (isValidMove(currentTileType,xBox,yBox)) {
						g.setColor(Color.RED);
						int w = squareWidth-4, h = squareHeight-4;
						TileType useTileType = currentTileType;
						if (useTileType == TileType.Delete) {
							Tile x = used[xBox][yBox];
							useTileType = x.type;
							xBox = x.posX;
							yBox = x.posY;
						}
						if (useTileType == TileType.Mouse || useTileType == TileType.TwoByOne) {
							w += squareWidth;
						}
						else if (useTileType == TileType.ThreeByOne) {
							w += 2*squareWidth;
						}
						else if (useTileType == TileType.OneByTwo) {
							h += squareHeight;
						}
						else if (useTileType == TileType.OneByThree) {
							h += 2*squareHeight;
						}
						g.drawRect(xPadding+2+squareWidth*xBox,yPadding+2+squareHeight*yBox,w,h);
						g.drawRect(xPadding+3+squareWidth*xBox,yPadding+3+squareHeight*yBox,w-2,h-2);
						g.drawRect(xPadding+4+squareWidth*xBox,yPadding+4+squareHeight*yBox,w-4,h-4);
					}
				}
				if (arrowStart != -1) {
					int startX = xPadding*5/2+squareWidth*((arrowStart+arrowShift)%6);
					int startY = yPadding*5/2+squareHeight*((arrowStart+arrowShift)/6);
					int endX = xPadding*5/2+squareWidth*((arrowEnd+arrowShift)%6);
					int endY = yPadding*5/2+squareHeight*((arrowEnd+arrowShift)/6);
					g.setColor(Color.RED);
					if (startX == endX) {
						g.drawLine(startX-2,startY,endX-2,endY);
						g.drawLine(startX-1,startY,endX-1,endY);
						g.drawLine(startX,startY,endX,endY);
						g.drawLine(startX+1,startY,endX+1,endY);
						g.drawLine(startX+2,startY,endX+2,endY);
						
						if (startY < endY) {
							g.drawLine(endX,endY+2,endX-20,endY-18);
							g.drawLine(endX,endY+1,endX-20,endY-19);
							g.drawLine(endX,endY,endX-20,endY-20);
							g.drawLine(endX,endY-1,endX-20,endY-21);
							g.drawLine(endX,endY-2,endX-20,endY-22);
							g.drawLine(endX,endY+2,endX+20,endY-18);
							g.drawLine(endX,endY+1,endX+20,endY-19);
							g.drawLine(endX,endY,endX+20,endY-20);
							g.drawLine(endX,endY-1,endX+20,endY-21);
							g.drawLine(endX,endY-2,endX+20,endY-22);
						}
						else {
							g.drawLine(endX,endY-2,endX-20,endY+18);
							g.drawLine(endX,endY-1,endX-20,endY+19);
							g.drawLine(endX,endY,endX-20,endY+20);
							g.drawLine(endX,endY+1,endX-20,endY+21);
							g.drawLine(endX,endY+2,endX-20,endY+22);
							g.drawLine(endX,endY-2,endX+20,endY+18);
							g.drawLine(endX,endY-1,endX+20,endY+19);
							g.drawLine(endX,endY,endX+20,endY+20);
							g.drawLine(endX,endY+1,endX+20,endY+21);
							g.drawLine(endX,endY+2,endX+20,endY+22);
						}
					}
					else {
						g.drawLine(startX,startY-2,endX,endY-2);
						g.drawLine(startX,startY-1,endX,endY-1);
						g.drawLine(startX,startY,endX,endY);
						g.drawLine(startX,startY+1,endX,endY+1);
						g.drawLine(startX,startY+2,endX,endY+2);
						
						if (startX < endX) {
							g.drawLine(endX+2,endY,endX-18,endY-20);
							g.drawLine(endX+1,endY,endX-19,endY-20);
							g.drawLine(endX,endY,endX-20,endY-20);
							g.drawLine(endX-1,endY,endX-21,endY-20);
							g.drawLine(endX-2,endY,endX-22,endY-20);
							g.drawLine(endX+2,endY,endX-18,endY+20);
							g.drawLine(endX+1,endY,endX-19,endY+20);
							g.drawLine(endX,endY,endX-20,endY+20);
							g.drawLine(endX-1,endY,endX-21,endY+20);
							g.drawLine(endX-2,endY,endX-22,endY+20);
						}
						else {
							g.drawLine(endX-2,endY,endX+18,endY-20);
							g.drawLine(endX-1,endY,endX+19,endY-20);
							g.drawLine(endX,endY,endX+20,endY-20);
							g.drawLine(endX+1,endY,endX+21,endY-20);
							g.drawLine(endX+2,endY,endX+22,endY-20);
							g.drawLine(endX-2,endY,endX+18,endY+20);
							g.drawLine(endX-1,endY,endX+19,endY+20);
							g.drawLine(endX,endY,endX+20,endY+20);
							g.drawLine(endX+1,endY,endX+21,endY+20);
							g.drawLine(endX+2,endY,endX+22,endY+20);
						}
					}
				}
			}
			
			private void setWidth() {
				int width = getWidth();
				xPadding = width/20;
				squareWidth = 3*xPadding;
				xRight = 19*xPadding;
			}
			
			private void setHeight() {
				int height = getHeight();
				yPadding = height/20;
				squareHeight = 3*yPadding;
				yBottom = 19*yPadding;
			}
			
			private class MyMouseMotionListener implements MouseMotionListener {
				
				public void mouseDragged(MouseEvent e) {
					mouseX = e.getX();
					mouseY = e.getY();
					repaint();
				}

				public void mouseMoved(MouseEvent e) {
					mouseX = e.getX();
					mouseY = e.getY();
					repaint();
				}
			}
			
			private class MyMouseListener implements MouseListener {

				public void mouseClicked(MouseEvent e) {}

				public void mouseEntered(MouseEvent e) {}

				public void mouseExited(MouseEvent e) {
					mouseX = 0;
					mouseY = 0;
				}

				public void mousePressed(MouseEvent e) {}

				public void mouseReleased(MouseEvent e) {
					if (mouseX >= 19*xPadding || mouseX < xPadding ||
							mouseY >= 19*yPadding || mouseY < yPadding) {
						return;
					}
					int xBox = (mouseX-xPadding)/squareWidth;
					int yBox = (mouseY-yPadding)/squareHeight;
					if (isValidMove(currentTileType,xBox,yBox)) {
						makeMove(currentTileType,xBox,yBox);
					}
					repaint();
				}
			}
			
			private class ResizeListener extends ComponentAdapter {
				
				public void componentResized(ComponentEvent e) {
					setWidth();
					setHeight();
					repaint();
				}
			}
		}
		
		@SuppressWarnings("static-access")
		public void solvePuzzle() {
			pieces = tiles.size();
			int[] tilesX = new int[pieces];
			int[] tilesY = new int[pieces];
			boolean[] leftRight = new boolean[pieces];
			boolean[] upDown = new boolean[pieces];
			boolean[] isMouse = new boolean[pieces];
			for (int i = 0; i != pieces; i++) {
				TileType type = tiles.get(i).type;
				if (type == TileType.Mouse) {
					tilesX[i] = 2;
					tilesY[i] = 1;
					leftRight[i] = true;
					upDown[i] = false;
					isMouse[i] = true;
				}
				else if (type == TileType.TwoByOne) {
					tilesX[i] = 2;
					tilesY[i] = 1;
					leftRight[i] = true;
					upDown[i] = false;
				}
				else if (type == TileType.ThreeByOne) {
					tilesX[i] = 3;
					tilesY[i] = 1;
					leftRight[i] = true;
					upDown[i] = false;
				}
				else if (type == TileType.OneByTwo) {
					tilesX[i] = 1;
					tilesY[i] = 2;
					leftRight[i] = false;
					upDown[i] = true;
				}
				else if (type == TileType.OneByThree) {
					tilesX[i] = 1;
					tilesY[i] = 3;
					leftRight[i] = false;
					upDown[i] = true;
				}
				else {
					JOptionPane.showMessageDialog(null,"Uh oh! Your board somehow had a tile of type "
							+ type.toString() + ".\nPlease inform the developer. Cannot continue solving.");
					reset();
					return;
				}
			}
			
			ArrayList<Integer> startState = new ArrayList<Integer>();
			for (int i = 0; i != pieces; i++) {
				Tile x = tiles.get(i);
				startState.add(x.posX + 6 * x.posY);
			}
			
			HashMap<ArrayList<Integer>,ArrayList<Integer>> gameStates =
					new HashMap<ArrayList<Integer>,ArrayList<Integer>>();
			gameStates.put(startState,null);
			ArrayList<ArrayList<Integer>> a = new ArrayList<ArrayList<Integer>>();
			a.add(startState);
			ArrayList<ArrayList<Integer>> b = new ArrayList<ArrayList<Integer>>();
			ArrayList<ArrayList<Integer>> previous;
			ArrayList<ArrayList<Integer>> next;
			
			int states = 1;
			
			moveLoop:
			for (int moves = 1; moves <= moveLimit; moves++) {
				if (moves%2 == 0) {
					previous = b;
					a = new ArrayList<ArrayList<Integer>>();
					next = a;
				}
				else {
					previous = a;
					b = new ArrayList<ArrayList<Integer>>();
					next = b;
				}
				for (int i = previous.size()-1; i != -1; i--) {
					boolean[][] used = new boolean[6][6];
					ArrayList<Integer> gameState = previous.get(i);
					int[] xPos = new int[pieces];
					int[] yPos = new int[pieces];
					for (int piece = 0; piece != pieces; piece++) {
						int bigIndex = gameState.get(piece);
						xPos[piece] = bigIndex%6;
						yPos[piece] = bigIndex/6;
						for (int x = 0; x != tilesX[piece]; x++) {
							for (int y = 0; y != tilesY[piece]; y++) {
								used[x+xPos[piece]][y+yPos[piece]] = true;
							}
						}
					}
					
					for (int piece = 0; piece != pieces; piece++) {
						if (leftRight[piece]) {
							int index = gameState.get(piece);
							while (index%6 != 0 && !used[(index%6)-1][index/6]) {
								index--;
								ArrayList<Integer> copy = new ArrayList<Integer>(gameState);
								copy.set(piece,index);
								if (gameStates.containsKey(copy)) {
									continue;
								}
								else {
									gameStates.put(copy,gameState);
									next.add(copy);
								}
							}
							index = gameState.get(piece);
							while ((index + tilesX[piece])%6 != 0 && !used[(index%6)+tilesX[piece]][index/6]) {
								index++;
								ArrayList<Integer> copy = new ArrayList<Integer>(gameState);
								copy.set(piece,index);
								if (gameStates.containsKey(copy)) {
									continue;
								}
								else {
									gameStates.put(copy,gameState);
									if (isMouse[piece] && index == 16) {
										@SuppressWarnings("unchecked")
										ArrayList<Integer>[] finalStates = new ArrayList[moves+1];
										for (int finalIndex = moves; finalIndex != -1; finalIndex--) {
											finalStates[finalIndex] = copy;
											copy = gameStates.get(copy);
										}
										trueFinalStates = finalStates;
										trueMoves = moves;
										moveIndex = 0;
										
										storedTiles = new ArrayList<Tile>();
										for (int myPiece = 0; myPiece != pieces; myPiece++) {
											storedTiles.add(Tile.copyOf(tiles.get(myPiece)));
										}
										
										storedUsed = new Tile[8][8];
										
										for (int m = 0; m != 6; m++) {
											for (int n = 0; n != 6; n++) {
												if (this.used[m][n] == null) {
													storedUsed[m][n] = null;
													continue;
												}
												for (int k = 0; k != pieces; k++) {
													if (this.used[m][n] == tiles.get(k)) {
														storedUsed[m][n] = storedTiles.get(k);
														break;
													}
												}
											}
										}
										
										limitButtons();
										showNextMove();
										break moveLoop;
									}
									next.add(copy);
								}
							}
						}
						
						if (upDown[piece]) {
							int index = gameState.get(piece);
							while (index/6 != 0 && !used[(index%6)][(index/6)-1]) {
								index-=6;
								ArrayList<Integer> copy = new ArrayList<Integer>(gameState);
								copy.set(piece,index);
								if (gameStates.containsKey(copy)) {
									continue;
								}
								else {
									gameStates.put(copy,gameState);
									next.add(copy);
								}
							}
							index = gameState.get(piece);
							while ((index/6) + tilesY[piece] != 6 && !used[(index%6)][(index/6)+tilesY[piece]]) {
								index+=6;
								ArrayList<Integer> copy = new ArrayList<Integer>(gameState);
								copy.set(piece,index);
								if (gameStates.containsKey(copy)) {
									continue;
								}
								else {
									gameStates.put(copy,gameState);
									next.add(copy);
								}
							}
						}
					}
				}
				if (next.size() == 0) {
					JOptionPane.showMessageDialog(panel,"After " + (moves-1) + " moves, all " + states
							+ " possible game states have been reached, and there is no solution.");
					break;
				}
				else {
					states += next.size();
					if (moves == moveLimit) {
						JOptionPane.showMessageDialog(panel,"No solution in " + moves + " moves! :("
								+ "\nthough try entering a higher move limit and hit \"SOLVE\" again.");
					}
				}
			}
		}
		
		public void backOneMove() {
			
			reset();
			if (moveIndex == 0 || moveIndex == 1) {
				return;
			}
			int m = moveIndex-1;
			moveIndex = 0;
			limitButtons();
			while (m != 0) {
				m--;
				showNextMove();
			}
		}
		
		@SuppressWarnings("unchecked")
		public void showNextMove() {
			int finalIndex = moveIndex;
			if (finalIndex-1 == trueMoves) {
				reset();
				return;
			}
			if (finalIndex != 0) {
				int xBefore = arrowStart%6;
				int yBefore = arrowStart/6;
				Tile t = null;
				for (int i = 0; i != pieces; i++) {
					Tile x = tiles.get(i);
					if (x.posX == xBefore && x.posY == yBefore) {
						t = x;
						break;
					}
				}
				t.posX = arrowEnd%6;
				t.posY = arrowEnd/6;
			}
			repaint();
			if (finalIndex != trueMoves) {
				solve.setText("move " + (finalIndex+1));
				repaint();
				ArrayList<Integer> before = trueFinalStates[finalIndex];
				ArrayList<Integer> after = trueFinalStates[finalIndex+1];
				for (int x = 0; x != pieces; x++) {
					if (before.get(x) != after.get(x)) {
						arrowStart = before.get(x);
						arrowEnd = after.get(x);
						TileType z = tiles.get(x).type;
						arrowShift = 0;
						if (z == TileType.OneByThree && arrowEnd > arrowStart) {
							arrowShift = 12;
						} else if (z == TileType.OneByTwo && arrowEnd > arrowStart) {
							arrowShift = 6;
						} else if (z == TileType.ThreeByOne && arrowEnd > arrowStart) {
							arrowShift = 2;
						} else if (z == TileType.TwoByOne && arrowEnd > arrowStart) {
							arrowShift = 1;
						} else if (z == TileType.Mouse && arrowEnd > arrowStart) {
							arrowShift = 1;
						}
						repaint();
						break;
					}
				}
			}
			else {
				arrowStart = -1;
				arrowEnd = -1;
				solve.setText("Reset");
				repaint();
			}
			moveIndex++;
		}
		
		public void limitButtons() {
			currentTileType = TileType.DoNothing;
			oneByThree.setEnabled(false);
			oneByTwo.setEnabled(false);
			threeByOne.setEnabled(false);
			twoByOne.setEnabled(false);
			mouse.setEnabled(false);
			delete.setEnabled(false);
			moveLimitLabel.setEnabled(false);
			moveLimitField.setEnabled(false);
			undo.setText("Back one move");
			deleteAll.setText("Back to board");
		}
		
		public void reset() {
			arrowStart = -1;
			arrowEnd = -1;
			
			tiles = new ArrayList<Tile>();
			for (int myPiece = 0; myPiece != pieces; myPiece++) {
				tiles.add(Tile.copyOf(storedTiles.get(myPiece)));
			}
			
			used = new Tile[8][8];
			
			for (int i = 0; i != 7; i++) {
				used[6][i] = new Tile(TileType.DoNothing,6,i);
				used[7][i] = new Tile(TileType.DoNothing,7,i);
				used[i][6] = new Tile(TileType.DoNothing,i,6);
				used[i][7] = new Tile(TileType.DoNothing,i,7);
			}
			
			for (int m = 0; m != 6; m++) {
				for (int n = 0; n != 6; n++) {
					for (int k = 0; k != pieces; k++) {
						if (storedUsed[m][n] == storedTiles.get(k)) {
							used[m][n] = tiles.get(k);
							break;
						}
					}
				}
			}
			
			solve.setText("SOLVE");
			oneByThree.setEnabled(true);
			oneByTwo.setEnabled(true);
			threeByOne.setEnabled(true);
			twoByOne.setEnabled(true);
			mouse.setEnabled(true);
			delete.setEnabled(true);
			moveLimitLabel.setEnabled(true);
			moveLimitField.setEnabled(true);
			undo.setText("X");
			deleteAll.setText("Delete All");
			repaint();
		}
	}
}