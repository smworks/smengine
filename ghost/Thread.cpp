/*
 * Thread.cpp
 *
 *  Created on: 2015.03.01
 *      Author: Martynas Šustavičius
 */

#include "Thread.h"

Thread::Thread(Task* task) : running(true), task(task) {
	ASSERT(task != 0, "Task specified for thread is null.");
	workerThread = new thread(&Thread::run, this);
}

Thread::~Thread() {
	workerThread->join();
	delete workerThread;
	task->finish();
	delete task;
}

UINT64 Thread::getId() {
	return workerThread == 0 ? 0 : workerThread->get_id().hash();
}

bool Thread::isRunning() {
	bool ret;
	workerMutex.lock();
	ret = running;
	workerMutex.unlock();
	return ret;
}

void Thread::run() {
	task->run();
	workerMutex.lock();
	running = false;
	workerMutex.unlock();
}