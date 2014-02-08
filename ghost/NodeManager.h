/*
 * NodeManager.h
 *
 *  Created on: 2014.01.28
 *      Author: Martynas Šustavičius
 */

#ifndef NODEMANAGER_H_
#define NODEMANAGER_H_

#include "Multiplatform/Ghost.h"

class ServiceLocator;

class NodeManager {
public:
	/**
	 * @param system - pointer to service locator.
	 */
	NodeManager(ServiceLocator* services);
	~NodeManager();
private:
	ServiceLocator* services_;
};

#endif