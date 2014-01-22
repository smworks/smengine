/*
 * MacOSXThread.h
 *
 *  Created on: 2014.01.09
 *      Author: Martynas Šustavičius
 */

#ifndef MACOSXTHREAD_H_
#define MACOSXTHREAD_H_

#include "../Ghost.h"
#include "../ServiceLocator.h"
#include "../Thread.h"

class MacOSXThread : public Thread {
public:
	MacOSXThread();
	~MacOSXThread();

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
