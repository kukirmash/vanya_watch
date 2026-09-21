import javax.microedition.lcdui.Display;
import javax.microedition.midlet.MIDlet;

public class LockGameMIDlet extends MIDlet {
	private Display display;
	private LockGameCanvas gameCanvas;

	public void startApp() {
		if (gameCanvas == null) {
			gameCanvas = new LockGameCanvas();
		}

		display = Display.getDisplay(this);
		display.setCurrent(gameCanvas);
	}

	public void pauseApp() {
	}

	public void destroyApp(boolean unconditional) {
	}
}