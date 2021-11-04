package myMath;

import javax.swing.JOptionPane;

// this was the last problem on the CSC171 final in Spring 2014, worth 20 bonus points

public class StringPermutations {
	
	public static void main(String[] args) {
		String s = JOptionPane.showInputDialog(null,"Enter a String to permute.");
		int x = s.length();
		boolean[] used = new boolean[x];
		for (int i = 0; i!=x; i++) {
			used[i] = false;
		}
		permuteString(s,"",used);
	}
	
	public static void permuteString(String s, String prefix, boolean[] used) {
		if (s.length()==prefix.length()) {
			System.out.println(prefix);
		}
		else {
			for (int i = 0; i<s.length(); i++) {
				if (!used[i]) {
					used[i] = true;
					permuteString(s,prefix+s.charAt(i),used);
					used[i] = false;
				}
			}
		}
	}

}
