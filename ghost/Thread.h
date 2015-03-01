/*
 * Thread.h
 *
 *  Created on: 2013.01.29
 *      Author: Martynas Šustavičius
 */

#ifndef THREAD_H_
#define THREAD_H_

#include "Multiplatform/Ghost.h"

class Task {
public:
    virtual ~Task() {}
    
	/**
	 * This method will be executed on separate thread.
	 */
	virtual void run() = 0;

	/**
	 * This method will be executed on main thread.
	 */
	virtual void finish() {}
};

class Thread {
public:
	/**
	 * Executes single task.
	 * @param task - task that will be executed in thread.
	 * @NOTE: task is deleted when thread finishes work.
	 */
	Thread(Task* task);
	~Thread();
	UINT64 getId();
	bool isRunning();
private:
	void run();
private:
	mutex workerMutex;
	thread* workerThread;
	bool running;
	Task* task;
};

#endif