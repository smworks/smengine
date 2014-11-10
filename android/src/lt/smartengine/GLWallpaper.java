package lt.smartengine;

import android.annotation.SuppressLint;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.service.wallpaper.WallpaperService;
import android.util.Log;
import android.view.SurfaceHolder;

public abstract class GLWallpaper extends WallpaperService {
	
	@Override
	public Engine onCreateEngine() {
		return new WallpaperEngine();
	}
	
	abstract Renderer getNewRenderer();

	class WallpaperEngine extends Engine {

		public WallpaperEngine() {

		}
		
		private WallpaperGLSurfaceView glSurfaceView;
		private boolean rendererHasBeenSet;
		 
		@Override
		public void onCreate(SurfaceHolder surfaceHolder) {
			super.onCreate(surfaceHolder);
			glSurfaceView = new WallpaperGLSurfaceView(GLWallpaper.this);
			// Check if the system supports OpenGL ES 2.0.
			final ActivityManager activityManager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
			final ConfigurationInfo configurationInfo = activityManager
					.getDeviceConfigurationInfo();
			final boolean supportsEs2 = configurationInfo.reqGlEsVersion >= 0x20000;

			if (supportsEs2) {
				// Request an OpenGL ES 2.0 compatible context.
				setEGLContextClientVersion(2);

				// On Honeycomb+ devices, this improves the performance when
				// leaving and resuming the live wallpaper.
				setPreserveEGLContextOnPause(true);

				// Set the renderer to our user-defined renderer.
				setRenderer(getNewRenderer());
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
		    super.onDestroy();
		    glSurfaceView.onDestroy();
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
	}
}
