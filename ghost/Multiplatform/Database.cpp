/*
 * Database.cpp
 *
 *  Created on: 2013.02.19
 *      Author: Martynas Šustavičius
 */

#include "Database.h"
#include "FileManager.h"
#include "../Xml.h"
#include "../Utils.h"

#include "../../dependencies/includes/sqlite/sqlite3.h"

const string Database::CONFIG_FILE = "level.cfg";
const string Database::CONFIG = "config";
const string Database::NAME = "name";
const string Database::VALUE = "value";
const string Database::FRAME_DURATION = "frame_duration";
const string Database::FIELD_OF_VIEW = "field_of_view";
const string Database::NEAR_PLANE_DISTANCE = "near_plane_distance";
const string Database::FAR_PLANE_DISTANCE = "far_plane_distance";
const string Database::LOADING_SCREEN_SHADER = "loading_shader";
const string Database::IMMEDIATE_MODE_SHADER = "immediate_mode_shader";
const string Database::POST_PROCESS_SHADER = "postprocess";
const string Database::DEFAULT_MODEL_SHADER = "default_model_shader";
const string Database::DEFAULT_SPRITE_SHADER = "default_sprite_shader";
const string Database::DEFAULT_TEXT_SHADER = "default_text_shader";

Database::Database(FileManager* fileManager, const string& name) :
	fileManager_(fileManager),
	scene_(""),
	sceneLoaded_(false)
{
	fromFile(name.length() > 0 ? name : CONFIG_FILE);
	LOGD("Created settings.");

	sqlite3* pDb = NULL;
    sqlite3_stmt* query = NULL;
    int ret = 0;
    do // avoid nested if's
    {
        // initialize engine
        if (SQLITE_OK != (ret = sqlite3_initialize()))
        {
            LOGI("Failed to initialize library: %d\n", ret);
            break;
        }
        // open connection to a DB
        if (SQLITE_OK != (ret = sqlite3_open_v2("test.db", &pDb, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL)))
        {
            LOGI("Failed to open conn: %d\n", ret);
            break;
        }
        // prepare the statement
        if (SQLITE_OK != (ret = sqlite3_prepare_v2(pDb, "SELECT 2012", -1, &query, NULL)))
        {
            LOGI("Failed to prepare insert: %d, %s\n", ret, sqlite3_errmsg(pDb));
            break;
        }
        // step to 1st row of data
        if (SQLITE_ROW != (ret = sqlite3_step(query))) // see documentation, this can return more values as success
        {
            LOGI("Failed to step: %d, %s\n", ret, sqlite3_errmsg(pDb));
            break;
        }
        // ... and print the value of column 0 (expect 2012 here)
        LOGI("Value from sqlite: %s", sqlite3_column_text(query, 0));     

    } while (false);
    // cleanup
    if (NULL != query) sqlite3_finalize(query);
    if (NULL != pDb) sqlite3_close(pDb);
    sqlite3_shutdown();

}

Database::~Database() {
	LOGD("Deleted settings.");
}

void Database::fromFile(const string& file) {
	string data;
	fileManager_->loadText(data, (GHOST_SCENES + file).c_str());
	if (data.length() == 0) {
		LOGW("Configuration file not found.");
		return;
	}
	settingsString_.clear();
	settingsInt_.clear();
	settingsFloat_.clear();
	Xml xml(data);
	XmlNode* root = xml.getRootNode();
	vector<XmlNode*> configs = root->getNodes(CONFIG);
	vector<XmlNode*>::const_iterator it;
	it = configs.begin();
	while (it != configs.end()) {
		XmlNode* node = *it;
		string* name = node->getAttr(NAME);
		string* val = node->getAttr(VALUE);
		settingsString_.insert(
			pair<string, string>(*name, *val));
		it++;
	}
}

string& Database::getString(const string& name) {
	static unordered_map<string, string>::iterator it;
	it = settingsString_.find(name);
	if (it != settingsString_.end()) {
		return it->second;
	}
	static string ret = "";
	return ret;
}

void Database::setString(string name, string value) {
	settingsString_.insert(pair<string, string>(name, value));
}

int Database::getInt(const string& name) {
	static unordered_map<string, int>::iterator it;
	it = settingsInt_.find(name);
	if (settingsInt_.end() != it) {
		return it->second;
	}
	static unordered_map<string, string>::iterator its;
	its = settingsString_.find(name);
	if (its == settingsString_.end()) {
		return -1;
	}
	int val = toInt(its->second.c_str());
	settingsInt_.insert(
		pair<string, int>(name, val));
	return val;
}

float Database::getFloat(const string& name) {
	static unordered_map<string, float>::iterator it;
	it = settingsFloat_.find(name);
	if (settingsFloat_.end() != it) {
		return it->second;
	}
	static unordered_map<string, string>::iterator its;
	its = settingsString_.find(name);
	if (its == settingsString_.end()) {
		return 0.0f;
	}
	float val = toFloat(its->second.c_str());
	settingsFloat_.insert(
		pair<string, float>(name, val));
	return val;
}

bool Database::getBool(const string& name) {
	static unordered_map<string, bool>::iterator it;
	it = settingsBool_.find(name);
	if (settingsBool_.end() != it) {
		return it->second;
	}
	static unordered_map<string, string>::iterator its;
	its = settingsString_.find(name);
	if (its == settingsString_.end()) {
		return false;
	}
	bool val = toBool(its->second);
	settingsBool_.insert(
		pair<string, bool>(name, val));
	return val;
}
