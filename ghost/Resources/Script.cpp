/*
 * Script.cpp
 *
 *  Created on: 2013.02.26
 *      Author: Martynas Šustavičius
 */


#include "Script.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Multiplatform/FileManager.h"

Script::Script(ServiceLocator* services) :
	Resource(services),
	id_(-1),
	script_("")
{}

Script::~Script() {}

void Script::release() {
	id_ = -1;
	script_ = "";
}

SIZE Script::getSize() {
	return sizeof(Script);
}

Resource::Type Script::getType() {
	return SCRIPT;
}

bool Script::isValid() {
	return script_.length() > 0 && id_ != -1;
}

void Script::setScript(string& script) {
	script_ = script;
}

string& Script::getScript() {
	return script_;
}

void Script::setId(int id) {
	id_ = id;
}

int Script::getId() {
	return id_;
}

void Script::reset() {
	release();
	create();
}

bool Script::create() {
	string sc = GHOST_SCRIPTS + getAttribute(ATTR_FILE);
	script_ = getServiceLocator()->getFileManager()->loadText(sc);
	return script_.length() > 0;
}
