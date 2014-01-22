/*
 * Attributes.cpp
 *
 *  Created on: 2013.10.26
 *      Author: Martynas Šustavičius
 */


#include "Attributes.h"

Attributes::Attributes() {}

Attributes::~Attributes() {}

void Attributes::setInt(const string& name, INT32 val) {
	mapIntIt_ = mapInt_.find(name);
	if (mapInt_.end() != mapIntIt_) {
		mapIntIt_->second = val;
		return;
	}
	mapInt_.insert(pair<string, INT32>(name, val));
}

INT32 Attributes::getInt(const string& name, INT32 val) {
	mapIntIt_ = mapInt_.find(name);
	if (mapInt_.end() != mapIntIt_) {
		return mapIntIt_->second;
	}
	return val;
}

void Attributes::setLong(const string& name, INT64 val) {
	mapLongIt_ = mapLong_.find(name);
	if (mapLong_.end() != mapLongIt_) {
		mapLongIt_->second = val;
		return;
	}
	mapInt_.insert(pair<string, INT64>(name, val));
}

INT64 Attributes::getLong(const string& name, INT64 val) {
	mapLongIt_ = mapLong_.find(name);
	if (mapLong_.end() != mapLongIt_) {
		return mapLongIt_->second;
	}
	return val;
}

void Attributes::setFloat(const string& name, float val) {
	mapFloatIt_ = mapFloat_.find(name);
	if (mapFloat_.end() != mapFloatIt_) {
		mapFloatIt_->second = val;
		return;
	}
	mapFloat_.insert(pair<string, float>(name, val));
}

float Attributes::getFloat(const string& name, float val) {
	mapFloatIt_ = mapFloat_.find(name);
	if (mapFloat_.end() != mapFloatIt_) {
		return mapFloatIt_->second;
	}
	return val;
}

void Attributes::setString(const string& name, string val) {
	mapStringIt_ = mapString_.find(name);
	if (mapString_.end() != mapStringIt_) {
		mapStringIt_->second = val;
		return;
	}
	mapString_.insert(pair<string, string>(name, val));
}

string Attributes::getString(const string& name, string val) {
	mapStringIt_ = mapString_.find(name);
	if (mapString_.end() != mapStringIt_) {
		return mapStringIt_->second;
	}
	return val;
}

void Attributes::setPointer(const string& name, void* val) {
	mapPointerIt_ = mapPointer_.find(name);
	if (mapPointer_.end() != mapPointerIt_) {
		mapPointerIt_->second = val;
		return;
	}
	mapPointer_.insert(pair<string, void*>(name, val));
}

void* Attributes::getPointer(const string& name) {
	mapPointerIt_ = mapPointer_.find(name);
	if (mapPointer_.end() != mapPointerIt_) {
		return mapPointerIt_->second;
	}
	return 0;
}