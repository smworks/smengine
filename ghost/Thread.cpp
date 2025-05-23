﻿/*
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
	delete task;
	delete workerThread;
}

UINT64 Thread::getId() const
{
	return workerThread == nullptr ? 0 : hash<thread::id>()(workerThread->get_id());
}

bool Thread::isRunning() {
	bool ret;
	workerMutex.lock();
	ret = running;
	workerMutex.unlock();
	return ret;
}

void Thread::run() {
	LOGD("Created thread with id: %lu.", hash<thread::id>()(this_thread::get_id()));
	task->run();
	workerMutex.lock();
	running = false;
	workerMutex.unlock();
}
