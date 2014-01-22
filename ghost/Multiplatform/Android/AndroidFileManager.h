/*
 * AndroidFileManager.h
 *
 *  Created on: 2013.03.31
 *      Author: Martynas Šustavičius
 */

#ifndef ANDROIDFILEMANAGER_H_
#define ANDROIDFILEMANAGER_H_

#include "../Ghost.h"
#include "../FileManager.h"
#include "../../../dependencies/includes/libpng/png.h"

class AndroidFileManager : public FileManager {
public:
	/**
	 * @param env - java environment.
	 * @param obj - class object in java that contains
	 * methods used by native code.
	 */
	AndroidFileManager(JNIEnv* env, jobject obj);
	~AndroidFileManager();

	/**
	 * @see FileManager
	 */
	bool loadRaw(
		INT8*& bytes, SIZE& size, const char* path);

	/**
	 * @see FileManager
	 */
	void loadText(string& text, const char* path);

	/**
	 * @see FileManager
	 */
	vector<string> getFiles(const char* path);

	/**
	 * @see FileManager
	 */
	bool fileExists(const char* path);
private:
	JNIEnv* env_;
	jobject obj_;
};

#endif
