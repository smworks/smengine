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

	/**
	 * Set integer attribute value.
	 * @param name - name of the attribute.
	 * @param value - value of the attribute.
	 */
	void setInt(const string& name, INT32 val);

	/**
	 * Get integer attribute value.
	 * @param name - name of the attribute.
	 * @param defValue - default value to return
	 * if attribute with specified name not found.
	 * @return Value of the attribute.
	 */
	INT32 getInt(const string& name, INT32 defValue = 0);

	/**
	 * Set long attribute value.
	 * @param name - name of the attribute.
	 * @param value - value of the attribute.
	 */
	void setLong(const string& name, INT64 val);

	/**
	 * Get long attribute value.
	 * @param name - name of the attribute.
	 * @param defValue - default value to return
	 * if attribute with specified name not found.
	 * @return Value of the attribute.
	 */
	INT64 getLong(const string& name, INT64 defValue = 0);

	/**
	 * Set float attribute value.
	 * @param name - name of the attribute.
	 * @param value - value of the attribute.
	 */
	void setFloat(const string& name, float val);

	/**
	 * Get float attribute value.
	 * @param name - name of the attribute.
	 * @param defValue - default value to return
	 * if attribute with specified name not found.
	 * @return Value of the attribute.
	 */
	float getFloat(const string& name, float defValue = 0.0f);

	/**
	 * Set string attribute value.
	 * @param name - name of the attribute.
	 * @param value - value of the attribute.
	 */
	void setString(const string& name, string val);

	/**
	 * Get string attribute value.
	 * @param name - name of the attribute.
	 * @param defValue - default value to return
	 * if attribute with specified name not found.
	 * @return Value of the attribute.
	 */
	string getString(const string& name, string defValue = "");

	/**
	 * Set pointer attribute value.
	 * @param name - name of the attribute.
	 * @param value - value of the attribute.
	 */
	void setPointer(const string& name, void* val);

	/**
	 * Get pointer attribute value.
	 * @param name - name of the attribute.
	 * @return Pointer to object or zero.
	 */
	void* getPointer(const string& name);
private:
	unordered_map<string, INT32> mapInt_;
	unordered_map<string, INT32>::iterator mapIntIt_;
	unordered_map<string, INT64> mapLong_;
	unordered_map<string, INT64>::iterator mapLongIt_;
	unordered_map<string, float> mapFloat_;
	unordered_map<string, float>::iterator mapFloatIt_;
	unordered_map<string, string> mapString_;
	unordered_map<string, string>::iterator mapStringIt_;
	unordered_map<string, void*> mapPointer_;
	unordered_map<string, void*>::iterator mapPointerIt_;
};

#endif