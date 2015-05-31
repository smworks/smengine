#ifndef LINUXDATABASE_H_
#define LINUXDATABASE_H_

#include "../Ghost.h"
#include "../ServiceLocator.h"
#include "../Database.h"

struct sqlite3;
struct sqlite3_stmt;

class LinuxDatabase : public Database {
public:
	LinuxDatabase(FileManager* fileManager, const string& name = "") : Database(fileManager, name) {}
	~LinuxDatabase() {}
	ResultSet execute(string query) { ResultSet rs; return rs; }
protected:
//	bool initializeSQLEngine();
//	bool openConnectionToDB();
//	bool executeStep(sqlite3_stmt* stmt);
//	unordered_map<string, string> getData(sqlite3_stmt* stmt);
//	sqlite3_stmt* createStatement(string query);
//	void destroyStatement(sqlite3_stmt* stmt);
private:
//	sqlite3* db_;
//	int err_;
};

#endif
