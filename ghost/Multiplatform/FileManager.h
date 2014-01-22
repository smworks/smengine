/*
 * FileManager.h
 *
 *  Created on: 2013.03.01
 *      Author: Martynas Šustavičius
 */

#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include "Ghost.h"

class FileManager {
public:
	virtual ~FileManager() {}

	/**
	 * Loads file into byte array.
	 * @param bytes - pointer that will point
	 * to newly created byte array.
	 * @param size - size of bytes array.
	 * @param path - path to file.
	 * @return True on success, false otherwise.
	 */
	virtual bool loadRaw(INT8*& bytes, SIZE& size, const char* path) = 0;

	/**
	 * Loads text file from assets folder.
	 * @param text - string containing loaded
	 * file or empty line if file was not
	 * found or is empty.
	 * @param path - path to file in assets folder.
	 */
	virtual void loadText(string& text, const char* path) = 0;

	/**
	 * Finds all files in specified folder.
	 * @param path - path to folder.
	 * @return vector containing file names.
	 */
	virtual vector<string> getFiles(const char* path) = 0;

	/**
	 * Checks if file exists.
	 * @param path - path to file.
	 * @return True if file exists.
	 */
	virtual bool fileExists(const char* path) = 0;
};

#endif
