#include "Database.h"
#include "FileManager.h"
#include "../Utils.h"

const string Database::START_SCRIPT = "start_script";
const string Database::LOAD_SECOND_SCRIPT = "load_second_script";
const string Database::ROOT_NODE = "root_node";
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

Database::Database()
{
	LOGD("Created database.");
}

string Database::getString(string key) {
	auto cachedValue = cache.find(key);
	if (cachedValue != cache.end()) {
		return cachedValue->second;
	}

	ResultSet rs = execute("SELECT value FROM settings WHERE key = '" + key + "'");
	if (rs.vec.size() == 1) {
		auto map = rs.vec[0];
		auto value = map.find("value");
		if (value != map.end()) {
			cache.insert(pair<string, string>(key, value->second));
			return value->second;
		}
	}
	return "0";
}

void Database::setString(string key, string value) {
	cache.erase(key);
	execute("INSERT OR REPLACE INTO settings (key, value) VALUES ('" + key + "', '" + value + "')");
}

int Database::getInt(string key) {
	return toInt(getString(key).c_str());
}

float Database::getFloat(string key) {
	return toFloat(getString(key).c_str());
}

bool Database::getBool(string key) {
	return getString(key) == "1";
}

void Database::prepare() {
	ResultSet rs = execute("SELECT name FROM sqlite_master WHERE type='table' AND name='settings'");
	if (rs.vec.size() == 0) { // First time use of DB
		execute("CREATE TABLE settings (key TEXT UNIQUE, value TEXT)");
		execute("INSERT INTO settings (key, value) VALUES "
			"('" + START_SCRIPT + "', 'main.lua'),"
			"('" + LOAD_SECOND_SCRIPT + "', ''),"
			"('" + ROOT_NODE + "', 'root'),"
			"('" + FRAME_DURATION + "', '16f'),"
			"('" + FIELD_OF_VIEW + "', '30.0f'),"
			"('" + NEAR_PLANE_DISTANCE + "', '0.1f'),"
			"('" + FAR_PLANE_DISTANCE + "', '500.0f'),"
			"('" + LOADING_SCREEN_SHADER + "', 'loading'),"
			"('" + IMMEDIATE_MODE_SHADER + "', 'openglimmediate'),"
			"('" + POST_PROCESS_SHADER + "', 'postprocess'),"
			"('" + DEFAULT_MODEL_SHADER + "', 'default_model'),"
			"('" + DEFAULT_SPRITE_SHADER + "', 'default_sprite'),"
			"('" + DEFAULT_TEXT_SHADER + "', 'font')"
		);
	}
}