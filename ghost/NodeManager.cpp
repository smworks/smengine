/*
 * ScenarioManager.cpp
 *
 *  Created on: 2014.01.28
 *      Author: Martynas Ðustavièius
 */

#include "NodeManager.h"
#include "Multiplatform/ServiceLocator.h"

NodeManager::NodeManager(ServiceLocator* services) :
	services_(services)
{
	LOGD("Created node manager.");
}

NodeManager::~NodeManager() {
	LOGD("Deleted node manager.");
}