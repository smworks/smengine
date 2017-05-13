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

	void release() override;
	SIZE getSize() override;
	Type getType() override;
	bool isValid() override;
	void setScript(string& script);
	string& getScript();
	void setId(int id);
	int getId();
	void reset();
protected:
	bool create() override;
	
	int id_;
	string script_;
};

#endif