/*
 * WindowsDatabase.h
 *
 *  Created on: 2014.12.09
 *      Author: Martynas Šustavičius
 */

#ifndef WINDOWSDATABASE_H_
#define WINDOWSDATABASE_H_

#include "../Ghost.h"
#include "../ServiceLocator.h"
#include "../Database.h"

class WindowsDatabase : public Database {
public:
	WindowsDatabase(FileManager* fileManager, const string& name = "");
	~WindowsDatabase();
};

#endif