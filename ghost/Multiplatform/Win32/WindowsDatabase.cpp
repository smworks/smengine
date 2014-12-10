/*
 * WindowsDatabase.cpp
 *
 *  Created on: 2014.12.09
 *      Author: Martynas Šustavičius
 */

#include "WindowsDatabase.h"
#include "WindowsFileManager.h"

WindowsDatabase::WindowsDatabase(FileManager* fileManager, const string& name) : Database(fileManager, name) {

}

WindowsDatabase::~WindowsDatabase() {
}