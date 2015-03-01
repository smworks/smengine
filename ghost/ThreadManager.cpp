/*
 * ThreadManager.cpp
 *
 *  Created on: 2013.04.14
 *      Author: Martynas Ðustavièius
 */

#include "ThreadManager.h"
#include "Multiplatform/ServiceLocator.h"
#include "Thread.h"

ThreadManager::ThreadManager(ServiceLocator* services) :
	services_(services),
	destroy_(false)
{
	LOGD("Created thread manager.");
}

ThreadManager::~ThreadManager() {
	destroy_ = true;
	PROFILE("Joining all threads. Thread count: %u.", (UINT32) threads_.size());
	joinAll();
	PROFILE("All threads joined.");
	LOGD("Deleted thread manager.");
}

void ThreadManager::update() {
    for (SIZE i = 0; i < threads_.size(); i++) {
		if (!threads_[i]->isRunning()) {
			delete threads_[i];
			threads_.erase(threads_.begin() + i);
		}
	}
}

UINT64 ThreadManager::execute(Task* task) {
	if (destroy_) {
		delete task;
		return 0;
	}
	Thread* thread = NEW Thread(task);
	threads_.push_back(thread);
	return thread->getId();
}

void ThreadManager::join(UINT64 id) {
	for (UINT32 i = 0; i < threads_.size(); i++) {
		if (threads_[i]->getId() == id) {
			if (id != services_->getCurrentThreadId()) {
				delete threads_[i];
				threads_.erase(threads_.begin() + i);
			} else {
				LOGW("Thread with id %llu attempted to join itself.", id);
			}
			return;
		}
	}
}

void ThreadManager::joinAll() {
	for (Thread* t : threads_)
		delete t;
	threads_.clear();
}