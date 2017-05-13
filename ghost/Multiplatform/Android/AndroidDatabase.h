#ifndef ANDROIDDATABASE_H_
#define ANDROIDDATABASE_H_

#include "../Ghost.h"
#include "../ServiceLocator.h"
#include "../Database.h"

struct sqlite3;
struct sqlite3_stmt;

class AndroidDatabase : public Database {
public:
	AndroidDatabase();
    ~AndroidDatabase();
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
