/*
 * WindowsDatabase.cpp
 *
 *  Created on: 2014.12.09
 *      Author: Martynas Šustavičius
 */

#include "WindowsDatabase.h"
#include "WindowsFileManager.h"

#include "../../../dependencies/includes/sqlite/sqlite3.h"

#define DB_OPEN_RWC SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
#define LOGDBI(...) \
	LOGI(__VA_ARGS__); \
	LOGI("Error: %d, %s.", err_, sqlite3_errmsg(db_));

WindowsDatabase::WindowsDatabase(FileManager* fileManager, const string& name) :
	Database(fileManager, name),
	db_(0),
	err_(0)
{
	if (initializeSQLEngine() && openConnectionToDB()) {
		//execute("CREATE TABLE IF NOT EXISTS strings (name TEXT UNIQUE, value TEXT)");
	}
}

bool WindowsDatabase::initializeSQLEngine() {
	if (SQLITE_OK != (err_ = sqlite3_initialize())) {
		LOGDBI("Failed to initialize library.");
	}
	return SQLITE_OK == err_;
}

bool WindowsDatabase::openConnectionToDB() {
	if (SQLITE_OK != (err_ = sqlite3_open_v2("database.db", &db_, DB_OPEN_RWC, NULL))) {
		LOGDBI("Failed to open connection.");
    }
	return SQLITE_OK == err_;
}

WindowsDatabase::~WindowsDatabase() {
	LOGD("Deleted Windows database.");
	if (db_ != 0) sqlite3_close(db_);
	db_ = 0;
	sqlite3_shutdown();
}

WindowsDatabase::ResultSet WindowsDatabase::execute(string query) {
	LOGD("Executing query: %s.", query.c_str());
	sqlite3_stmt* stmt = createStatement(query + ";");
	ResultSet rs;
	while (executeStep(stmt)) {
		rs.vec.push_back(getData(stmt));
	}
	destroyStatement(stmt);
	return rs;
}

bool WindowsDatabase::executeStep(sqlite3_stmt* stmt) {
	err_ = sqlite3_step(stmt);
	return SQLITE_ROW == err_;
}

unordered_map<string, string> WindowsDatabase::getData(sqlite3_stmt* stmt) {
	unordered_map<string, string> um;
	int count = sqlite3_column_count(stmt);
	for (int i = 0; i < count; i++) {
		string key = sqlite3_column_name(stmt, i);
		string value(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
		um.insert(pair<string, string>(key, value));
	}
	return um;
}

sqlite3_stmt* WindowsDatabase::createStatement(string query) {
	sqlite3_stmt* s = 0;
	if (SQLITE_OK != (err_ = sqlite3_prepare_v2(db_, query.c_str(), query.size(), &s, NULL))) {
		LOGDBI("Failed to compile query: \"%s\".", query.c_str());
	}
	return s;
}

void WindowsDatabase::destroyStatement(sqlite3_stmt* stmt) {
	if (stmt != 0 && SQLITE_OK != (err_ = sqlite3_finalize(stmt))) {
		LOGDBI("Destroying statement.");
	}
}