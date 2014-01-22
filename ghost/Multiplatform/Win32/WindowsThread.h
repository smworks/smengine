/*
 * WindowsThreadManager.h
 *
 *  Created on: 2013.01.29
 *      Author: Martynas Šustavičius
 */

#ifndef WINDOWSTHREAD_H_
#define WINDOWSTHREAD_H_

#include "../Ghost.h"
#include "../ServiceLocator.h"
#include "../Thread.h"

class WindowsThread : public Thread {
public:
	WindowsThread();
	~WindowsThread();

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
	static int staticRun(void* thread);
	
	/**
	 * Main loop.
	 * @return Thread finish state.
	 */
	int run();
protected:
	UINT64 id_;
	HANDLE handle_;
	HANDLE mutex_;
	bool running_;
	Task* task_;
};

#endif