package myMath;

import java.io.*;
import javax.sound.sampled.*;

public class AudioPlayer {
	public static void play(String fileName) {
		try {
		    AudioInputStream stream;
		    AudioFormat format;
		    DataLine.Info info;
		    Clip clip;

		    stream = AudioSystem.getAudioInputStream(new File(fileName));
		    format = stream.getFormat();
		    info = new DataLine.Info(Clip.class, format);
		    clip = (Clip) AudioSystem.getLine(info);
		    clip.open(stream);
		    clip.start();
		}
		catch (Exception e) {
			System.err.println(e.getLocalizedMessage());
			System.out.println("Well you fucked up");
			System.exit(0);
		}
	}
}
