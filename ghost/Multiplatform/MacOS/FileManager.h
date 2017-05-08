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
	
    bool loadRaw(INT8*& bytes, SIZE& size, const char* path);
    string loadText(string path);
    vector<string> getFiles(const char* path);
    bool fileExists(const char* path);
};

#endif
