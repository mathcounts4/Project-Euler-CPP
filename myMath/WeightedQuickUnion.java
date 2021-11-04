package myMath;

public class WeightedQuickUnion {
	
	//http://www.sfs.uni-tuebingen.de/~dg/l1.html#QFAlgo
	
	int elements;
	int[] id;
	int[] sz;
	
	public static void main(String[] args) {
		// This tests this class
		WeightedQuickUnion hi = new WeightedQuickUnion(4);
		hi.nextConnection(0,1);
		hi.nextConnection(3,2);
		hi.nextConnection(1,3);
	}
	
	public WeightedQuickUnion(int elements) {
		id = new int[elements];
		sz = new int[elements];
		for (int i = 0; i < elements ; i++) {
			id[i] = i; sz[i] = 1;
		}
		this.elements = elements;

		System.out.print("P q  ");
		printRow(id, elements);
	    System.out.println();
	}
	public boolean nextConnection(int p, int q) {
		//returns if elements had different roots and connects them
    	System.out.print("" + p + " " + q + "  ");
    	while (p != id[p]) {
    		id[p] = id[id[p]];
    		p = id[p];
    	}
    	while (q != id[q]) {
    		id[q] = id[id[q]];
    		q = id[q];
    	}
    	if (p == q) {
    		printRow(id, elements);
    		return false;
    	}
    	if (sz[p] < sz[q]) {
    		id[p] = q; sz[q] += sz[p];
    	}
    	else {
    		id[q] = p; sz[p] += sz[q];
    	}
    	printRow(id, elements);
    	return true;
	}
	
	private void printRow(int[] id, int elements) {
		for (int i = 0; i < elements; i++) {
			System.out.print(" " + id[i]);
		}
		System.out.println();
	}
}