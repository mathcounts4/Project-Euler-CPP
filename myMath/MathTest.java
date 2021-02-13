package myMath;

import java.util.*;

public class MathTest {

	public static void main(String[] args) {
		Scanner scanner = new Scanner(System.in);
		int b = 0, c = 0, d = 0;
		
		System.out.println("Enter \"GCD\" or \"LCM\"");
		String a = scanner.nextLine();
		
		for (int iTry=0; iTry==0;) {
			try {
				 if (a.equalsIgnoreCase("GCD")) {
					System.out.println("How many numbers?");
					b = Integer.parseInt(scanner.next());
					if (b<2) {
						System.out.println("Fuck you. I'm done with your shit.");
						System.exit(0);
					}
					System.out.println("Gimme dem numbahs.");
					c = Math.abs(Integer.parseInt(scanner.next()));
					d = 0;
					while (b-->1) {
						d = Math.abs(Integer.parseInt(scanner.next()));
						c = MyMath.gcd(c,d);
					}
				}
				else if (a.equalsIgnoreCase("LCM")) {
					System.out.println("How many numbers?");
					b = Integer.parseInt(scanner.next());
					if (b<2) {
						System.out.println("Fuck you. I'm done with your shit.");
						System.exit(0);
					}
					System.out.println("Gimme dem numbahs");
					c = Math.abs(Integer.parseInt(scanner.next()));
					d = 0;
					while (b-->1) {
						d = Math.abs(Integer.parseInt(scanner.next()));
						c = MyMath.lcm(c,d);
					}
				}
				else {
					System.out.println("Well why the fuck did you run this program, Bitch??");
					System.exit(0);
				}
				System.out.printf("Your %s is %d.\n", a, c);
				iTry++;
			}
			catch (NumberFormatException fuck1) {
				System.out.println("Bitch can't do shit " + fuck1.getMessage());
				System.out.println("Try again.");
			}
			catch (InputMismatchException fuck2) {
				System.out.println("Bitch can't do shit " + fuck2.getMessage());
				System.out.println("Try again.");
			}
		}
	}
}