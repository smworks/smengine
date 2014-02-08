/*
 * ScenarioManager.h
 *
 *  Created on: 2014.01.28
 *      Author: Martynas Šustavičius
 */

#ifndef SCENARIOMANAGER_H_
#define SCENARIOMANAGER_H_

#include "Multiplatform/Ghost.h"

class ServiceLocator;
class Scenario;

class ScenarioManager {
public:
	/**
	 * @param system - pointer to service locator.
	 */
	ScenarioManager(ServiceLocator* services);
	~ScenarioManager();

	/**
	 * Executes all scenarios.
	 * @param delta - time delta in miliseconds.
	 */
	void update(double delta);

	/**
	 * Adds scenario to the scenario array.
	 * @param scenario - pointer to scenario object.
	 */
	void execute(Scenario* scenario);
private:
	ServiceLocator* services_;
	vector<Scenario*> scenarios_;
	vector<Scenario*>::iterator it_;
};

#endif