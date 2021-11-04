package myMath;

public class BigInt { //30 digits each
	private int[] digits;
	
	public BigInt(int[] array) {
		digits = new int[30];
		for (int i = 0; i<array.length; i++) {
			digits[i] = array[i];
		}
		for (int i = array.length; i<30; i++) {
			digits[i] = 0;
		}
	}
	
	public String toString() {
		String thing = "";
		for (int i = 29; i>-1; i--) {
			thing += this.getDigit(i);
		}
		return thing;
	}
	
	public int getDigit(int digit) {
		return this.digits[digit];
	}
	
	public boolean equals(BigInt y) {
		for (int i = 0; i<30; i++) {
			if (this.getDigit(i)!=y.getDigit(i)) {
				return false;
			}
		}
		return true;
	}
	
	public BigInt multiply(BigInt y) {
		int extra = 0, tempMult = 0;
		
		int[] array = new int[30];
		for (int i = 0; i<30; i++) {
			tempMult = 0;
			for (int j = 0; j<=i; j++) {
				tempMult += this.getDigit(j)*y.getDigit(i-j);
			}
			tempMult += extra;
			extra = tempMult/10;
			array[i] = tempMult%10;
		}
		return new BigInt(array);
	}
}