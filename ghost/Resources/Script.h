/*
 * Script.h
 *
 *  Created on: 2012.07.02
 *      Author: Martynas Šustavičius
 */

#ifndef SCRIPT_H_
#define SCRIPT_H_

#include "../Multiplatform/Ghost.h"
#include "Resource.h"

class Script : public Resource {
public:
	Script(ServiceLocator* services);
	~Script();

	/**
	 * @see Resource
	 */
	void release();

	/**
	 * @see Resource
	 */
	SIZE getSize();

	/**
	 * @see Resource
	 */
	Resource::Type getType();

	/**
	 * @see Resource
	 */
	bool isValid();

	/**
	 * @param script - string containing script.
	 */
	void setScript(string& script);

	/**
	 * @return String containing script.
	 */
	string& getScript();

	/**
	 * @param id - script id.
	 */
	void setId(int id);

	/**
	 * @return Id of the logic.
	 */
	int getId();

	/**
	 * Releases and creates resource again.
	 */
	void reset();
protected:
	/**
	 * @see Resource
	 */
	bool create() override;
	
	int id_;
	string script_;
};

#endif