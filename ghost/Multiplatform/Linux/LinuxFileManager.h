/*
 * LinuxFileManager.h
 *
 *  Created on: 2013.03.26
 *      Author: Martynas Šustavičius
 */

#ifndef LINUXFILEMANAGER_H_
#define LINUXFILEMANAGER_H_

#include "../Ghost.h"
#include "../FileManager.h"
#include "../../../dependencies/includes/libpng/png.h"

class LinuxFileManager : public FileManager {
public:
	LinuxFileManager();
	~LinuxFileManager();

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
};

#endif
