package myMath;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.math.BigInteger;

public class NickPotter {

	public static void main(String[] args) throws FileNotFoundException {
		File file = new File("NickPotterNumbers.txt");
		PrintWriter writer = new PrintWriter(file);
		for (int i = 2; i <= 10000; i++) {
			BigInteger j = SmallestNumberWithXFactors.smallestNumberWithXFactors(i);
			writer.println(i + "\t" + j);
			System.out.println(i + "\t" + j);
		}
		writer.close();
	}
}