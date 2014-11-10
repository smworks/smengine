package lt.smartengine;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.Display;
import android.view.WindowManager;

public class Wallpaper extends GLWallpaper {
	
	private JNI jni;
	
	public Wallpaper() {
		jni = new JNI(this);
	}

	@Override Renderer getNewRenderer() {
		return new Renderer();
	}
	
	@Override public void onDestroy() {
		super.onDestroy();
		jni.onPause();
		jni.onDestroy();
	}
	
	class Renderer implements GLSurfaceView.Renderer {
		public void onDrawFrame(GL10 gl) {
			jni.render();
		}

		public void onSurfaceChanged(GL10 gl, int width, int height) {
			Log.d(SMartEngine.TAG, "Surface changed.");
			jni.resize(width, height);
		}

		@SuppressWarnings("deprecation")
		public void onSurfaceCreated(GL10 gl, EGLConfig config) {
			Log.d(SMartEngine.TAG, "Surface created.");
			Display display = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
			Log.d(SMartEngine.TAG, "Starting engine with resolution: " + display.getWidth() + "x" + display.getHeight() + ".");
			jni.onCreate(display.getHeight(), display.getWidth());
			jni.onResume();
		}
		
	}
}