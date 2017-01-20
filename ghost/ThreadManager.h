/*
 * ThreadManager.h
 *
 *  Created on: 2013.04.14
 *      Author: Martynas Šustavičius
 */

#ifndef THREADMANAGER_H_
#define THREADMANAGER_H_

#include "Multiplatform/Ghost.h"

class Task;
class ServiceLocator;
class Thread;

class ThreadManager {
public:
	/**
	 * @param system - pointer to service locator.
	 */
	explicit ThreadManager(ServiceLocator* services);
	~ThreadManager();

	/**
	 * Deletes threads that finished their tasks and executes
	 * callbacks if any were set.
	 */
	void update();

	/**
	 * Finds a thread for execution and runs the task.
	 * @param task - task to be executed.
	 * @return Index of thread that executes task.
	 */
	UINT64 execute(Task* task);

	/**
	 * Waits for thread with specified id to finish.
	 */
	void join(UINT64 id);

	/**
	 * Waits for all threads to finish.
	 */
	void joinAll();
private:
	ServiceLocator* services;
	vector<Thread*> threads;
	bool destroy;
};

#endif