/*
 * Thread.h
 *
 *  Created on: 2013.01.29
 *      Author: Martynas Šustavičius
 */

#ifndef THREAD_H_
#define THREAD_H_

#include "Ghost.h"

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
	virtual ~Thread() {};

	/**
	 * @return Unique thread id.
	 */
	virtual UINT64 getId() = 0;

	/**
	 * Starts running thread.
	 * @param task - task to be executed.
	 */
	virtual void start(Task* task) = 0;

	/**
	 * Indicates whether thread is running.
	 * Method is thread safe.
	 * @return True if thread is alive.
	 */
	virtual bool isRunning() = 0;
};

#endif