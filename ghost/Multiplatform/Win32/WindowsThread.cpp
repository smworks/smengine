/*
 * WindowsThread.cpp
 *
 *  Created on: 2013.01.29
 *      Author: Martynas Šustavičius
 */

#include "WindowsThread.h"


WindowsThread::WindowsThread() :
	running_(false),
	task_(0)
{
	mutex_ = CreateMutex(0, FALSE, 0);
	if (mutex_ == 0) {
		LOGE("Unable to create mutex.");
		return;
	}
	unsigned long id;
	handle_ = CreateThread(0, 0,
		(unsigned long (__stdcall *)(void *)) this->staticRun,
		(void*) this, CREATE_SUSPENDED, &id);
	id_ = id;
	if (handle_ == 0) {
		LOGE("Unable to create thread.");
		return;
	}
	if (!SetThreadPriority(handle_, THREAD_PRIORITY_NORMAL)) {
		LOGE("Unable to set thread priority for thread.");
		return;
	}
}

WindowsThread::~WindowsThread() {
	bool running;
	while (true) {
		if (WaitForSingleObject(mutex_, INFINITE) == WAIT_ABANDONED) {
			LOGW("Unable to lock mutex.");
		}
		running = running_;
		if (!ReleaseMutex(mutex_)) {
			LOGW("Unable to release mutex.");
		}
		if (!running) {
			break;
		}
		Sleep(1);
	}
	if (task_ != 0) {
		task_->finish();
		delete task_;
		task_ = 0;
	}
	WaitForSingleObject(handle_, INFINITE);
	CloseHandle(handle_);
	CloseHandle(mutex_);
	LOGD("Deleted thread with id: %d", id_);
}

UINT64 WindowsThread::getId() {
	return id_;
}

void WindowsThread::start(Task* task) {
	LOGD("Starting thread with id: %d", id_);
	if (task == 0) {
		LOGW("No task specified for thread to run.");
		return;
	}
	if (WaitForSingleObject(mutex_, INFINITE) == WAIT_ABANDONED) {
		LOGW("Unable to lock mutex.");
	}
	running_ = true;
	if (!ReleaseMutex(mutex_)) {
		LOGW("Unable to release mutex.");
	}
	task_ = task;
	if (!ResumeThread(handle_)) {
		LOGW("Unable to resume thread.");
	}
}

bool WindowsThread::isRunning() {
	bool ret;
	if (WaitForSingleObject(mutex_, INFINITE) == WAIT_ABANDONED) {
		LOGW("Unable to lock mutex.");
	}
	ret = running_;
	if (!ReleaseMutex(mutex_)) {
		LOGW("Unable to release mutex.");
	}
	return ret;
}

int WindowsThread::staticRun(void* thread) {
	return ((WindowsThread*) thread)->run();
}

int WindowsThread::run() {
	task_->run();
	if (WaitForSingleObject(mutex_, INFINITE) == WAIT_ABANDONED) {
		LOGW("Unable to lock mutex.");
	}
	running_ = false;
	if (!ReleaseMutex(mutex_)) {
		LOGW("Unable to release mutex.");
	}
	return 1;
}