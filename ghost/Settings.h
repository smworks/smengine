﻿/*
 * Settings.h
 *
 *  Created on: 2012.10.30
 *      Author: Martynas Šustavičius
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "Multiplatform/Ghost.h"

class FileManager;

class Settings {
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
	Settings(FileManager* fileManager, const string& name = "");
	~Settings();

	/**
	 * Loads parameters from file.
	 * @param name - name of configuration file.
	 */
	void fromFile(const string& name);

	/**
	 * @param name - parameter name.
	 * @return Value that belongs to specified parameter.
	 * If no parameter was found, return "".
	 */
	string& getString(const string& name);

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
	FileManager* fileManager_;
	unordered_map<string, string> settingsString_;
	unordered_map<string, int> settingsInt_;
	unordered_map<string, float> settingsFloat_;
	unordered_map<string, bool> settingsBool_;
	string scene_;
	bool sceneLoaded_;
};

#endif /* SETTINGS_H_ */