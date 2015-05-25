/*
 * Attributes.h
 *
 *  Created on: 2013.10.26
 *      Author: Martynas Šustavičius
 */

#ifndef ATTRIBUTES_H_
#define ATTRIBUTES_H_

#include "../Multiplatform/Ghost.h"

class Attributes {
public:
	Attributes();
	~Attributes();

	void setBool(const string& name, bool val);
	bool getBool(const string& name, bool defValue = false);
	void setInt(const string& name, INT32 val);
	INT32 getInt(const string& name, INT32 defValue = 0);
	void setFloat(const string& name, float val);
	float getFloat(const string& name, float defValue = 0.0f);
	void setString(const string& name, string val);
	string getString(const string& name, string defValue = "");
	void setPointer(const string& name, void* val);
	void* getPointer(const string& name);
private:
	unordered_map<string, bool> mapBool;
	unordered_map<string, bool>::iterator mapBoolIt;
	unordered_map<string, INT32> mapInt;
	unordered_map<string, INT32>::iterator mapIntIt;
	unordered_map<string, float> mapFloat;
	unordered_map<string, float>::iterator mapFloatIt;
	unordered_map<string, string> mapString;
	unordered_map<string, string>::iterator mapStringIt;
	unordered_map<string, void*> mapPointer;
	unordered_map<string, void*>::iterator mapPointerIt;
};

#endif