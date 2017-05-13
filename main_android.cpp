#include "ghost/Input.h"
#include "ghost/Multiplatform/Android/AndroidServiceLocator.h"
#include "ghost/Engine.h"

extern "C" {
	JNIEXPORT void JNICALL Java_lt_smworks_smengine_JNI_provideTouchInput(JNIEnv* env, jobject obj, jint x, jint y, jboolean pressed, jlong engine);
	JNIEXPORT void JNICALL Java_lt_smworks_smengine_JNI_setString(JNIEnv* env, jobject obj, jstring name, jstring value, jlong engine);
	JNIEXPORT jlong JNICALL Java_lt_smworks_smengine_JNI_onCreate(JNIEnv* env, jobject obj,  jint width, jint height);
	JNIEXPORT void JNICALL Java_lt_smworks_smengine_JNI_onResume(JNIEnv* env, jobject obj, jlong engine);
	JNIEXPORT void JNICALL Java_lt_smworks_smengine_JNI_onPause(JNIEnv* env, jobject obj, jlong engine);
	JNIEXPORT void JNICALL Java_lt_smworks_smengine_JNI_onDestroy(JNIEnv* env, jobject obj, jlong engine);
	JNIEXPORT void JNICALL Java_lt_smworks_smengine_JNI_resize(JNIEnv* env, jobject obj,  jint width, jint height, jlong engine);
	JNIEXPORT void JNICALL Java_lt_smworks_smengine_JNI_render(JNIEnv* env, jobject obj, jlong engine);
};

JNIEXPORT void JNICALL Java_lt_smworks_smengine_JNI_setString(JNIEnv* env, jobject obj, jstring name, jstring value, jlong engine) {
//	Engine* e = (Engine*) engine;
//	if (e != 0) {
//		ServiceLocator* s = e->getServiceLocator();
//		const char* cn = env->GetStringUTFChars(name, JNI_FALSE);
//		const char* cv = env->GetStringUTFChars(value, JNI_FALSE);
//		string n(cn);
//		string v(cv);
//		s->getSettings()->setString(n, v);
//  		env->ReleaseStringUTFChars(name, cn);
//		env->ReleaseStringUTFChars(value, cv);
//	}
}

JNIEXPORT void JNICALL Java_lt_smworks_smengine_JNI_provideTouchInput(JNIEnv* env, jobject obj, jint x, jint y, jboolean pressed, jlong engine) {
	Engine* e = reinterpret_cast<Engine*>(engine);
	if (e != 0) {
		ServiceLocator* s = e->getServiceLocator();
		s->getInput()->provideTouch(x, s->getScreenHeight() - y, pressed ? Input::PRESSED : Input::RELEASED);
	}
}

JNIEXPORT jlong JNICALL Java_lt_smworks_smengine_JNI_onCreate(JNIEnv* env, jobject obj, jint width, jint height) {
	LOGI("Native onCreate");
	obj = env->NewGlobalRef(obj);
	LOGD("Creating new engine object.");
	auto* sl = new AndroidServiceLocator(env, obj);
	sl->setScreenWidth(width);
	sl->setScreenHeight(height);
	Engine* engine = new Engine(sl);
	ASSERT(engine != nullptr, "Engine instance not created");
	LOGD("Engine instance id: %lld", (jlong) engine);
	return (jlong) engine;
}

JNIEXPORT void JNICALL Java_lt_smworks_smengine_JNI_onResume(JNIEnv* env, jobject obj, jlong engine) {
	LOGI("Native onResume: %lld", engine);
	Engine* e = (Engine*) engine;
	if (e == 0) {
		LOGW("Engine instance not provided by Java runtime.");
		return;
	}	
	e->resume();
	
}

JNIEXPORT void JNICALL Java_lt_smworks_smengine_JNI_onPause(JNIEnv* env, jobject obj, jlong engine) {
	LOGI("Native onPause: %lld", engine);
	Engine* e = (Engine*) engine;
	if (e == 0) {
		LOGW("Engine instance not provided by Java runtime.");
		return;
	}
	e->pause();
}

JNIEXPORT void JNICALL Java_lt_smworks_smengine_JNI_onDestroy(JNIEnv* env, jobject obj, jlong engine) {
	LOGI("Native onDestroy: %lld", engine);
	Engine* e = (Engine*) engine;
	if (e == 0) {
		LOGW("Engine instance not provided by Java runtime.");
		return;
	}
	delete e;
	e = 0;
}

JNIEXPORT void JNICALL Java_lt_smworks_smengine_JNI_resize(JNIEnv* env, jobject obj,  jint width, jint height, jlong engine) {
	LOGI("Native resize: %lld", engine);
	Engine* e = (Engine*) engine;
	if (e == 0) {
		LOGW("Engine instance not provided by Java runtime.");
		return;
	}
	e->resizeScreen(width, height);
}

JNIEXPORT void JNICALL Java_lt_smworks_smengine_JNI_render(JNIEnv* env, jobject obj, jlong engine) {
	Engine* e = (Engine*) engine;
	if (e == 0) {
		LOGW("Engine instance not provided by Java runtime.");
		return;
	}
	e->computeFrame();
}
