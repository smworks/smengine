/*
 * AndroidFileManager.cpp
 *
 *  Created on: 2013.03.31
 *      Author: Martynas Šustavičius
 */

#include "AndroidFileManager.h"

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

void AndroidFileManager::loadText(string& text, const char* path) {
	INT8* charArr;
	SIZE len;
	loadRaw(charArr, len, path);
	string tmp(reinterpret_cast<char*>(charArr), len);
	text = tmp;
}

vector<string> AndroidFileManager::getFiles(const char* path) {
	/*char p[path.length()];
	for (int i = 0; i < path.length() - 1; i++) {
		p[i] = path[i];
	}
	p[path.length()] = '\0';
	AAssetDir* dir = AAssetManager_openDir(assetManager_, "shaders");
	vector<string> files;
	const char* filename;
	while ((filename = AAssetDir_getNextFileName(dir)) != 0) {
		string file(filename);
		files.push_back(file);

	}
	AAssetDir_close(dir);*/
	vector<string> files;
	return files;
}

bool AndroidFileManager::fileExists(const char* path) {
	/*AAsset* asset = AAssetManager_open(assetManager_, path.c_str(),
				AASSET_MODE_UNKNOWN);
	if (asset == 0) {
		return false;
	}
	AAsset_close(asset);*/
	return true;
}
