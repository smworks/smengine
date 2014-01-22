/*
 * FileManager.cpp
 *
 *  Created on: 2013.08.24
 *      Author: Martynas Šustavičius
 */

#include "FileManager.h"

MacOSXFileManager::MacOSXFileManager() {
    PROFILE("Creating FileManager object.");
    PROFILE("FileManager object created.");
}
MacOSXFileManager::~MacOSXFileManager() {}

bool MacOSXFileManager::loadRaw(
	INT8*& bytes, SIZE& size, const char* path)
{
	ifstream file(path, ios::in | ios::binary);
	if (!file.is_open()) {
		bytes = 0;
		size = 0;
		LOGW("Unable to open \"%s\" file.", path);
		return false;
	}
	file.seekg(0, ios::end);
	size = (SIZE) file.tellg();
	file.seekg(0, ios::beg);
	bytes = NEW INT8[size];
	file.read(bytes, size);
	return true;
}

void MacOSXFileManager::loadText(string& text, const char* path) {
	ifstream input(path, ios::in);
	if (!input) {
		LOGW("Text file \"%s\" not found.", path);
		text = "";
	}
	else {
		stringstream content;
		content << input.rdbuf();
		text =  content.str();
	}
}

vector<string> MacOSXFileManager::getFiles(const char* path) {
	vector<string> files;
	LOGW("FileManager::getFiles not implemented.");
	return files;
}

bool MacOSXFileManager::fileExists(const char* path) {
	ifstream my_file(path);
	if (my_file.good()) {
		return true;
	}
	return false;
}