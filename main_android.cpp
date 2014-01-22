/*
 * main.cpp
 *
 *  Created on: 2012.06.19
 *      Author: MS
 */

#include "ghost/Input.h"
#include "ghost/Multiplatform/Android/AndroidServiceLocator.h"
#include "ghost/Engine.h"

GLuint gProgram;
GLuint gvPositionHandle;

ServiceLocator* __services__ = 0;
Engine* __ghost__ = 0;
Input* __input__ = 0;

extern "C" {
	JNIEXPORT void JNICALL Java_lt_smartengine_JNI_provideTouchInput(JNIEnv* env, jobject obj, jint x, jint y, jboolean pressed);
    JNIEXPORT void JNICALL Java_lt_smartengine_JNI_onCreate(JNIEnv* env, jobject obj,  jint width, jint height);
	JNIEXPORT void JNICALL Java_lt_smartengine_JNI_onResume(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_lt_smartengine_JNI_onPause(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_lt_smartengine_JNI_onDestroy(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_lt_smartengine_JNI_resize(JNIEnv* env, jobject obj,  jint width, jint height);
    JNIEXPORT void JNICALL Java_lt_smartengine_JNI_render(JNIEnv* env, jobject obj);
};

JNIEXPORT void JNICALL Java_lt_smartengine_JNI_provideTouchInput(JNIEnv* env, jobject obj, jint x, jint y, jboolean pressed) {
	if (__input__ != 0) {
		__input__->provideTouch(
			x, __services__->getScreenHeight() - y,	pressed ? Input::PRESSED : Input::RELEASED);
	}
}

JNIEXPORT void JNICALL Java_lt_smartengine_JNI_onCreate(JNIEnv* env, jobject obj, jint width, jint height) {
	obj = env->NewGlobalRef(obj);
	__services__ = new AndroidServiceLocator(env, obj);
	__ghost__ = new Engine(__services__);
	__input__ = __ghost__->getServiceLocator()->getInput();
	__ghost__->resizeScreen(width, height);
}

JNIEXPORT void JNICALL Java_lt_smartengine_JNI_onResume(JNIEnv* env, jobject obj) {
	__ghost__->resume();
}

JNIEXPORT void JNICALL Java_lt_smartengine_JNI_onPause(JNIEnv* env, jobject obj) {
	__ghost__->pause();
}

JNIEXPORT void JNICALL Java_lt_smartengine_JNI_onDestroy(JNIEnv* env, jobject obj) {
	delete __ghost__;
	__ghost__ = 0;
}

JNIEXPORT void JNICALL Java_lt_smartengine_JNI_resize(JNIEnv* env, jobject obj,  jint width, jint height) {
	if (__ghost__ != 0) {
		__ghost__->resizeScreen(width, height);
	}
}

JNIEXPORT void JNICALL Java_lt_smartengine_JNI_render(JNIEnv* env, jobject obj) {
	if (__ghost__ != 0) {
		__ghost__->computeFrame();
	}
}
