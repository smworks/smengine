/**
 * @author Martynas Šustavičius
 * @since 2013 06 14
 */
package lt.smworks.live_wallpapers;

import java.io.IOException;
import java.io.InputStream;

import android.content.Context;
import android.util.Log;


/**
 * Class acts as a wrapper for native C++ code.
 */
public class JNI {

	static {
		//System.loadLibrary("fmodex");
		System.loadLibrary("ghost");
	}
	
	/** Handle to context. */
	private Context context = null;
	
	/**
	 * @param context - handle to application context.
	 */
	public JNI(Context context) {
		this.context = context;
	}

	/**
	 * Sends touch event to native code.
	 * @param x - column of screen pixels.
	 * @param y - row of screen pixels.
	 * @param pressed - if true, means that button is pressed.
	 * @param engine - pointer to engine object.
	 */
	public native void provideTouchInput(int x, int y, boolean pressed, long engine);
	
	/**
	 * Sends accelerometer values to native code.
	 * @param x - gravity vector x component.
	 * @param y - gravity vector y component.
	 * @param z - gravity vector z component.
	 */
	public native void provideAccelerometerInput(float x, float y, float z);
	
	/**
	 * Sends string key-value pair to inner engine Settings object.
	 * @param name - name of the string.
	 * @param value - string value.
	 * @param engine - pointer to engine object.
	 */
	public native void setString(String name, String value, long engine);

	/**
	 * Creates game engine object.
	 * @param width - the current view width.
	 * @param height - the current view height.
	 * @return Pointer to engine object, that must be passed to other methods.
	 */
	public native long onCreate(int width, int height);

	/**
	 * Starts running game engine.
	 * @param engine - pointer to engine object.
	 */
	public native void onResume(long engine);

	/**
	 * Stops game engine from doing any work.
	 * @param engine - pointer to engine object.
	 */
	public native void onPause(long engine);

	/**
	 * Destroys game engine object.
	 * @param engine - pointer to engine object.
	 */
	public native void onDestroy(long engine);

	/**
	 * Notifies game engine that current resolution has changed.
	 * @param width - new width of the screen in pixels.
	 * @param height - new height of the screen in pixels.
	 * @param engine - pointer to engine object.
	 */
	public native void resize(int width, int height, long engine);

	/**
	 * Renders one frame representing current game engine scene.
	 * @param engine - pointer to engine object.
	 */
	public native void render(long engine);
	
    /**
     * Loads specified file into byte array.
     * @param file - full path to file.
     * @return Byte array.
     */
	public byte[] loadAsset(String file) {
		try {
			InputStream is = context.getAssets().open(file);
			int size = (int) is.skip(Long.MAX_VALUE);
			byte[] buffer = new byte[size];
			is.close();
			is = context.getAssets().open(file);
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
