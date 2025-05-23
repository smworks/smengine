﻿#ifndef WINDOWSDATABASE_H_
#define WINDOWSDATABASE_H_

#include "../Ghost.h"
#include "../ServiceLocator.h"
#include "../Database.h"

#ifndef ENABLE_DATABASE
class WindowsDatabase : public Database {
public:
	WindowsDatabase() :
		Database(fileManager, name) {}
	~WindowsDatabase() {}
	ResultSet execute(string query) { ResultSet rs; return rs; }
};
#else

struct sqlite3;
struct sqlite3_stmt;

class WindowsDatabase : public Database {
public:
	WindowsDatabase();
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
#endif