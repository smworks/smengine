/*
 * ScenarioManager.cpp
 *
 *  Created on: 2014.01.28
 *      Author: Martynas Šustavičius
 */

#include "ScenarioManager.h"
#include "Multiplatform/ServiceLocator.h"
#include "Resources/Scenario.h"
#include "ResourceManager.h"

ScenarioManager::ScenarioManager(ServiceLocator* services) :
	services_(services)
{
	LOGD("Created scenario manager.");
}

ScenarioManager::~ScenarioManager() {
	LOGD("Deleted scenario manager.");
}

void ScenarioManager::update(double delta) {
	for (it_ = scenarios_.begin(); it_ != scenarios_.end();) {
		Scenario* scenario = (*it_);
		float progress = scenario->getProgress() + (float) delta;
		vector<Scenario::Action*>& actions = scenario->getActions();
		SIZE size = actions.size();
		for (SIZE i = scenario->getIndex(); i < size; i++) {
			if (progress > actions[i]->getDuration()) {
				actions[i]->run(1.0f);
				progress -= actions[i]->getDuration();
				scenario->setIndex(i + 1);
				continue;
			}
			actions[i]->run(progress / actions[i]->getDuration());
			scenario->setProgress(progress);
			break;
		}
		if (scenario->getIndex() == size) {
			scenario->setProgress(0.0f);
			scenario->setIndex(0);
			delete scenario;
			it_ = scenarios_.erase(it_);
		}
		else {
			it_++;
		}
	}
}

void ScenarioManager::execute(Scenario* scenario) {
	scenarios_.push_back(scenario);
}