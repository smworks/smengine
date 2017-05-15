/*
 * AndroidFileManager.cpp
 *
 *  Created on: 2013.03.31
 *      Author: Martynas Šustavičius
 */

#include "AndroidFileManager.h"

//        Signature	                Java Type
//        Z	                        boolean
//        B	                        byte
//        C	                        char
//        S	                        short
//        I	                        int
//        J	                        long
//        F	                        float
//        D	                        double
//        L fully-qualified-class ;	fully-qualified-class
//        [ type	                type[]
//        ( arg-types ) ret-type	method type

AndroidFileManager::AndroidFileManager(JNIEnv* env, jobject obj) :
	env_(env),
	obj_(obj)
{
	if (env_ == 0) {
		LOGE("Java environment variable not initialized.");
	}
	if (obj_ == 0) {
		LOGE("Java class object that contains methods used by native code is not initialized.");
	}
	LOGD("Created Android file manager.");
}
AndroidFileManager::~AndroidFileManager() {
	LOGD("Deleted Android file manager.");
}

bool AndroidFileManager::loadRaw(
	INT8*& bytes, SIZE& size, const char* path)
{
	jstring jstr = env_->NewStringUTF(path);
	jclass objClass = env_->GetObjectClass(obj_);
	jmethodID messageMe = env_->GetMethodID(objClass, "loadAsset", "(Ljava/lang/String;)[B");
	jobject result = env_->CallObjectMethod(obj_, messageMe, jstr);
	jbyteArray arr = (jbyteArray) result;
	jsize len = env_->GetArrayLength(arr);
	size = len;
	jbyte* buff = env_->GetByteArrayElements(arr, 0);
	bytes = NEW INT8[size];
	memcpy(bytes, buff, size);
	env_->ReleaseByteArrayElements(arr, buff, 0);
	if (size == 0) {
		delete [] bytes;
		bytes = 0;
		return false;
	}
	return true;
}

string AndroidFileManager::loadText(string path) {
	INT8* charArr;
	SIZE len;
	loadRaw(charArr, len, path.c_str());
	string tmp(charArr, len);
	return tmp;
}

vector<string> AndroidFileManager::getFiles(const char* path) {
	vector<string> files;
	return files;
}

bool AndroidFileManager::fileExists(const char* path) {
	jstring jstr = env_->NewStringUTF(path);
	jclass objClass = env_->GetObjectClass(obj_);
	jmethodID messageMe = env_->GetMethodID(objClass, "fileExists", "(Ljava/lang/String;)Z");
	jboolean result = env_->CallBooleanMethod(obj_, messageMe, jstr);
	return result == JNI_TRUE;
}
