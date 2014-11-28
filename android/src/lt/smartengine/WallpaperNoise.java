package lt.smartengine;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

import android.annotation.SuppressLint;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.service.wallpaper.WallpaperService;
import android.util.Log;
import android.view.Display;
import android.view.SurfaceHolder;
import android.view.WindowManager;

public class WallpaperNoise extends WallpaperService {
	
	public WallpaperNoise() {
		Log.d(SMartEngine.TAG, "Creating new wallpaper service.");
	}
	
	@Override
	public Engine onCreateEngine() {
		return new WallpaperEngine();
	}
	
	class WallpaperEngine extends Engine {
		
		/** Long containing pointer to engine object in native code. */
		private long engine;
		/** Handle to native methods. */
		private JNI jni;

		public WallpaperEngine() {
			Log.d(SMartEngine.TAG, "Creating new wallpaper engine.");
			jni = new JNI(WallpaperNoise.this);
		}
		
		private WallpaperGLSurfaceView glSurfaceView;
		private boolean rendererHasBeenSet;
		 
		@Override
		public void onCreate(SurfaceHolder surfaceHolder) {
			super.onCreate(surfaceHolder);
			glSurfaceView = new WallpaperGLSurfaceView(WallpaperNoise.this);
			// Check if the system supports OpenGL ES 2.0.
			final ActivityManager activityManager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
			final ConfigurationInfo configurationInfo = activityManager
					.getDeviceConfigurationInfo();
			final boolean supportsEs2 = configurationInfo.reqGlEsVersion >= 0x20000;
			if (supportsEs2) {
				// Use custom context factory.
				glSurfaceView.setEGLContextFactory(new ContextFactory());
				// Request an OpenGL ES 2.0 compatible context.
				setEGLContextClientVersion(2);
				// On Honeycomb+ devices, this improves the performance when
				// leaving and resuming the live wallpaper.
				setPreserveEGLContextOnPause(true);
				// Set the renderer to our user-defined renderer.
				setRenderer(new Renderer());
			} else {
				// This is where you could create an OpenGL ES 1.x compatible
				// renderer if you wanted to support both ES 1 and ES 2.
				return;
			}
		}

		@Override public void onVisibilityChanged(boolean visible) {
			Log.d(SMartEngine.TAG, "onVisibilityChanged(" + visible + ")");
			super.onVisibilityChanged(visible);
			if (rendererHasBeenSet) {
		        if (visible) {
		            glSurfaceView.onResume();
		        } else {
		            glSurfaceView.onPause();
		        }
		    }
		}

		@Override public void onSurfaceDestroyed(SurfaceHolder holder) {
			super.onSurfaceDestroyed(holder);
		}
		
		@Override public void onSurfaceChanged(SurfaceHolder holder, int format, int width, int height) {
			super.onSurfaceChanged(holder, format, width, height);
		}
		
		@Override
		public void onDestroy() {
			glSurfaceView.onDestroy();
		    super.onDestroy();
		}
		
		protected void setRenderer(Renderer renderer) {
		    glSurfaceView.setRenderer(renderer);
		    rendererHasBeenSet = true;
		}
		
		@SuppressLint("NewApi")
		protected void setEGLContextClientVersion(int version) {
			if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.FROYO) {
				glSurfaceView.setEGLContextClientVersion(version);
			}
		}
		 
		@SuppressLint("NewApi")
		protected void setPreserveEGLContextOnPause(boolean preserve) {
			if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.HONEYCOMB) {
				glSurfaceView.setPreserveEGLContextOnPause(preserve);
			}
		}
		
		private void checkEglError(String prompt, EGL10 egl) {
			int error;
			while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS) {
				Log.e(SMartEngine.TAG, String.format("%s: EGL error: 0x%x", prompt, error));
			}
		}
		
		public long getEngine() {
			return engine;
		}

		public void setEngine(long engine) {
			this.engine = engine;
		}

		class ContextFactory implements GLSurfaceView.EGLContextFactory {
			private int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

			public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig) {
				Log.d(SMartEngine.TAG, "Creating OpenGL ES 2.0 context.");
				checkEglError("Before eglCreateContext", egl);
				int[] attrib_list = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
				EGLContext context = egl.eglCreateContext(display, eglConfig, EGL10.EGL_NO_CONTEXT, attrib_list);
				checkEglError("After eglCreateContext", egl);
				return context;
			}

			public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context) {
				jni.onPause(engine);
				jni.onDestroy(engine);
				egl.eglDestroyContext(display, context);
				Log.d(SMartEngine.TAG, "OpenGL ES 2.0 context destroyed.");
			}
		}

		class WallpaperGLSurfaceView extends GLSurfaceView {
		 
		    WallpaperGLSurfaceView(Context context) {
		        super(context);
		    }
		 
		    @Override
		    public SurfaceHolder getHolder() {
		        return getSurfaceHolder();
		    }
		 
		    public void onDestroy() {
		        super.onDetachedFromWindow();
		    }
		}
		
		class Renderer implements GLSurfaceView.Renderer {
			public void onDrawFrame(GL10 gl) {
				jni.render(engine);
			}

			public void onSurfaceChanged(GL10 gl, int width, int height) {
				Log.d(SMartEngine.TAG, "onSurfaceChanged: " + engine);
				jni.resize(width, height, engine);
			}

			@SuppressWarnings("deprecation")
			public void onSurfaceCreated(GL10 gl, EGLConfig config) {
				Log.e(SMartEngine.TAG, "onSurfaceCreated");
				Display display = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
				Log.d(SMartEngine.TAG, "Starting engine with resolution: " + display.getWidth() + "x" + display.getHeight() + ".");
				engine = jni.onCreate(display.getHeight(), display.getWidth());
				jni.setString("wallpaper", "noise", engine);
				jni.onResume(engine);
			}
			
		}
	}
}
