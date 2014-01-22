/**
 * @author Martynas Šustavičius
 * @since 2013 06 14
 */
package lt.smartengine;

import java.io.IOException;
import java.io.InputStream;

import android.util.Log;


/**
 * Class acts as a wrapper for native C++ code.
 */
public class JNI {

	static {
		System.loadLibrary("fmodex");
		System.loadLibrary("ghost");
	}
	
	/** Handle to main activity. */
	private SMartEngine activity = null;
	
	/**
	 * @param activity - handle to main activity.
	 */
	public JNI(SMartEngine activity) {
		this.activity = activity;
	}

	/**
	 * Sends touch event to native code.
	 * @param x - column of screen pixels.
	 * @param y - row of screen pixels.
	 * @param pressed - if true, means that button is pressed.
	 */
	public native void provideTouchInput(int x, int y, boolean pressed);
	
	/**
	 * Sends accelerometer values to native code.
	 * @param x - gravity vector x component.
	 * @param y - gravity vector y component.
	 * @param z - gravity vector z component.
	 */
	public native void provideAccelerometerInput(float x, float y, float z);

	/**
	 * Creates game engine object.
	 * @param width - the current view width.
	 * @param height - the current view height.
	 */
	public native void onCreate(int width, int height);

	/**
	 * Starts running game engine.
	 */
	public native void onResume();

	/**
	 * Stops game engine from doing any work.
	 */
	public native void onPause();

	/**
	 * Destroys game engine object.
	 */
	public native void onDestroy();

	/**
	 * Notifies game engine that current resolution has changed.
	 * @param width - new width of the screen in pixels.
	 * @param height - new height of the screen in pixels.
	 */
	public native void resize(int width, int height);

	/**
	 * Renders one frame representing current game engine scene.
	 */
	public native void render();
	
    /**
     * Loads specified file into byte array.
     * @param file - full path to file.
     * @return Byte array.
     */
	public byte[] loadAsset(String file) {
		try {
			InputStream is = activity.getAssets().open(file);
			int size = (int) is.skip(Long.MAX_VALUE);
			byte[] buffer = new byte[size];
			is.close();
			is = activity.getAssets().open(file);
			int offset = 0;
			int nRead;
			byte[] data = new byte[8192];
			while ((nRead = is.read(data, 0, data.length)) != -1) {
				for (int i = 0; i < nRead; i++) {
					buffer[offset++] = data[i];
				}
			}
			return buffer;
		} catch (IOException e) {
			return new byte[0];
		} catch (OutOfMemoryError e) {
			Log.e(SMartEngine.TAG, "Unable to load file " + file + " due to insufficient memory.");
			return new byte[0];
		}
	}
}
