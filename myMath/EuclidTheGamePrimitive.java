package myMath;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.Scanner;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;

public class EuclidTheGamePrimitive {
	
	// for 8, one must drag c to the left of a (though not farther than ab from a).
	// Circles bc and cb intersect at d. circle da intersects ab at e, and ce is the answer
	
	// Works for: 1,2,3,4,5,6,8,9,13,14,21
	
	static final double moe = 0.0000000001; // Margin of Error
	static int problem, maxMove, baseMoves, baseGameMoves;
	static boolean pointed, exit;
	static ArrayList<String> finalMoves;
	static double[] stored;
	static boolean display = false;
	static int count;
	static boolean special = true;
	
	private static class Point {
		double x, y;
		int move;
		public Point(double x, double y, int move) {
			this.x = x;
			this.y = y;
			this.move = move;
		}
	}
	
	private static class Line {
		double slope, intercept;
		boolean vertical; double xValue;
		int move;
		public Line(Point a, Point b, int move) {
			this.move = move;
			if (close(a.x,b.x)) {
				vertical = true;
				xValue = b.x;
			} else if (close(a.y,b.y)) {
				vertical = false;
				slope = 0;
				intercept = b.y;
			} else {
				vertical = false;
				slope = ((b.y-a.y)/(b.x-a.x));
				intercept = b.y - slope*b.x;
			}
		}
		
		public Line(Point a, double slope, int move) {
			this.move = move;
			vertical = false;
			this.slope = slope;
			intercept = a.y - slope*a.x;
		}
		
		public Line(double xValue, int move) {
			vertical = true;
			this.xValue = xValue;
		}
	}
	
	private static class Circle {
		double x, y, radius;
		int move;
		public Circle(Point center, Point outer, int move) {
			x = center.x;
			y = center.y;
			radius = dist(center,outer);
			this.move = move;
		}
		
		public Circle(double x, double y, double radius, int move) {
			this.x = x;
			this.y = y;
			this.radius = radius;
			this.move = move;
		}
	}
	
	public static boolean close(double m, double n) {
		if (m > n) {
			return (m - n < moe);
		} else {
			return (n - m < moe);
		}
	}
	
	public static double dist(Point a, Point b) {
		double x = a.x - b.x;
		double y = a.y - b.y;
		return Math.sqrt(x*x + y*y);
	}

	public static boolean equals(Point a, Point b) {
		return (close(a.x,b.x) && close(a.y,b.y));
	}
	
	public static boolean equals(Line a, Line b) {
		if (a.vertical) {
			return (b.vertical && close(a.xValue,b.xValue));
		} else {
			return (!b.vertical && close(a.slope,b.slope) && close(a.intercept,b.intercept));
		}
	}
	
	public static boolean equals(Circle a, Circle b) {
		return (close(a.x,b.x) && close(a.y,b.y) && close(a.radius,b.radius));
	}
	
	public static boolean parallel(Line a, Line b) {
		if (a.vertical) {
			return b.vertical;
		} else {
			return (!b.vertical && close(a.slope,b.slope));
		}
	}
	
	public static boolean perpendicular(Line a, Line b) {
		if (a.vertical) {
			return (!b.vertical && b.slope == 0);
		} else if (a.slope == 0) {
			return b.vertical;
		} else {
			return close(a.slope*b.slope,-1.0);
		}
	}
	
	public static Point intersect(Line a, Line b, int move) {
		if (a.vertical) {
			return new Point(a.xValue,b.slope*a.xValue + b.intercept,move);
		} else if (b.vertical) {
			return new Point(b.xValue,a.slope*b.xValue + a.intercept,move);
		} else {
			// y = m1*x+b1
			// y = m2*x+b2
			// x = (b1-b2)/(m2-m1)
			// y = m1*x+b1 = (m1b1-m1b2+b1m2-b1m1)/(m2-m1) = (b1m2-m1b2)/(m2-m1)
			double x = (a.intercept-b.intercept)/(b.slope-a.slope);
			double y = (a.intercept*b.slope-b.intercept*a.slope)/(b.slope-a.slope);
			return new Point(x,y,move);
		}
	}
	
	public static ArrayList<Point> intersect(Line a, Circle c, int move) {
		ArrayList<Point> points = new ArrayList<Point>();
		if (a.vertical) {
			if (c.x + c.radius + moe < a.xValue || c.x - c.radius - moe > a.xValue) {
				// they don't intersect
			} else if (c.x + c.radius - moe < a.xValue) {
				// one intersection
				points.add(new Point(a.xValue,c.y,move));
			} else if (c.x - c.radius + moe > a.xValue) {
				// one intersection
				points.add(new Point(a.xValue,c.y,move));
			} else {
				// two intersections
				// (x-c.x)^2 + (y-c.y)^2 = c.radius^2
				// y = c.y +- rt(c.radius^2 - (a.xValue-c.x)^2)
				double rt = Math.sqrt(c.radius*c.radius - (a.xValue-c.x)*(a.xValue-c.x));
				points.add(new Point(a.xValue,c.y+rt,move));
				points.add(new Point(a.xValue,c.y-rt,move));
			}
		} else {
			// point d,e to line y = mx + b --> mx - y + b = 0 --> (md-e+b)/rt(m^2 + 1)
			double dist = Math.abs(a.slope*c.x - c.y + a.intercept)/Math.sqrt(a.slope*a.slope+1);
			if (dist > c.radius + moe) {
				// they don't intersect
			} else if (dist > c.radius - moe) {
				// one intersection
				if (a.slope == 0) {
					Line l = new Line(c.x,move);
					points.add(intersect(a,l,move));
				} else {
					Line l = new Line(new Point(c.x,c.y,move),-1/a.slope,move);
					points.add(intersect(a,l,move));
				}
			} else {
				double m = a.slope;
				double b = a.intercept;
				double r = c.radius;
				double quadA = m*m+1;
				double quadB = 2*(m*(b-c.y)-c.x);
				double quadC = (b-c.y)*(b-c.y)+c.x*c.x-r*r;
				double rt = Math.sqrt(quadB*quadB-4*quadA*quadC);
				double x = (-quadB+rt)/(2*quadA);
				double y = m*x + b;
				points.add(new Point(x,y,move));
				x = (-quadB-rt)/(2*quadA);
				y = m*x + b;
				points.add(new Point(x,y,move));
			}
		}
		return points;
	}
	
	public static ArrayList<Point> intersect(Circle m, Circle n, int move) {
		ArrayList<Point> points = new ArrayList<Point>();
		double a = m.x, b = m.y, c = n.x, d = n.y, r1 = m.radius, r2 = n.radius;
		double dist = dist(new Point(a,b,move),new Point(c,d,move));
		if (r1 + r2 + moe < dist) {
			// do not intersect (external)
		} else if (dist + r2 + moe < r1 || dist + r1 + moe < r2) {
			// do not intersect (one contained within other)
		} else if (r1 + r2 - moe < dist) {
			double x = (r2*a+r1*c)/(r1+r2);
			double y = (r2*b+r1*d)/(r1+r2);
			points.add(new Point(x,y,move));
		} else if (dist + r2 - moe < r1) {
			double x = (r1*c-r2*a)/(r1-r2);
			double y = (r1*d-r2*b)/(r1-r2);
			points.add(new Point(x,y,move));
		} else if (dist + r1 - moe < r2) {
			double x = (r1*c-r2*a)/(r1-r2);
			double y = (r1*d-r2*b)/(r1-r2);
			points.add(new Point(x,y,move));
		} else if (!close(b,d)) {
			double temp = a; a = b; b = temp;
			temp = c; c = d; d = temp;
			double s = (d-b)/(a-c), v = (r2*r2-r1*r1+a*a+b*b-c*c-d*d)/(2*(a-c));
			Line l = new Line(new Point(0,v,move),s,move);
			points = intersect(l,m,move);
		} else if (!close(a,c)) {
			double s = (d-b)/(a-c), v = (r2*r2-r1*r1+a*a+b*b-c*c-d*d)/(2*(a-c));
			if (!close(0,s)) {
				Line l = new Line(new Point(v,0,move),1/s,move);
				points = intersect(l,m,move);
			} else {
				Line l = new Line(v,0);
				points = intersect(l,m,move);
			}
		}
		return points;
	}
	
	public static void main(String[] args) {
		while (problem < 1 || problem > 25) {
			String t = JOptionPane.showInputDialog(null,"Which problem number?");
			try {
				problem = Integer.parseInt(t);
			} catch (NumberFormatException e) {}
		}
		pointed = false;
		exit = false;
		go();
	}
	
	public static void go() {
		if (problem == 1) {
			solve1();
		} else if (problem == 2) {
			solve2();
		} else if (problem == 3) {
			solve3();
		} else if (problem == 4) {
			solve4();
		} else if (problem == 5) {
			solve5();
		} else if (problem == 6) {
			solve6();
		} else if (problem == 7) {
			solve7();
		} else if (problem == 8) {
			solve8();
		} else if (problem == 9) {
			solve9();
		} else if (problem == 10) {
			solve10();
		} else if (problem == 11) {
			solve11();
		} else if (problem == 12) {
			solve12();
		} else if (problem == 13) {
			solve13();
		} else if (problem == 14) {
			solve14();
		} else if (problem == 15) {
			solve15();
		} else if (problem == 16) {
			solve16();
		} else if (problem == 17) {
			solve17();
		} else if (problem == 18) {
			solve18();
		} else if (problem == 19) {
			solve19();
		} else if (problem == 20) {
			solve20();
		} else if (problem == 21) {
			solve21();
		} else if (problem == 22) {
			solve22();
		} else if (problem == 23) {
			solve23();
		} else if (problem == 24) {
			solve24();
		} else if (problem == 25) {
			solve25();
		} else {
			System.out.println("Not a valid problem. Uh oh.");
		}
	}
	
	public static void solve1() {
		ArrayList<Point> points = new ArrayList<Point>();
		ArrayList<Line> lines = new ArrayList<Line>();
		ArrayList<Circle> circles = new ArrayList<Circle>();
		Point a = new Point(0,0,-1);
		Point b = new Point(1,0,-1);
		points.add(a);
		points.add(b);
		Line l = new Line(a,b,-1);
		lines.add(l);
		baseMoves = 3;
		baseGameMoves = 1;
		ArrayList<Integer> minCIndex = new ArrayList<Integer>();
		ArrayList<Integer> minLIndex = new ArrayList<Integer>();
		minCIndex.add(1);
		minCIndex.add(0);
		minLIndex.add(1);
		minLIndex.add(0);
		tryMoves(points,lines,circles,0,4,"",minCIndex,minLIndex);
	}
	
	public static void solve2() {
		ArrayList<Point> points = new ArrayList<Point>();
		ArrayList<Line> lines = new ArrayList<Line>();
		ArrayList<Circle> circles = new ArrayList<Circle>();
		Point a = new Point(0,0,-1);
		Point b = new Point(1,0,-1);
		points.add(a);
		points.add(b);
		Line l = new Line(a,b,-1);
		lines.add(l);
		baseMoves = 3;
		baseGameMoves = 1;
		ArrayList<Integer> minCIndex = new ArrayList<Integer>();
		ArrayList<Integer> minLIndex = new ArrayList<Integer>();
		minCIndex.add(1);
		minCIndex.add(0);
		minLIndex.add(1);
		minLIndex.add(0);
		tryMoves(points,lines,circles,0,3,"",minCIndex,minLIndex);
	}
	
	public static void solve3() {
		ArrayList<Point> points = new ArrayList<Point>();
		ArrayList<Line> lines = new ArrayList<Line>();
		ArrayList<Circle> circles = new ArrayList<Circle>();
		Point a = new Point(0,0,-1);
		Point b = new Point(1,0,-1);
		points.add(a);
		Line l = new Line(a,b,-1);
		lines.add(l);
		double angle = Math.random()*Math.PI/2;
		stored = new double[1];
		stored[0] = angle;
		Line m = new Line(a,Math.tan(angle),-1);
		lines.add(m);
		baseMoves = 3;
		baseGameMoves = 2;
		ArrayList<Integer> minCIndex = new ArrayList<Integer>();
		ArrayList<Integer> minLIndex = new ArrayList<Integer>();
		minCIndex.add(1);
		minCIndex.add(0);
		minLIndex.add(1);
		minLIndex.add(0);
		tryMoves(points,lines,circles,0,4,"",minCIndex,minLIndex);
	}
	
	public static void solve4() {
		ArrayList<Point> points = new ArrayList<Point>();
		ArrayList<Line> lines = new ArrayList<Line>();
		ArrayList<Circle> circles = new ArrayList<Circle>();
		Point a = new Point(0,0,-1);
		points.add(a);
		Line l = new Line(a,0,-1);
		lines.add(l);
		baseMoves = 2;
		baseGameMoves = 1;
		ArrayList<Integer> minCIndex = new ArrayList<Integer>();
		ArrayList<Integer> minLIndex = new ArrayList<Integer>();
		minCIndex.add(1);
		minLIndex.add(1);
		tryMoves(points,lines,circles,0,3,"",minCIndex,minLIndex);
	}
	
	public static void solve5() {
		ArrayList<Point> points = new ArrayList<Point>();
		ArrayList<Line> lines = new ArrayList<Line>();
		ArrayList<Circle> circles = new ArrayList<Circle>();
		Point a = new Point(0,0,-1);
		Point b = new Point(1,1,-1);
		points.add(a);
		points.add(b);
		Line l = new Line(a,0,-1);
		lines.add(l);
		baseMoves = 3;
		baseGameMoves = 1;
		ArrayList<Integer> minCIndex = new ArrayList<Integer>();
		ArrayList<Integer> minLIndex = new ArrayList<Integer>();
		minCIndex.add(1);
		minCIndex.add(0);
		minLIndex.add(1);
		minLIndex.add(0);
		tryMoves(points,lines,circles,0,3,"",minCIndex,minLIndex);
	}
	
	public static void solve6() {
		ArrayList<Point> points = new ArrayList<Point>();
		ArrayList<Line> lines = new ArrayList<Line>();
		ArrayList<Circle> circles = new ArrayList<Circle>();
		Point a = new Point(0,0,-1);
		Point b = new Point(1,1,-1);
		points.add(a);
		points.add(b);
		Line l = new Line(a,0,-1);
		lines.add(l);
		baseMoves = 3;
		baseGameMoves = 1;
		ArrayList<Integer> minCIndex = new ArrayList<Integer>();
		ArrayList<Integer> minLIndex = new ArrayList<Integer>();
		minCIndex.add(1);
		minCIndex.add(0);
		minLIndex.add(1);
		minLIndex.add(0);
		tryMoves(points,lines,circles,0,4,"",minCIndex,minLIndex);
	}
	
	public static void solve7() {
		JOptionPane.showMessageDialog(null,"Sorry! Too many moves :(");
		System.exit(0);
		ArrayList<Point> points = new ArrayList<Point>();
		ArrayList<Line> lines = new ArrayList<Line>();
		ArrayList<Circle> circles = new ArrayList<Circle>();
		Point a = new Point(0,0,-1);
		Point b = new Point(1,0,-1);
		double x = Math.random();
		double y = Math.random();
		stored = new double[2];
		stored[0] = x;
		stored[1] = y;
		Point c = new Point(x,y,-1);
		points.add(a);
		points.add(b);
		points.add(c);
		Line l = new Line(a,b,-1);
		lines.add(l);
		baseMoves = 4;
		baseGameMoves = 1;
		ArrayList<Integer> minCIndex = new ArrayList<Integer>();
		ArrayList<Integer> minLIndex = new ArrayList<Integer>();
		minCIndex.add(1);
		minCIndex.add(0);
		minCIndex.add(0);
		minLIndex.add(1);
		minLIndex.add(0);
		minLIndex.add(0);
		tryMoves(points,lines,circles,0,6,"",minCIndex,minLIndex);
	}
	
	public static void solve8() {
		ArrayList<Point> points = new ArrayList<Point>();
		ArrayList<Line> lines = new ArrayList<Line>();
		ArrayList<Circle> circles = new ArrayList<Circle>();
		Point a = new Point(0,0,-1);
		Point b = new Point(1,0,-1);
		double x = Math.random();
		stored = new double[1];
		stored[0] = x;
		Point c = new Point(x,0,-1);
		points.add(a);
		points.add(b);
		points.add(c);
		Line l = new Line(a,b,-1);
		lines.add(l);
		baseMoves = 4;
		baseGameMoves = 1;
		ArrayList<Integer> minCIndex = new ArrayList<Integer>();
		ArrayList<Integer> minLIndex = new ArrayList<Integer>();
		minCIndex.add(1);
		minCIndex.add(0);
		minCIndex.add(0);
		minLIndex.add(1);
		minLIndex.add(0);
		minLIndex.add(0);
		tryMoves(points,lines,circles,0,3,"",minCIndex,minLIndex);
	}
	
	public static void solve9() {
		ArrayList<Point> points = new ArrayList<Point>();
		ArrayList<Line> lines = new ArrayList<Line>();
		ArrayList<Circle> circles = new ArrayList<Circle>();
		Point a = new Point(0,0,-1);
		Point b = new Point(1,0,-1);
		double x = Math.random();
		double y = Math.random();
		stored = new double[2];
		stored[0] = x;
		stored[1] = y;
		Point c = new Point(x,y,-1);
		points.add(a);
		points.add(b);
		points.add(c);
		Line l = new Line(a,b,-1);
		lines.add(l);
		baseMoves = 4;
		baseGameMoves = 1;
		ArrayList<Integer> minCIndex = new ArrayList<Integer>();
		ArrayList<Integer> minLIndex = new ArrayList<Integer>();
		minCIndex.add(1);
		minCIndex.add(0);
		minCIndex.add(0);
		minLIndex.add(1);
		minLIndex.add(0);
		minLIndex.add(0);
		tryMoves(points,lines,circles,0,5,"",minCIndex,minLIndex);
	}
	
	public static void solve10() {
		ArrayList<Point> points = new ArrayList<Point>();
		ArrayList<Line> lines = new ArrayList<Line>();
		ArrayList<Circle> circles = new ArrayList<Circle>();
		Point a = new Point(0,1,-1);
		Point b = new Point(1,1,-1);
		double x1 = 0.07;
		double y1 = 0.20;
		double x2 = 0.31;
		double y2 = 0.02;
		Point c = new Point(x1,y1,-1);
		Point d = new Point(x2,y2,-1);
		points.add(a);
		points.add(b);
		points.add(c);
		points.add(d);
		Line l = new Line(a,b,-1);
		Line m = new Line(c,d,-1);
		lines.add(l);
		lines.add(m);
		baseMoves = 6;
		baseGameMoves = 2;
		ArrayList<Integer> minCIndex = new ArrayList<Integer>();
		ArrayList<Integer> minLIndex = new ArrayList<Integer>();
		minCIndex.add(1);
		minCIndex.add(0);
		minCIndex.add(0);
		minCIndex.add(0);
		minLIndex.add(1);
		minLIndex.add(0);
		minLIndex.add(0);
		minLIndex.add(0);
		tryMoves(points,lines,circles,0,5,"",minCIndex,minLIndex);
	}
	
	public static void solve11() {
		
	}
	
	public static void solve12() {
		JOptionPane.showMessageDialog(null,"Sorry! Too many moves :(");
		System.exit(0);
		ArrayList<Point> points = new ArrayList<Point>();
		ArrayList<Line> lines = new ArrayList<Line>();
		ArrayList<Circle> circles = new ArrayList<Circle>();
		double theta1 = Math.random()*Math.PI/2;
		double theta2 = Math.random()*Math.PI/2;
		double r1 = 3*Math.random();
		double r2 = 3*Math.random();
		double x = Math.random();
		double y = Math.random();
		
		Point a = new Point(x,y,-1);
		Point b = new Point(x+r1*Math.cos(theta1+theta2),y+r1*Math.sin(theta1+theta2),-1);
		Point c = new Point(x+r2*Math.cos(theta2),y+r2*Math.sin(theta2),-1);
		Point d = new Point(0,0,-1);
		
		Line l = new Line(a,b,-1);
		Line m = new Line(a,c,-1);
		Line n = new Line(d,0,-1);
		
		stored = new double[1];
		stored[0] = theta1;
		
		points.add(a);
		points.add(b);
		points.add(c);
		points.add(d);
		
		lines.add(l);
		lines.add(m);
		lines.add(n);
		
		baseMoves = 7;
		baseGameMoves = 3;
		ArrayList<Integer> minCIndex = new ArrayList<Integer>();
		ArrayList<Integer> minLIndex = new ArrayList<Integer>();
		minCIndex.add(1);
		minCIndex.add(0);
		minCIndex.add(0);
		minCIndex.add(0);
		minLIndex.add(1);
		minLIndex.add(0);
		minLIndex.add(0);
		minLIndex.add(0);
		tryMoves(points,lines,circles,0,5,"",minCIndex,minLIndex);
	}
	
	public static void solve13() {
		ArrayList<Point> points = new ArrayList<Point>();
		ArrayList<Line> lines = new ArrayList<Line>();
		ArrayList<Circle> circles = new ArrayList<Circle>();
		Circle c = new Circle(new Point(0,0,-1),new Point(0,1,-1),-1);
		circles.add(c);
		baseMoves = 1;
		baseGameMoves = 1;
		ArrayList<Integer> minCIndex = new ArrayList<Integer>();
		ArrayList<Integer> minLIndex = new ArrayList<Integer>();
		tryMoves(points,lines,circles,0,5,"",minCIndex,minLIndex);
	}
	
	public static void solve14() {
		ArrayList<Point> points = new ArrayList<Point>();
		ArrayList<Line> lines = new ArrayList<Line>();
		ArrayList<Circle> circles = new ArrayList<Circle>();
		Point a = new Point(0,0,-1);
		Point b = new Point(1,0,-1);
		points.add(a);
		points.add(b);
		Circle c = new Circle(a,b,-1);
		circles.add(c);
		baseMoves = 3;
		baseGameMoves = 1;
		ArrayList<Integer> minCIndex = new ArrayList<Integer>();
		ArrayList<Integer> minLIndex = new ArrayList<Integer>();
		minCIndex.add(1);
		minCIndex.add(0);
		minLIndex.add(1);
		minLIndex.add(0);
		tryMoves(points,lines,circles,0,3,"",minCIndex,minLIndex);
	}
	
	public static void solve15() {
		
	}
	
	public static void solve16() {
		
	}
	
	public static void solve17() {
		
	}
	
	public static void solve18() {
		
	}
	
	public static void solve19() {
		
	}
	
	public static void solve20() {
		
	}
	
	public static void solve21() {
		ArrayList<Point> points = new ArrayList<Point>();
		ArrayList<Line> lines = new ArrayList<Line>();
		ArrayList<Circle> circles = new ArrayList<Circle>();
		Point a = new Point(2.1,0,-1);
		Point b = new Point(0,1,-1);
		points.add(a);
		points.add(b);
		Circle c = new Circle(b,new Point(0,0,-1),-1);
		circles.add(c);
		baseMoves = 3;
		baseGameMoves = 1;
		ArrayList<Integer> minCIndex = new ArrayList<Integer>();
		ArrayList<Integer> minLIndex = new ArrayList<Integer>();
		minCIndex.add(1);
		minCIndex.add(0);
		minLIndex.add(1);
		minLIndex.add(0);
		tryMoves(points,lines,circles,0,6,"",minCIndex,minLIndex);
	}
	
	public static void solve22() {
		
	}
	
	public static void solve23() {
		
	}
	
	public static void solve24() {
		
	}
	
	public static void solve25() {
		
	}
	
	public static void tryMoves(ArrayList<Point> points, ArrayList<Line> lines,
			ArrayList<Circle> circles, int movesDone, int moveMax, String moves,
			ArrayList<Integer> minCIndex, ArrayList<Integer> minLIndex) {
	//	display = true;
		count++;
		if (display && count%10000 == 0) { // useful for debugging
			System.out.println();
			for (int m = movesDone; m != 0; m--) {
				System.out.print("\t");
			}
			System.out.println(moves);
			for (int m = movesDone; m != 0; m--) {
				System.out.print("\t");
			}
			System.out.print("Points: ");
			for (int i = 0; i != points.size(); i++) {
				System.out.print(points.get(i).x + "," + points.get(i).y + "  ");
			}
			System.out.print("   Lines: ");
			for (int i = 0; i != lines.size(); i++) {
				Line l = lines.get(i);
				System.out.print(l.vertical?("x = " + l.xValue + "\t"):"slope " + l.slope +
						" and intercept " + l.intercept + "  ");
			}
			System.out.print("   Circles: ");
			for (int i = 0; i != circles.size(); i++) {
				System.out.print(circles.get(i).x + "," + circles.get(i).y + "," + circles.get(i).radius + "  ");
			}
			System.out.println();
			Scanner s = new Scanner(System.in);
			s.nextLine();
		}
		if (movesDone == moveMax) {
			if (goal(points,lines,circles)) {
				exit = true;
				displaySolved(points,lines,circles);
			}
			return;
		} else if (movesDone == moveMax-1) {
			if (!tempGoal(points,lines,circles)) {
				return;
			}
		}
		int move = movesDone + 1;
		int pointsSize = points.size();
		int linesSize = lines.size();
		int circlesSize = circles.size();
		
		for (int i = 0; i != pointsSize; i++) {
			Point x = points.get(i);
			innerLoop:
			for (int j = minCIndex.get(i); j != pointsSize; j++) {
				if (j == i) {
					continue;
				}
				Point y = points.get(j);
				Circle c = new Circle(x,y,move);
				for (int k = 0; k != circlesSize; k++) {
					if (equals(c,circles.get(k))) {
						continue innerLoop;
					}
				}
				
				ArrayList<Integer> newMinCIndex = new ArrayList<Integer>(minCIndex);
				ArrayList<Integer> newMinLIndex = new ArrayList<Integer>(minLIndex);
				for (int z = 0; z <= i; z++) {
					newMinCIndex.set(z,Math.max(j,newMinCIndex.get(z)));
				}
				
				for (int k = 0; k != linesSize; k++) {
					ArrayList<Point> intersections = intersect(lines.get(k),c,move);
					thisLoop:
					for (int index = intersections.size()-1; index != -1; index--) {
						Point intersection = intersections.get(index);
						for (int m = 0; m != pointsSize; m++) {
							if (equals(intersection,points.get(m))) {
								continue thisLoop;
							}
						}
						points.add(intersection);
						newMinCIndex.add(0);
						newMinLIndex.add(0);
					}
				}
				for (int k = 0; k != circlesSize; k++) {
					ArrayList<Point> intersections = intersect(c,circles.get(k),move);
					thatLoop:
					for (int index = intersections.size()-1; index != -1; index--) {
						Point intersection = intersections.get(index);
						for (int m = 0; m != pointsSize; m++) {
							if (equals(intersection,points.get(m))) {
								continue thatLoop;
							}
						}
						points.add(intersection);
						newMinCIndex.add(0);
						newMinLIndex.add(0);
					}
				}
				circles.add(c);
				tryMoves(points,lines,circles,move,moveMax,moves +
						"Circle with center " + c.x + " " + c.y + " and radius " + c.radius + "\t",
						newMinCIndex,newMinLIndex);
				if (exit) {
					return;
				}
				for (int p = points.size()-1; p >= pointsSize; p--) {
					points.remove(p);
				}
				for (int p = lines.size()-1; p >= linesSize; p--) {
					lines.remove(p);
				}
				for (int p = circles.size()-1; p >= circlesSize; p--) {
					circles.remove(p);
				}
			}
		}
		
		for (int i = 0; i < pointsSize-1; i++) {
			Point x = points.get(i);
			innerLoop:
			for (int j = minLIndex.get(i); j != pointsSize; j++) {
				if (i == j) {
					continue;
				}
				Point y = points.get(j);
				Line l = new Line(x,y,move);
				for (int k = 0; k != linesSize; k++) {
					if (equals(l,lines.get(k))) {
						continue innerLoop;
					}
				}
				
				ArrayList<Integer> newMinCIndex = new ArrayList<Integer>(minCIndex);
				ArrayList<Integer> newMinLIndex = new ArrayList<Integer>(minLIndex);
				for (int z = 0; z <= i; z++) {
					newMinLIndex.set(z,Math.max(j,newMinLIndex.get(z)));
				}
				
				thisLoop:
				for (int k = 0; k != linesSize; k++) {
					Point intersection = intersect(lines.get(k),l,move);
					for (int m = 0; m != points.size(); m++) {
						if (equals(intersection,points.get(m))) {
							continue thisLoop;
						}
					}
					points.add(intersection);
					newMinCIndex.add(0);
					newMinLIndex.add(0);
				}
				for (int k = 0; k != circlesSize; k++) {
					ArrayList<Point> intersections = intersect(l,circles.get(k),move);
					thatLoop:
					for (int index = intersections.size()-1; index != -1; index--) {
						Point intersection = intersections.get(index);
						for (int m = 0; m != pointsSize; m++) {
							if (equals(intersection,points.get(m))) {
								continue thatLoop;
							}
						}
						points.add(intersection);
						newMinCIndex.add(0);
						newMinLIndex.add(0);
					}
				}
				lines.add(l);
				tryMoves(points,lines,circles,move,moveMax, moves + 
						"Line with slope " + l.slope + " and intercept " + l.intercept + "\t",
						newMinCIndex,newMinLIndex);
				if (exit) {
					return;
				}
				for (int p = points.size()-1; p >= pointsSize; p--) {
					points.remove(p);
				}
				for (int p = lines.size()-1; p >= linesSize; p--) {
					lines.remove(p);
				}
				for (int p = circles.size()-1; p >= circlesSize; p--) {
					circles.remove(p);
				}
			}
		}
		
		if (move == 1 && !pointed) {
			System.out.println("here");
			pointed = true;
			for (int i = 0; i != linesSize; i++) {
				Line l = lines.get(i);
				if (l.vertical) {
					for (int j = 0; j != 6; j++) {
						ArrayList<Integer> newMinCIndex = new ArrayList<Integer>(minCIndex);
						ArrayList<Integer> newMinLIndex = new ArrayList<Integer>(minLIndex);
						Point p = new Point(l.xValue,Math.random()+(i/2==0?0:1),-1);
						points.add(p);
						newMinCIndex.add(0);
						newMinLIndex.add(0);
						tryMoves(points,lines,circles,move-1,moveMax,moves +
								"Point at " + p.x + " " + p.y + " on a line\t",
								newMinCIndex,newMinLIndex);
						if (exit) {
							return;
						}
						for (int q = points.size()-1; q >= pointsSize; q--) {
							points.remove(p);
						}
						for (int q = lines.size()-1; q >= linesSize; q--) {
							lines.remove(p);
						}
						for (int q = circles.size()-1; q >= circlesSize; q--) {
							circles.remove(p);
						}
					}
				} else {
					for (int j = 0; j != 6; j++) {
						double x = Math.random()+(i/2==0?0:1);
						double y = x*l.slope+l.intercept;
						while (y < 0 || y > 2) {
							x = Math.random()+(i/2==0?0:1);
							y = x*l.slope+l.intercept;
						}
						ArrayList<Integer> newMinCIndex = new ArrayList<Integer>(minCIndex);
						ArrayList<Integer> newMinLIndex = new ArrayList<Integer>(minLIndex);
						Point p = new Point(x,y,-1);
						points.add(p);
						newMinCIndex.add(0);
						newMinLIndex.add(0);
						tryMoves(points,lines,circles,move-1,moveMax,moves +
								"Point at " + p.x + " " + p.y + " on a line\t",
								newMinCIndex,newMinLIndex);
						if (exit) {
							return;
						}
						for (int q = points.size()-1; q >= pointsSize; q--) {
							points.remove(p);
						}
						for (int q = lines.size()-1; q >= linesSize; q--) {
							lines.remove(p);
						}
						for (int q = circles.size()-1; q >= circlesSize; q--) {
							circles.remove(p);
						}
					}
				}
			}
			for (int i = 0; i != circlesSize; i++) {
				Circle l = circles.get(i);
				for (int j = 0; j != 6; j++) {
					double rad = Math.random()*Math.PI;
					double x = l.x + l.radius*Math.cos(rad);
					double y = l.y + l.radius*Math.sin(rad);
					ArrayList<Integer> newMinCIndex = new ArrayList<Integer>(minCIndex);
					ArrayList<Integer> newMinLIndex = new ArrayList<Integer>(minLIndex);
					Point p = new Point(x,y,-1);
					points.add(p);
					newMinCIndex.add(0);
					newMinLIndex.add(0);
					tryMoves(points,lines,circles,move-1,moveMax,moves +
							"Point at " + p.x + " " + p.y + " on a circle\t",
							newMinCIndex,newMinLIndex);
					if (exit) {
						return;
					}
					for (int q = points.size()-1; q >= pointsSize; q--) {
						points.remove(p);
					}
					for (int q = lines.size()-1; q >= linesSize; q--) {
						lines.remove(p);
					}
					for (int q = circles.size()-1; q >= circlesSize; q--) {
						circles.remove(p);
					}
				}
			}
			for (int i = 0; i != 6; i++) {
				ArrayList<Integer> newMinCIndex = new ArrayList<Integer>(minCIndex);
				ArrayList<Integer> newMinLIndex = new ArrayList<Integer>(minLIndex);
				Point p = new Point(Math.random()+(i%2==0?0:1),Math.random()+(i/2==0?0:1),-1);
				points.add(p);
				newMinCIndex.add(0);
				newMinLIndex.add(0);
				tryMoves(points,lines,circles,move-1,moveMax,moves +
						"Point at " + p.x + " " + p.y + "\t",
						newMinCIndex,newMinLIndex);
				if (exit) {
					return;
				}
				for (int q = points.size()-1; q >= pointsSize; q--) {
					points.remove(p);
				}
				for (int q = lines.size()-1; q >= linesSize; q--) {
					lines.remove(p);
				}
				for (int q = circles.size()-1; q >= circlesSize; q--) {
					circles.remove(p);
				}
			}
		} else if (problem == 13 && special) {
			special = false;
			System.out.println("really here");
			Circle l = circles.get(0);
			for (int j = 0; j != 6; j++) {
				double rad = Math.random()*Math.PI;
				double x = l.x + l.radius*Math.cos(rad);
				double y = l.y + l.radius*Math.sin(rad);
				ArrayList<Integer> newMinCIndex = new ArrayList<Integer>(minCIndex);
				ArrayList<Integer> newMinLIndex = new ArrayList<Integer>(minLIndex);
				Point p = new Point(x,y,-1);
				points.add(p);
				newMinCIndex.add(0);
				newMinLIndex.add(0);
				tryMoves(points,lines,circles,move-1,moveMax,moves +
						"Point at " + p.x + " " + p.y + " on a circle\t",
						newMinCIndex,newMinLIndex);
				if (exit) {
					return;
				}
				for (int q = points.size()-1; q >= pointsSize; q--) {
					points.remove(p);
				}
				for (int q = lines.size()-1; q >= linesSize; q--) {
					lines.remove(p);
				}
				for (int q = circles.size()-1; q >= circlesSize; q--) {
					circles.remove(p);
				}
			}
		}
	}
	
	public static boolean goal(ArrayList<Point> points, ArrayList<Line> lines,
			ArrayList<Circle> circles) {
		if (problem == 1) {
			if (lines.size() < 3) {
				return false;
			} else {
				Point a = new Point(0,0,0);
				Point b = new Point(1,0,0);
				Point c = new Point(0.5,Math.sqrt(3)/2,0);
				Line goal1 = new Line(a,b,0);
				Line goal2 = new Line(a,c,0);
				Line goal3 = new Line(b,c,0);
				boolean line1 = false, line2 = false, line3 = false;
				for (int i = lines.size()-1; i != -1; i--) {
					Line l = lines.get(i);
					if (!line1 && equals(l,goal1)) {
						line1 = true;
					} else if (!line2 && equals(l,goal2)) {
						line2 = true;
					} else if (!line3 &&equals(l,goal3)) {
						line3 = true;
					}
				}
				return (line1 && line2 && line3);
			}
			
		} else if (problem == 2) {
			Point x = new Point(0.5,0,0);
			for (int i = points.size()-1; i != -1; i--) {
				if (equals(x,points.get(i))) {
					return true;
				}
			}
			return false;
		} else if (problem == 3) {
			Line goal = new Line(new Point(0,0,0),Math.tan(stored[0]/2),0);
			for (int i = lines.size()-1; i != -1; i--) {
				if (equals(goal,lines.get(i))) {
					return true;
				}
			}
			return false;
		} else if (problem == 4) {
			Line goal = new Line(new Point(0,0,0),new Point(0,1,0),0);
			for (int i = lines.size()-1; i != -1; i--) {
				if (equals(goal,lines.get(i))) {
					return true;
				}
			}
			return false;
		} else if (problem == 5) {
			Line goal = new Line(new Point(1,0,0),new Point(1,1,0),0);
			for (int i = lines.size()-1; i != -1; i--) {
				if (equals(goal,lines.get(i))) {
					return true;
				}
			}
			return false;
		} else if (problem == 6) {
			Line goal = new Line(new Point(1,1,0),new Point(2,1,0),0);
			for (int i = lines.size()-1; i != -1; i--) {
				if (equals(goal,lines.get(i))) {
					return true;
				}
			}
			return false;
		} else if (problem == 7) {
			Line goal1 = new Line(new Point(1,1,0),new Point(2,1,0),0);
			Point goal2 = new Point(stored[0]+1,stored[1],0);
			for (int i = lines.size()-1; i != -1; i--) {
				if (equals(goal1,lines.get(i))) {
					for (int j = points.size()-1; j != -1; j--) {
						if (equals(goal2,points.get(j))) {
							return true;
						}
					}
				}
			}
			return false;
		} else if (problem == 8) {
			Point goal2 = new Point(stored[0]+1,0,0);
			for (int j = points.size()-1; j != -1; j--) {
				if (equals(goal2,points.get(j))) {
					return true;
				}
			}
			return false;
		} else if (problem == 9) {
			Circle goal = new Circle(stored[0],stored[1],1,0);
			for (int i = circles.size()-1; i != -1; i--) {
				if (equals(goal,circles.get(i))) {
					return true;
				}
			}
			return false;
		} else if (problem == 10) {
			Point goal = new Point(0.3,1,0);
			for (int j = points.size()-1; j != -1; j--) {
				if (equals(goal,points.get(j))) {
					return true;
				}
			}
		} else if (problem == 11) {
			
		} else if (problem == 12) {
			Line goal = new Line(new Point(0,0,0),Math.tan(stored[0]),0);
			for (int i = lines.size()-1; i != -1; i--) {
				if (equals(goal,lines.get(i))) {
					return true;
				}
			}
			return false;
		} else if (problem == 13) {
			Point goal = new Point(0,0,0);
			for (int i = points.size()-1; i != -1; i--) {
				if (equals(goal,points.get(i))) {
					return true;
				}
			}
		} else if (problem == 14) {
			Line goal = new Line(new Point(1,0,0),new Point(1,1,0),0);
			for (int i = lines.size()-1; i != -1; i--) {
				if (equals(lines.get(i),goal)) {
					return true;
				}
			}
			return false;
		} else if (problem == 15) {
			
		} else if (problem == 16) {
			
		} else if (problem == 17) {
			
		} else if (problem == 18) {
			
		} else if (problem == 19) {
			
		} else if (problem == 20) {
			
		} else if (problem == 21) {
			Circle c = new Circle(new Point(0,1,0),new Point(0,0,0),0);
			boolean go = false;
			for (int i = lines.size()-1; i != -1; i--) {
				if (intersect(lines.get(i),c,0).size() == 1) {
					if (go) {
						return true;
					} else {
						go = true;
					}
				}
			}
			return false;
		} else if (problem == 22) {
			
		} else if (problem == 23) {
			
		} else if (problem == 24) {
			
		} else if (problem == 25) {
			
		} else {
			System.out.println("Not a valid problem. Uh oh.");
		}
		return false;
	}
	
	public static boolean tempGoal(ArrayList<Point> points, ArrayList<Line> lines,
			ArrayList<Circle> circles) {
		if (problem == 7) {
			Line goal1 = new Line(new Point(1,1,0),new Point(2,1,0),0);
			Point goal2 = new Point(stored[0]+1,stored[1],0);
			for (int i = lines.size()-1; i != -1; i--) {
				if (equals(goal1,lines.get(i))) {
					return true;
				}
			}
			for (int j = points.size()-1; j != -1; j--) {
				if (equals(goal2,points.get(j))) {
					return true;
				}
			}
			return false;
		} else if (problem == 21) {
			Line p = new Line(new Point(0,0,0),0,0);
			for (int i = lines.size()-1; i != -1; i--) {
				if (equals(p,lines.get(i))) {
					return true;
				}
			}
			return false;
		}
		return true;
	}
	
	public static void displaySolved(ArrayList<Point> points, ArrayList<Line> lines, ArrayList<Circle> circles) {
		ArrayList<Object> list = new ArrayList<Object>();
		finalMoves = new ArrayList<String>();
		int pointIndex = 0, lineIndex = 0, circleIndex = 0;
		int pointSize = points.size(), linesSize = lines.size(), circlesSize = circles.size();
		int moveIndex = -2;
		boolean go = true;
		while (go) {
			moveIndex++;
			go = false;
			while (circleIndex < circlesSize) {
				go = true;
				Circle c;
				if ((c = circles.get(circleIndex)).move == moveIndex) {
					list.add(c);
					finalMoves.add("Circle at " + c.x + " " + c.y + " with radius " + c.radius);
					circleIndex++;
				} else {
					break;
				}
			}
			while (lineIndex < linesSize) {
				go = true;
				Line l;
				if ((l = lines.get(lineIndex)).move == moveIndex) {
					list.add(l);
					finalMoves.add("Line with " + (l.vertical?" x = " + l.xValue:" slope " + l.slope + " and intercept " + l.intercept));
					lineIndex++;
				} else {
					break;
				}
			}
			while (pointIndex < pointSize) {
				go = true;
				Point p;
				if ((p = points.get(pointIndex)).move == moveIndex) {
					list.add(p);
					finalMoves.add("Point at " + p.x + " " + p.y);
					pointIndex++;
				} else {
					break;
				}
			}
		}
		maxMove = list.size();
		
		JFrame frame = new JFrame();
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setSize(800,700);
		MyPanel panel = new MyPanel(list);
		frame.add(panel);
		frame.setVisible(true);
	}
	
	@SuppressWarnings("serial")
	private static class MyPanel extends JPanel {
		static int currentMove, gameMove;
		static ArrayList<Object> list;
		static JTextField thisMove;
		static int range = 5;
		public MyPanel(ArrayList<Object> list) {
			MyPanel.list = list;
			this.setLayout(new BorderLayout());
			currentMove = baseMoves-1;
			gameMove = baseGameMoves-2;
			JButton back = new JButton("back");
			back.addActionListener(new ActionListener() {
				@Override
				public void actionPerformed(ActionEvent e) {
					if (currentMove >= baseMoves) {
						currentMove--;
						repaint();
					}
				}
			});
			JButton next = new JButton("next");
			next.addActionListener(new ActionListener() {
				@Override
				public void actionPerformed(ActionEvent e) {
					if (currentMove != maxMove-1) {
						currentMove++;
						repaint();
					}
				}
			});
			this.add(back,BorderLayout.WEST);
			this.add(next,BorderLayout.EAST);
			Graph graph = new Graph();
			this.add(graph,BorderLayout.CENTER);
			thisMove = new JTextField();
			thisMove.setEditable(false);
			this.add(thisMove,BorderLayout.SOUTH);
			JButton zoom = new JButton("zoom out");
			zoom.addActionListener(new ActionListener() {
				@Override
				public void actionPerformed(ActionEvent e) {
					range *= 2;
					repaint();
				}
			});
			this.add(zoom,BorderLayout.NORTH);
		}
		
		public void paintComponent(Graphics g) {
			if (currentMove >= 0) {
				thisMove.setText(finalMoves.get(currentMove));
			} else {
				thisMove.setText("");
			}
		}
		
		private static class Graph extends JPanel {
			public Graph() {
				repaint();
			}
			
			public void paintComponent(Graphics g) {
				super.paintComponent(g);
				// -5,5			5,5
				//
				//
				//
				//
				// -5,-5		5,-5
				
				g.setColor(Color.BLACK);
				gameMove = -1;
				for (int i = 0; i <= currentMove; i++) {
					if (list.get(i).getClass() == Point.class) {
						Point p = (Point) list.get(i);
						int x = 325+(int)(300*p.x/range);
						int y = 325-(int)(300*p.y/range);
						g.fillRect(x-2,y-2,5,5);
					} else if (list.get(i).getClass() == Line.class) {
						gameMove++;
						Line l = (Line) list.get(i);
						if (!l.vertical) {
							int x1 = 325+(int)(-300);
							int y1 = 325-(int)(300*(-range*l.slope+l.intercept)/range);
							int x2 = 325+(int)(300);
							int y2 = 325-(int)(300*(range*l.slope+l.intercept)/range);
							g.drawLine(x1,y1,x2,y2);
						} else {
							int x = 325+(int)(300/range*l.xValue);
							g.drawLine(x,25,x,625);
						}
					} else if (list.get(i).getClass() == Circle.class) {
						gameMove++;
						Circle c = (Circle) list.get(i);
						int x = 325+(int)(300*(c.x-c.radius)/range);
						int y = 325-(int)(300*(c.y+c.radius)/range);
						int size = (int)(600*c.radius/range);
						g.drawOval(x,y,size,size);
					}
				}
				g.drawString("Move " + (gameMove-baseGameMoves+1),550,50);
			}
		}
	}
}