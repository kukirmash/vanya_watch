import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;

public class LockGameCanvas extends Canvas implements Runnable {
	private static final int GAME_OVER_COLOR = 0xE9685C;
	private static final int LOCK_COLOR = 0x1F0431;
	private static final int LOCK_HANDLE_COLOR = 0x314162;
	private static final int GOAL_COLOR = 0xEFCC3C;
	private static final int POINTER_COLOR = 0xE82C5E;
	private static final int SCORE_COLOR = 0xAEE7EE;

	private int screenWidth;
	private int screenHeight;
	private int backgroundColor;

	private int goalSize;
	private int ringSize;
	private int pointerWidth;
	private int pointerHeight;
	private int centerX;
	private int centerY;
	private int lockImageX;
	private int lockImageY;
	private int lockOpenHeight;

	private Image lockImage;
	private Image lockHandleImage;

	public LockGameCanvas() {
		new LockGame();

		setFullScreenMode(true);
		screenWidth = getWidth();
		screenHeight = getHeight();

		backgroundColor = 0x5CD3D9;
		initGraphics();
		loadGame();

		new Thread(this).start();
	}

	public void initGraphics() {
		int baseSize = Math.min(screenWidth / 3, screenHeight / 4);
		lockImage = Image.createImage(baseSize * 3, baseSize * 4);
		lockHandleImage = Image.createImage(baseSize * 3, baseSize * 4);

		int width = lockImage.getWidth();
		int height = lockImage.getHeight();
		int baseWidth = 240;
		int baseHeight = 320;
		Graphics g = lockHandleImage.getGraphics();

		goalSize = width * 17 / baseWidth;
		ringSize = width * 144 / baseWidth;
		pointerWidth = width * 18 / baseWidth;
		pointerHeight = width * 8 / baseWidth;
		lockImageX = (screenWidth - width) / 2;
		lockImageY = (screenHeight - height) / 2;
		centerX = lockImageX + width * 33 / baseWidth + width * 87 / baseWidth;
		centerY = lockImageY + height * 132 / baseHeight + height * 87 / baseHeight;
		lockOpenHeight = height * 48 / baseHeight;

		g.setColor(backgroundColor);
		g.fillRect(0, 0, width, height);

		g.setColor(LOCK_HANDLE_COLOR);
		g.fillArc(width * 72 / baseWidth, height * 57 / baseHeight, width * 96 / baseWidth, height * 70 / baseHeight, 0,
				360);
		g.fillArc(width * 72 / baseWidth, height * 140 / baseHeight, width * 26 / baseWidth, height * 14 / baseHeight,
				0, 360);
		g.fillArc(width * 142 / baseWidth, height * 140 / baseHeight, width * 26 / baseWidth, height * 14 / baseHeight,
				0, 360);
		g.fillRect(width * 72 / baseWidth, height * 92 / baseHeight, width * 96 / baseWidth, height * 54 / baseHeight);

		g.setColor(backgroundColor);
		g.fillArc(width * 98 / baseWidth, height * 84 / baseHeight, width * 44 / baseWidth, height * 34 / baseHeight, 0,
				360);
		g.fillRect(width * 98 / baseWidth, height * 100 / baseHeight, width * 44 / baseWidth, height * 46 / baseHeight);

		g = lockImage.getGraphics();
		g.setColor(backgroundColor);
		g.fillRect(0, 0, width, height);

		g.setColor(LOCK_COLOR);
		g.fillArc(width * 33 / baseWidth, height * 131 / baseHeight, width * 174 / baseWidth, height * 174 / baseHeight,
				0, 360);

		g.setColor(backgroundColor);
		g.fillArc(width * 64 / baseWidth, height * 162 / baseHeight, width * 112 / baseWidth, height * 112 / baseHeight,
				0, 360);

		int[] backgroundRgb = new int[1];

		lockImage.getRGB(backgroundRgb, 0, 1, 0, 0, 1, 1);
		lockImage = ImageProcessing.removeBackground(lockImage, backgroundRgb[0]);

		lockHandleImage.getRGB(backgroundRgb, 0, 1, 0, 0, 1, 1);
		lockHandleImage = ImageProcessing.removeBackground(lockHandleImage, backgroundRgb[0]);
	}

	protected void paint(Graphics g) {
		int goalPosition = LockGame.getGoalPosition();
		int pointerPosition = LockGame.getPointerPosition();
		int gameState = LockGame.getGameState();
		int animOpenProgress = LockGame.getAnimOpenProgress();
		int animNextLevelProgress = LockGame.getAnimNextLevelProgress();
		String scoreText = String.valueOf(LockGame.getScore());

		int deltaX = -screenWidth * animNextLevelProgress / LockGame.ANIM_NEXT_LEVEL_DURATION;

		int goalX = centerX + deltaX - goalSize / 2 + MathFunctions.kCos(ringSize / 2, goalPosition);
		int goalY = centerY - goalSize / 2 - MathFunctions.kSin(ringSize / 2, goalPosition);

		int pointerNearCenterX = centerX + deltaX + MathFunctions.kCos((ringSize - pointerWidth) / 2, pointerPosition);
		int pointerNearCenterY = centerY - MathFunctions.kSin((ringSize - pointerWidth) / 2, pointerPosition);
		int pointerFarCenterX = centerX + deltaX + MathFunctions.kCos((ringSize + pointerWidth) / 2, pointerPosition);
		int pointerFarCenterY = centerY - MathFunctions.kSin((ringSize + pointerWidth) / 2, pointerPosition);

		int pointerNearX = pointerNearCenterX - pointerHeight / 2;
		int pointerNearY = pointerNearCenterY - pointerHeight / 2;
		int pointerFarX = pointerFarCenterX - pointerHeight / 2;
		int pointerFarY = pointerFarCenterY - pointerHeight / 2;

		int pointerX1 = pointerNearCenterX + MathFunctions.kCos(pointerHeight / 2, pointerPosition + 90);
		int pointerY1 = pointerNearCenterY - MathFunctions.kSin(pointerHeight / 2, pointerPosition + 90);
		int pointerX2 = pointerNearCenterX + MathFunctions.kCos(pointerHeight / 2, pointerPosition - 90);
		int pointerY2 = pointerNearCenterY - MathFunctions.kSin(pointerHeight / 2, pointerPosition - 90);
		int pointerX3 = pointerFarCenterX + MathFunctions.kCos(pointerHeight / 2, pointerPosition + 90);
		int pointerY3 = pointerFarCenterY - MathFunctions.kSin(pointerHeight / 2, pointerPosition + 90);
		int pointerX4 = pointerFarCenterX + MathFunctions.kCos(pointerHeight / 2, pointerPosition - 90);
		int pointerY4 = pointerFarCenterY - MathFunctions.kSin(pointerHeight / 2, pointerPosition - 90);

		g.setColor(gameState != LockGame.GAME_OVER ? backgroundColor : GAME_OVER_COLOR);
		g.fillRect(0, 0, screenWidth, screenHeight);

		g.drawImage(lockHandleImage, lockImageX + deltaX,
				lockImageY - lockOpenHeight * animOpenProgress / LockGame.ANIM_OPEN_DURATION, 0);
		g.drawImage(lockImage, lockImageX + deltaX, lockImageY, 0);

		g.setColor(GOAL_COLOR);
		g.fillArc(goalX, goalY, goalSize, goalSize, 0, 360);

		g.setColor(POINTER_COLOR);
		g.fillArc(pointerNearX, pointerNearY, pointerHeight, pointerHeight, 0, 360);
		g.fillArc(pointerFarX, pointerFarY, pointerHeight, pointerHeight, 0, 360);
		g.fillTriangle(pointerX1, pointerY1, pointerX2, pointerY2, pointerX3, pointerY3);
		g.fillTriangle(pointerX2, pointerY2, pointerX3, pointerY3, pointerX4, pointerY4);

		g.setColor(SCORE_COLOR);
		g.drawString(scoreText, centerX + deltaX, centerY, 0);
	}

	private void update() {
		LockGame.update();
		if (LockGame.getSaveNeeded()) {
			saveGame();
		}
	}

	private void saveGame() {
		byte[] saveData = LockGame.saveGame();
		try {
			RecordStore rs = RecordStore.openRecordStore("LockGameSave", true);
			if (rs.getNumRecords() > 0) {
				rs.setRecord(1, saveData, 0, saveData.length);
			} else {
				rs.addRecord(saveData, 0, saveData.length);
			}
			rs.closeRecordStore();
		} catch (RecordStoreException e) {
			System.out.println("Failed to save the game");
		}
	}

	private void loadGame() {
		try {
			byte[] savedData = null;
			RecordStore rs = RecordStore.openRecordStore("LockGameSave", false);
			if (rs.getNumRecords() > 0) {
				savedData = rs.getRecord(1);
			}
			if (savedData != null) {
				LockGame.loadGame(savedData);
			}
		} catch (RecordStoreException e) {
			System.out.println("Failed to load the game");
		}
	}

	protected void pointerPressed(int x, int y) {
		LockGame.click();
	}

	protected void keyPressed(int keyCode) {
		int gameAction = getGameAction(keyCode);

		if (gameAction < 0) {
			switch (keyCode) {
			case KEY_NUM5:
				gameAction = FIRE;
				break;
			}
		}

		switch (gameAction) {
		case FIRE:
			LockGame.click();
			break;
		}
	}

	public void run() {
		while (true) {
			try {
				repaint();
				update();
				Thread.sleep(LockGame.PROGRESS_PER_FRAME);
			} catch (InterruptedException e) {
			}
		}
	}
}