/*
 * WindowsFileManager.cpp
 *
 *  Created on: 2013.03.01
 *      Author: Martynas Šustavičius
 */

#include "WindowsFileManager.h"
#include "../../Utils.h"

WindowsFileManager::WindowsFileManager() {
	LOGD("Created Windows file manager.");
}
WindowsFileManager::~WindowsFileManager() {
	LOGD("Deleted Windows file manager.");
}

bool WindowsFileManager::loadRaw(INT8*& bytes, SIZE& size, const char* path) {
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

string WindowsFileManager::loadText(string path) {
	ifstream input(path, ios::in);
	string text;
	if (!input) {
		LOGW("Text file \"%s\" not found.", path.c_str());
		text = "";
	}
	else {
		stringstream content;
		content << input.rdbuf();
		text =  content.str();
	}
	input.close();
	return text;
}

vector<string> WindowsFileManager::getFiles(const char* path) {
	string folder = string(path) + "/*.*";
	WIN32_FIND_DATAA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	vector<string> files;
	if (folder.length() + 3 >= MAX_PATH) {
		LOGW("Folder folder \"%s\" length exceeds the maximum of %d",
			folder.c_str(), MAX_PATH);
		return files;
	}
	// Find the first file in the directory.
	hFind = FindFirstFileA(folder.c_str(), &ffd);
	if (INVALID_HANDLE_VALUE == hFind) {
		LOGW("Invalid handle value while searching "
			"for files in folder \"%s\".", folder.c_str());
		return files;
	}
	// List all the files in the directory with some info about them.
	do {
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// Directory.
		}
		else {
			files.push_back(ffd.cFileName);
		}
	}
	while (FindNextFileA(hFind, &ffd) != 0);
	DWORD dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES) {
		LOGW("Error occured while reading files in folder \"%s\".",
			folder.c_str());
		return files;
	}
	FindClose(hFind);
	return files;
}

bool WindowsFileManager::fileExists(const char* path) {
	DWORD attr = GetFileAttributesA(path);
	return (attr != INVALID_FILE_ATTRIBUTES
		&& !(attr & FILE_ATTRIBUTE_DIRECTORY));
}