import javax.microedition.lcdui.Image;

public class ImageProcessing {
	public static Image removeBackground(Image image, int color) throws IllegalArgumentException {
		if (image == null) {
			throw new IllegalArgumentException("Image is null");
		}

		int width = image.getWidth();
		int height = image.getHeight();
		int rgb[] = new int[width * height];
		image.getRGB(rgb, 0, width, 0, 0, width, height);

		for (int i = 0; i < rgb.length; i++) {
			if (rgb[i] == color) {
				rgb[i] = 0x000000;
			}
		}
		return Image.createRGBImage(rgb, width, height, true);
	}
}
