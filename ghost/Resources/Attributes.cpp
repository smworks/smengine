/*
 * Attributes.cpp
 *
 *  Created on: 2013.10.26
 *      Author: Martynas Šustavičius
 */


#include "Attributes.h"

Attributes::Attributes() {}
Attributes::~Attributes() {}

void Attributes::setBool(const string& name, bool val) {
	mapBoolIt = mapBool.find(name);
	if (mapBool.end() != mapBoolIt) {
		mapBoolIt->second = val;
		return;
	}
	mapBool.insert(pair<string, bool>(name, val));
}

bool Attributes::getBool(const string& name, bool val) {
	mapBoolIt = mapBool.find(name);
	if (mapBool.end() != mapBoolIt) {
		return mapBoolIt->second;
	}
	return val;
}

void Attributes::setInt(const string& name, INT32 val) {
	mapIntIt = mapInt.find(name);
	if (mapInt.end() != mapIntIt) {
		mapIntIt->second = val;
		return;
	}
	mapInt.insert(pair<string, INT32>(name, val));
}

INT32 Attributes::getInt(const string& name, INT32 val) {
	mapIntIt = mapInt.find(name);
	if (mapInt.end() != mapIntIt) {
		return mapIntIt->second;
	}
	return val;
}

void Attributes::setFloat(const string& name, float val) {
	mapFloatIt = mapFloat.find(name);
	if (mapFloat.end() != mapFloatIt) {
		mapFloatIt->second = val;
		return;
	}
	mapFloat.insert(pair<string, float>(name, val));
}

float Attributes::getFloat(const string& name, float val) {
	mapFloatIt = mapFloat.find(name);
	if (mapFloat.end() != mapFloatIt) {
		return mapFloatIt->second;
	}
	return val;
}

void Attributes::setString(const string& name, string val) {
	mapStringIt = mapString.find(name);
	if (mapString.end() != mapStringIt) {
		mapStringIt->second = val;
		return;
	}
	mapString.insert(pair<string, string>(name, val));
}

string Attributes::getString(const string& name, string val) {
	mapStringIt = mapString.find(name);
	if (mapString.end() != mapStringIt) {
		return mapStringIt->second;
	}
	return val;
}

void Attributes::setPointer(const string& name, void* val) {
	mapPointerIt = mapPointer.find(name);
	if (mapPointer.end() != mapPointerIt) {
		mapPointerIt->second = val;
		return;
	}
	mapPointer.insert(pair<string, void*>(name, val));
}

void* Attributes::getPointer(const string& name) {
	mapPointerIt = mapPointer.find(name);
	if (mapPointer.end() != mapPointerIt) {
		return mapPointerIt->second;
	}
	return 0;
}