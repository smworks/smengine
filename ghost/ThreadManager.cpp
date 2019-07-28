#include "ThreadManager.h"
#include "Multiplatform/ServiceLocator.h"
#include "Thread.h"

ThreadManager::ThreadManager(ServiceLocator* services) :
	services(services),
	destroy(false)
{
	LOGD("Created thread manager.");
}

ThreadManager::~ThreadManager() {
	destroy = true;
	PROFILE("Joining all threads. Thread count: %u.", static_cast<UINT32>(threads.size()));
	joinAll();
	PROFILE("All threads joined.");
	LOGD("Deleted thread manager.");
}

void ThreadManager::update() {
    for (INT32 i = threads.size() - 1; i >= 0; i--) {
		if (!threads[i]->isRunning()) {
			delete threads[i];
			threads.erase(threads.begin() + i);
		}
	}
}

UINT64 ThreadManager::execute(Task* task) {
	if (destroy) {
		delete task;
		return 0;
	}
	auto thread = NEW Thread(task);
	threads.push_back(thread);
	return thread->getId();
}

void ThreadManager::join(UINT64 id) {
	if (id == hash<thread::id>()(this_thread::get_id())) {
		LOGW("Thread with id %llu attempted to join itself.", id);
		return;
	}
	for (UINT32 i = 0; i < threads.size(); i++) {
		if (threads[i]->getId() == id) {
			delete threads[i];
			threads.erase(threads.begin() + i);
			return;
		}
	}
}

void ThreadManager::joinAll() {
	for (auto t : threads) {
		delete t;
	}
	threads.clear();
}
