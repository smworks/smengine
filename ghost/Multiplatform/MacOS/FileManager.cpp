
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

string MacOSXFileManager::loadText(string path)
{
    PROFILE("Started loading text file: %s.", path.c_str());
    ios_base::openmode params = ios::in;
    string text;
    ifstream input(path.c_str(), params);
    if (!input) {
        LOGW("Text file \"%s\" not found.", path.c_str());
        text = "";
    }
    else
    {
        stringstream content;
        content << input.rdbuf();
        text =  content.str();
    }
    input.close();
    PROFILE("Finished loading text file to string.");
    return text;
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