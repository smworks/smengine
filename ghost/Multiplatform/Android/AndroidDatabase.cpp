#include "AndroidDatabase.h"
#include "../FileManager.h"

#include "../../../dependencies/includes/sqlite/sqlite3.h"

#define DB_OPEN_RWC SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
#define LOGDBI(...) \
	LOGI(__VA_ARGS__); \
	LOGI("Error: %d, %s.", err_, sqlite3_errmsg(db_));

AndroidDatabase::AndroidDatabase() :
	db_(0),
	err_(0)
{
    if (initializeSQLEngine() && openConnectionToDB()) {
        prepare();
    }
}

bool AndroidDatabase::initializeSQLEngine() {
	if (SQLITE_OK != (err_ = sqlite3_initialize())) {
		LOGDBI("Failed to initialize library.");
	}
	return SQLITE_OK == err_;
}

bool AndroidDatabase::openConnectionToDB() {
	if (SQLITE_OK != (err_ = sqlite3_open_v2("/data/user/0/lt.smworks.smengine/files/database.db", &db_, DB_OPEN_RWC, NULL))) {
		LOGDBI("Failed to open connection.");
    }
	return SQLITE_OK == err_;
}

AndroidDatabase::~AndroidDatabase() {
	LOGD("Deleted Windows database.");
	if (db_ != 0) sqlite3_close(db_);
	db_ = 0;
	sqlite3_shutdown();
}

AndroidDatabase::ResultSet AndroidDatabase::execute(string query) {
	LOGD("Executing query: %s.", query.c_str());
	sqlite3_stmt* stmt = createStatement(query + ";");
	ResultSet rs;
	while (executeStep(stmt)) {
		rs.vec.push_back(getData(stmt));
	}
	destroyStatement(stmt);
	return rs;
}

bool AndroidDatabase::executeStep(sqlite3_stmt* stmt) {
	err_ = sqlite3_step(stmt);
	return SQLITE_ROW == err_;
}

unordered_map<string, string> AndroidDatabase::getData(sqlite3_stmt* stmt) {
	unordered_map<string, string> um;
	int count = sqlite3_column_count(stmt);
	for (int i = 0; i < count; i++) {
		string key = sqlite3_column_name(stmt, i);
		string value(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
		um.insert(pair<string, string>(key, value));
	}
	return um;
}

sqlite3_stmt* AndroidDatabase::createStatement(string query) {
	sqlite3_stmt* s = 0;
	if (SQLITE_OK != (err_ = sqlite3_prepare_v2(db_, query.c_str(), static_cast<int>(query.size()), &s, NULL))) {
		LOGDBI("Failed to compile query: \"%s\".", query.c_str());
	}
	return s;
}

void AndroidDatabase::destroyStatement(sqlite3_stmt* stmt) {
	if (stmt != 0 && SQLITE_OK != (err_ = sqlite3_finalize(stmt))) {
		LOGDBI("Destroying statement.");
	}
}
