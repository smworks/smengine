/*
 * WindowsFileManager.h
 *
 *  Created on: 2013.03.01
 *      Author: Martynas Šustavičius
 */

#ifndef WINDOWSFILEMANAGER_H_
#define WINDOWSFILEMANAGER_H_

#include "../FileManager.h"

class WindowsFileManager : public FileManager {
public:
	WindowsFileManager();
	~WindowsFileManager();

	/**
	 * @see FileManager
	 */
	bool loadRaw(INT8*& bytes, SIZE& size, const char* path);

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
};

#endif
