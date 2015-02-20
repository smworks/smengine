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

struct sqlite3;
struct sqlite3_stmt;

class WindowsDatabase : public Database {
public:
	WindowsDatabase(FileManager* fileManager, const string& name = "");
	~WindowsDatabase();
	ResultSet execute(string query);
protected:
	bool initializeSQLEngine();
	bool openConnectionToDB();
	bool executeStep(sqlite3_stmt* stmt);
	unordered_map<string, string> getData(sqlite3_stmt* stmt);
	sqlite3_stmt* createStatement(string query);
	void destroyStatement(sqlite3_stmt* stmt);
private:
	sqlite3* db_;
	int err_;
};

#endif