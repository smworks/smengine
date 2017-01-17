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

	bool loadRaw(INT8*& bytes, SIZE& size, const char* path);
	string loadText(string path);
	vector<string> getFiles(const char* path);
	bool fileExists(const char* path);
};

#endif
