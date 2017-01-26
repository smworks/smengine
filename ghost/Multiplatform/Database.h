#ifndef DATABASE_H_
#define DATABASE_H_

#include "Ghost.h"

class FileManager;

class Database {
public:
	struct ResultSet {
		vector<unordered_map<string, string>> vec;
	};

	static const string START_SCRIPT;
	static const string LOAD_SECOND_SCRIPT;
	static const string ROOT_NODE;
	static const string FRAME_DURATION;
	static const string FIELD_OF_VIEW;
	static const string NEAR_PLANE_DISTANCE;
	static const string FAR_PLANE_DISTANCE;
	static const string LOADING_SCREEN_SHADER;
	static const string IMMEDIATE_MODE_SHADER;
	static const string POST_PROCESS_SHADER;
	static const string DEFAULT_MODEL_SHADER;
	static const string DEFAULT_SPRITE_SHADER;
	static const string DEFAULT_TEXT_SHADER;
public:

	Database();
	virtual ~Database() {}

	/**
	 * Executes specified query and returns result.
	 * @param query - SQL query.
	 * @return ResultSet containing vector that contains hash map with key-value
	 * pairs representing row from table.
	 */
	virtual ResultSet execute(string query) = 0;

	string getString(string key);
	void setString(string key, string value);
	int getInt(string key);
	float getFloat(string key);
	bool getBool(string key);
protected:
	void prepare();
private:
	unordered_map<string, string> cache;
};

#endif
