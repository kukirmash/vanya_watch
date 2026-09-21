import java.util.Random;

public class LockGame {
	private static final int GOAL_SPAWN_DISTANCE = 30;
	private static final int GOAL_SPAWN_LENGTH = 180;
	public static final int GOAL_LENGTH = 5;

	private static final int POINTER_INITIAL_POSITION = 90;
	private static final boolean POINTER_INITIAL_DIRECTION = false;
	private static final int POINTER_SPEED = 300;
	public static final int POINTER_LENGTH = 1;

	private static final int CIRCLE_FULL_STEP = 100;
	private static final int CIRCLE_FULL_LENGTH = 360 * CIRCLE_FULL_STEP;

	private static final int SKIN_COUNT = 1;

	public static final int PROGRESS_PER_FRAME = 33;
	public static final int ANIM_OPEN_DURATION = 300;
	public static final int ANIM_NEXT_LEVEL_DURATION = 700;

	public static final int GAME_READY = 0;
	public static final int GAME_POINTER_BEFORE_GOAL = 1;
	public static final int GAME_POINTER_INSIDE_GOAL = 2;
	public static final int GAME_OVER = 3;
	public static final int GAME_ANIM_OPEN = 4;
	public static final int GAME_ANIM_NEXT_LEVEL = 5;

	private static int level;
	private static int score;
	private static int money;
	private static int skin;
	private static boolean[] unlockedSkins;
	private static boolean saveNeeded;

	private static int pointerPosition;
	private static boolean isPointerDirectionPositive;

	private static int goalPosition;

	private static int animOpenProgress;
	private static int animNextLevelProgress;

	private static int gameState;
	private static Random random;

	static {
		level = 1;
		money = 0;
		skin = 0;
		unlockedSkins = new boolean[SKIN_COUNT];
		unlockedSkins[0] = true;
		saveNeeded = false;
		random = new Random();
		restart();
	}

	public static void loadGame(byte[] data) {
		try {
			level = data[0] & 0xFF;
			skin = data[1] & 0xFF;
			money = (data[2] & 0xFF) | ((data[3] >> 8) & 0xFF) | ((data[4] >> 16) & 0xFF) | ((data[5] >> 24) & 0xFF);
			for (int i = 0; i < SKIN_COUNT; i++) {
				unlockedSkins[i] = ((data[6 + i / 8] >> (i % 8)) & 0x1) == 1;
			}
		} catch (IndexOutOfBoundsException e) {
		}
		restart();
	}

	public static byte[] saveGame() {
		byte[] data = new byte[6 + (SKIN_COUNT + 7) / 8];

		data[0] = (byte) (level & 0xFF);
		data[1] = (byte) (skin & 0xFF);
		data[2] = (byte) (money & 0xFF);
		data[3] = (byte) ((money >> 8) & 0xFF);
		data[4] = (byte) ((money >> 16) & 0xFF);
		data[5] = (byte) ((money >> 24) & 0xFF);

		for (int i = 0; i < SKIN_COUNT; i++) {
			data[6 + i / 8] |= (unlockedSkins[i] ? 1 : 0) << (i % 8);
		}
		saveNeeded = false;

		return data;
	}

	private static void restart() {
		score = level;
		pointerPosition = POINTER_INITIAL_POSITION * CIRCLE_FULL_STEP;
		isPointerDirectionPositive = POINTER_INITIAL_DIRECTION;
		gameState = GAME_READY;
		animOpenProgress = 0;
		animNextLevelProgress = 0;
		spawnGoal();
	}

	private static void spawnGoal() {
		int position = random.nextInt(GOAL_SPAWN_LENGTH);
		goalPosition = MathFunctions.mod(
				pointerPosition
						+ (isPointerDirectionPositive ? 1 : -1) * (GOAL_SPAWN_DISTANCE + position) * CIRCLE_FULL_STEP,
				CIRCLE_FULL_LENGTH);
	}

	public static void update() {
		switch (gameState) {
		case GAME_POINTER_BEFORE_GOAL:
		case GAME_POINTER_INSIDE_GOAL:

			pointerPosition = MathFunctions.mod(pointerPosition + (isPointerDirectionPositive ? 1 : -1) * POINTER_SPEED,
					CIRCLE_FULL_LENGTH);

			int goalLength = GOAL_LENGTH * CIRCLE_FULL_STEP;
			if (goalPosition - goalLength <= pointerPosition && pointerPosition <= goalPosition + goalLength
					|| goalPosition - goalLength < 0
							&& MathFunctions.mod(goalPosition - goalLength, CIRCLE_FULL_LENGTH) <= pointerPosition
					|| goalPosition + goalLength >= CIRCLE_FULL_LENGTH
							&& pointerPosition <= MathFunctions.mod(goalPosition + goalLength, CIRCLE_FULL_LENGTH)) {
				gameState = GAME_POINTER_INSIDE_GOAL;
			} else if (gameState == GAME_POINTER_INSIDE_GOAL) {
				gameState = GAME_OVER;
			}

			break;

		case GAME_ANIM_OPEN:
			animOpenProgress += PROGRESS_PER_FRAME;

			if (animOpenProgress >= ANIM_OPEN_DURATION) {
				gameState = GAME_ANIM_NEXT_LEVEL;
			}

			break;

		case GAME_ANIM_NEXT_LEVEL:
			animNextLevelProgress += PROGRESS_PER_FRAME;

			if (animNextLevelProgress >= ANIM_NEXT_LEVEL_DURATION) {
				restart();
			}

			break;
		}

	}

	public static void click() {
		switch (gameState) {
		case GAME_READY:
			gameState = GAME_POINTER_BEFORE_GOAL;
			break;

		case GAME_POINTER_BEFORE_GOAL:
			gameState = GAME_OVER;
			break;

		case GAME_POINTER_INSIDE_GOAL:
			score--;
			if (score <= 0) {
				level++;
				saveNeeded = true;
				gameState = GAME_ANIM_OPEN;
			} else {
				gameState = GAME_POINTER_BEFORE_GOAL;
				isPointerDirectionPositive ^= true;
				spawnGoal();
			}
			break;

		case GAME_OVER:
			restart();
			break;
		}
	}

	public static int getLevel() {
		return level;
	}

	public static int getScore() {
		return score;
	}

	public static int getPointerPosition() {
		return pointerPosition / CIRCLE_FULL_STEP;
	}

	public static int getGoalPosition() {
		return goalPosition / CIRCLE_FULL_STEP;
	}

	public static int getGameState() {
		return gameState;
	}

	public static int getAnimOpenProgress() {
		return animOpenProgress;
	}

	public static int getAnimNextLevelProgress() {
		return animNextLevelProgress;
	}

	public static boolean getSaveNeeded() {
		return saveNeeded;
	}
}