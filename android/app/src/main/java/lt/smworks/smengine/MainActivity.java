package lt.smworks.smengine;

import android.annotation.SuppressLint;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.TextView;

import java.util.List;

import static android.os.Environment.getExternalStorageDirectory;

public class MainActivity extends AppCompatActivity implements View.OnTouchListener, SensorEventListener {

    public static final String TAG = "SMEngine";

//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//        setContentView(R.layout.activity_main);
//
//        // Example of a call to a native method
//        TextView tv = (TextView) findViewById(R.id.sample_text);
//        tv.setText("aksldjf");//stringFromJNI());
//    }

    private OpenGLSurface openGLSurface = null;
    private JNI jni = null;
    int sdkVersion;
    WindowManager windowManager;
    Display display;
    SensorManager sensorManager;
    boolean hasRotationSensor;
    boolean hasGameRotationSensor;
    Sensor rotationSensor;
    float rotationMatrix[] = new float[9];

    @SuppressLint({ "InlinedApi", "NewApi", "ClickableViewAccessibility" })
    @Override protected void onCreate(Bundle icicle) {
        Log.d(TAG, "onCreate");
        Log.i(TAG, "SDCard location: " + getExternalStorageDirectory().getAbsolutePath());
        Log.i(TAG, "Local content location: " + getFilesDir().getAbsolutePath());
        super.onCreate(icicle);
        sdkVersion = android.os.Build.VERSION.SDK_INT;
        jni = new JNI(this);
        try {
            openGLSurface = new OpenGLSurface(this, jni);
        } catch (Exception e) {
            Log.e(TAG, "Unable to create OpenGL surface. Error: " + e.toString());
            return;
        }
        if (sdkVersion >= 11) {
            openGLSurface.setPreserveEGLContextOnPause(true);
        }
        setContentView(openGLSurface);
        openGLSurface.setOnTouchListener(this);
        windowManager =  (WindowManager) getSystemService(WINDOW_SERVICE);
        display = windowManager.getDefaultDisplay();
        sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        List<Sensor> supportedSensors = sensorManager.getSensorList(Sensor.TYPE_ALL);
        Log.d(TAG, "Maximum available JVM memory: " + Runtime.getRuntime().maxMemory() / 1024 / 1024 + "MB");
        Log.d(TAG, "Supported sensors:");
        for (Sensor sensor : supportedSensors) {
            Log.d(TAG, String.format("  Sensor: %s\n" +
                            "    Vendor: %s\n    Version: %d\n    Maximum range: %.2f\n" +
                            "    Minimum delay: %d (micro seconds)\n    Used power: %.2f (mA)\n    Resolution: %.2f",
                    sensor.getName(), sensor.getVendor(), sensor.getVersion(), sensor.getMaximumRange(),
                    sdkVersion >= 9 ? sensor.getMinDelay() : 0, sensor.getPower(), sensor.getResolution()));
            if (sdkVersion >= 9 && sensor.getType() == Sensor.TYPE_ROTATION_VECTOR) {
                hasRotationSensor = true;
            } else if (sdkVersion >= 18 && sensor.getType() == Sensor.TYPE_GAME_ROTATION_VECTOR) {
                hasGameRotationSensor = true;
            }
        }
        if (hasGameRotationSensor) {
            rotationSensor = sensorManager.getDefaultSensor(Sensor.TYPE_GAME_ROTATION_VECTOR);
        } else if (hasRotationSensor) {
            rotationSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);
        }
    }

    @Override protected void onResume() {
        Log.d(TAG, "onResume");
        super.onResume();
        openGLSurface.onResume();
        if (hasRotationSensor) {
            sensorManager.registerListener(this, rotationSensor, SensorManager.SENSOR_DELAY_FASTEST);
        }
    }

    @Override protected void onPause() {
        Log.d(TAG, "onPause");
        if (hasRotationSensor) {
            sensorManager.unregisterListener(this);
        }
        if (isFinishing()) {
            openGLSurface.queueEvent(new Runnable() {
                @Override
                public void run() {
                    jni.onPause(openGLSurface.getEngine());
                    jni.onDestroy(openGLSurface.getEngine());
                    openGLSurface.setEngine(0);
                }
            });
        }
        openGLSurface.onPause();
        super.onPause();
    }

    @Override protected void onDestroy() {
        Log.d(TAG, "onDestroy");
        super.onDestroy();
    }

    @Override public boolean onTouch(View v, MotionEvent e) {
        final int x = (int) e.getX();
        final int y = (int) e.getY();
        final int action = e.getAction();
        openGLSurface.queueEvent(new Runnable() {
            @Override
            public void run() {
                jni.provideTouchInput(x, y, action != MotionEvent.ACTION_UP, openGLSurface.getEngine());
            }
        });
        return true;
    }

    @Override public void onAccuracyChanged(Sensor sensor, int accuracy) {}

    @SuppressLint("NewApi")
    @Override public void onSensorChanged(SensorEvent event) {
//		if (event.sensor.getType() == Sensor.TYPE_ROTATION_VECTOR) {
//			SensorManager.getRotationMatrixFromVector(rotationMatrix, event.values);
////			double x = Math.toDegrees(event.values[0]);
////			double y = Math.toDegrees(event.values[1]);
////			double z = Math.toDegrees(event.values[2]);
//			double x = Math.toDegrees(Math.atan2(rotationMatrix[7], rotationMatrix[8]));
//			double y = Math.toDegrees(Math.atan2(-rotationMatrix[6], Math.sqrt(
//				rotationMatrix[7] * rotationMatrix[7] + rotationMatrix[8] * rotationMatrix[8])));
//			double z = Math.toDegrees(Math.atan2(rotationMatrix[3], rotationMatrix[0]));
//			//Log.d(TAG, "Rotation vector: " + x + ", " + y + ", " + z);
//		}
    }
}
