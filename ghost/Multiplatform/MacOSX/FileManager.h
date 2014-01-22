/*
 * FileManager.h
 *
 *  Created on: 2013.08.24
 *      Author: Martynas Šustavičius
 */

#ifndef MACOSXFILEMANAGER_H_
#define MACOSXFILEMANAGER_H_

#include "../Ghost.h"
#include "../FileManager.h"
#include "../../../dependencies/includes/libpng/png.h"

class MacOSXFileManager : public FileManager {
public:
	MacOSXFileManager();
	~MacOSXFileManager();
	
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
