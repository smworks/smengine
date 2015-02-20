/*
 * Database.h
 *
 *  Created on: 2012.10.30
 *      Author: Martynas Šustavičius
 */

#ifndef DATABASE_H_
#define DATABASE_H_

#include "Ghost.h"

class FileManager;

class Database {
public:
	struct ResultSet {
		vector<unordered_map<string, string>> vec;
	};
public:
	static const string CONFIG_FILE;
	static const string CONFIG;
	static const string NAME;
	static const string VALUE;
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
	/**
	 * @param fileManager - file manager object.
	 * @param name - name of file containing configuration.
	 */
	Database(FileManager* fileManager, const string& name = "");
	virtual ~Database() {}

	/**
	 * Executes specified query and returns result.
	 * @param query - SQL query.
	 * @return ResultSet containing vector that contains hash map with key-value
	 * pairs representing row from table.
	 */
	virtual ResultSet execute(string query) = 0;

	/**
	 * @param name - parameter name.
	 * @return Value that belongs to specified parameter.
	 * If no parameter was found, return "".
	 */
	string& getString(const string& name);

	/**
	 * Sets string parameter.
	 * @param name - name of the string.
	 * @param value - string value.
	 */
	void setString(string name, string value);

	/**
	 * @param name - parameter name.
	 * @return Value that belongs to specified parameter.
	 * If no parameter was found, return -1.
	 */
	int getInt(const string& name);

	/**
	 * @param name - parameter name.
	 * @return Value that belongs to specified parameter.
	 * If no parameter was found, return 0.0f.
	 */
	float getFloat(const string& name);

	/**
	 * @param name - parameter name.
	 * @return Value that belongs to specified parameter.
	 * If no parameter was found, return false.
	 */
	bool getBool(const string& name);

	/**
	 * @param scene - name of scene file.
	 */
	void setScene(const string& scene) {
		scene_ = scene;
		sceneLoaded_ = false;
	}

	/**
	 * @return Name of the scene file.
	 */
	string& getScene() {
		return scene_;
	}

	/**
	 * @return True if scene needs to be loaded.
	 */
	bool isSceneLoaded() {
		return sceneLoaded_;
	}

	/**
	 * @param state - state indicating whether to load
	 * scene, or not.
	 */
	void setSceneLoaded(bool state) {
		sceneLoaded_ = state;
	}
private:
	/**
	 * Loads parameters from file.
	 * @param name - name of configuration file.
	 */
	void fromFile(const string& name);
private:
	FileManager* fileManager_;
	unordered_map<string, string> settingsString_;
	unordered_map<string, int> settingsInt_;
	unordered_map<string, float> settingsFloat_;
	unordered_map<string, bool> settingsBool_;
	string scene_;
	bool sceneLoaded_;
};

#endif /* DATABASE_H_ */
