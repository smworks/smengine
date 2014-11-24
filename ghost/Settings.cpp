/*
 * Settings.cpp
 *
 *  Created on: 2013.02.19
 *      Author: Martynas Šustavičius
 */

#include "Settings.h"
#include "Multiplatform/FileManager.h"
#include "Xml.h"
#include "Utils.h"

const string Settings::CONFIG_FILE = "level.cfg";
const string Settings::CONFIG = "config";
const string Settings::NAME = "name";
const string Settings::VALUE = "value";
const string Settings::FRAME_DURATION = "frame_duration";
const string Settings::FIELD_OF_VIEW = "field_of_view";
const string Settings::NEAR_PLANE_DISTANCE = "near_plane_distance";
const string Settings::FAR_PLANE_DISTANCE = "far_plane_distance";
const string Settings::LOADING_SCREEN_SHADER = "loading_shader";
const string Settings::IMMEDIATE_MODE_SHADER = "immediate_mode_shader";
const string Settings::POST_PROCESS_SHADER = "postprocess";
const string Settings::DEFAULT_MODEL_SHADER = "default_model_shader";
const string Settings::DEFAULT_SPRITE_SHADER = "default_sprite_shader";
const string Settings::DEFAULT_TEXT_SHADER = "default_text_shader";

Settings::Settings(FileManager* fileManager, const string& name) :
	fileManager_(fileManager),
	scene_(""),
	sceneLoaded_(false)
{
	fromFile(name.length() > 0 ? name : CONFIG_FILE);
	LOGD("Created settings.");
}

Settings::~Settings() {
	LOGD("Deleted settings.");
}

void Settings::fromFile(const string& file) {
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

string& Settings::getString(const string& name) {
	static unordered_map<string, string>::iterator it;
	it = settingsString_.find(name);
	if (it != settingsString_.end()) {
		return it->second;
	}
	static string ret = "";
	return ret;
}

void Settings::setString(string name, string value) {
	settingsString_.insert(pair<string, string>(name, value));
}

int Settings::getInt(const string& name) {
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

float Settings::getFloat(const string& name) {
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

bool Settings::getBool(const string& name) {
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
