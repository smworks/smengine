/*
 * FileManager.cpp
 *
 *  Created on: 2013.08.24
 *      Author: Martynas Šustavičius
 */

#include "FileManager.h"
#include <CoreData/CoreData.h>
#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>

#define HEADER_SIZE 8

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

vector<string> MacOSXFileManager::getFiles(const string& path) {
	vector<string> files;
	LOGW("FileManager::getFiles not implemented.");
	return files;
}

bool MacOSXFileManager::fileExists(const string& path) {
	ifstream my_file(path.c_str());
	if (my_file.good()) {
		return true;
	}
	return false;
}

//bool MacOSXFileManager::loadPng(
//	const string& path,
//	UINT8*& buffer,
//	UINT32& width,
//	UINT32& height,
//	bool upperLeft)
//{
//    ofstream myfile;
//    myfile.open ("example.txt");
//    myfile << "Writing this to a file.\n";
//    myfile.close();
//	if (path.length() == 0) {
//		LOGW("No image file path specified. Unable to load image.");
//		return false;
//	}
//    CGDataProviderRef source = 0;
//    const char* filename = path.c_str();
//    source = CGDataProviderCreateWithFilename(filename);
//    if (source == 0) {
//        LOGW("Image not found under path: \"%s\"", filename);
//		return false;
//    }
//    CGImageRef image = 0;
//    image = CGImageCreateWithPNGDataProvider(source, NULL, YES, kCGRenderingIntentDefault);
//    //ggimage->components = (int)(CGImageGetBitsPerPixel(image) / 8);
//    source = CGImageGetDataProvider(image);
//    CFDataRef data = CGDataProviderCopyData(source);
//    width = (UINT32) CGImageGetWidth(image);
//    height = (UINT32) CGImageGetHeight(image);
//    buffer = NEW UINT8[CFDataGetLength(data)];
//    GLubyte* imageData = (GLubyte*) CFDataGetBytePtr(data);
//    memcpy(buffer, imageData, CFDataGetLength(data));
//    long rowSize = CFDataGetLength(data) / height;
//    for (UINT32 i = 0; i < height; i++) {
//        long offset = rowSize * i;
//        long sourceOffset = rowSize * (height - 1 - i);
//        memcpy(buffer + offset, imageData + sourceOffset, rowSize);
//    }
//    CGDataProviderRelease(source);
//    //CGImageRelease(image);
//    CFRelease(data);
//    
//    return true;
//}
