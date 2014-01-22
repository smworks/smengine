/*
 * LinuxThread.h
 *
 *  Created on: 2013.03.26
 *      Author: Martynas Šustavičius
 */

#ifndef LINUXTHREAD_H_
#define LINUXTHREAD_H_

#include "../Ghost.h"
#include "../ServiceLocator.h"
#include "../Thread.h"

class Task;

class LinuxThread : public Thread {
public:
	LinuxThread();
	~LinuxThread();

	/**
	 * @see Thread
	 */
	UINT64 getId();

	/**
	 * @see Thread
	 */
	void start(Task* task);

	/**
	 * @see Thread
	 */
	bool isRunning();
private:
	/**
	 * Since you can only pass pointers
	 * to static methods. This method
	 * acts as a tunnel to real run method.
	 * @param thread - void pointer to Thread object.
	 */
	static void* staticRun(void* thread);

	/**
	 * Main loop.
	 * @return Thread finish state.
	 */
	int run();
protected:
	pthread_t handle_;
	pthread_mutex_t mutex_;
	bool running_;
	Task* task_;
};

#endif
