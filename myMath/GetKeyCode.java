package myMath;

import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;

import javax.swing.JFrame;
import javax.swing.JTextField;

public class GetKeyCode {
	public static void main(String[] argv) throws Exception {
		JTextField component = new JTextField();
		component.addKeyListener(new MyKeyListener());

		JFrame f = new JFrame();
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		f.add(component);
		f.setSize(300, 300);
		f.setVisible(true);
	}
}

class MyKeyListener extends KeyAdapter {
	public void keyPressed(KeyEvent evt) {

		System.out.println(evt.getKeyCode());
	}
}