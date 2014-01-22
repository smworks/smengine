/*
 * MacOSXThread.cpp
 *
 *  Created on: 2014.01.09
 *      Author: Martynas Šustavičius
 */

#include "Thread.h"

MacOSXThread::MacOSXThread() :
    mutex_(PTHREAD_MUTEX_INITIALIZER),
	running_(false),
	task_(0)
{
	if (pthread_mutex_init(&mutex_, 0)) {
		LOGE("Unable to create mutex.");
		return;
	}
}

MacOSXThread::~MacOSXThread() {
    bool running;
	while (true) {
	    if (pthread_mutex_lock(&mutex_)) {
			LOGW("Unable to lock mutex.");
		}
		running = running_;
		if (pthread_mutex_unlock(&mutex_)) {
			LOGW("Unable to release mutex.");
		}
		if (!running) {
			break;
		}
		usleep(1000);
	}
	if (task_ != 0) {
		task_->finish();
		delete task_;
		task_ = 0;
	}
	pthread_join(handle_, 0);
	LOGD("Deleted thread with id: %ld", getId());
}

UINT64 MacOSXThread::getId() {
	return reinterpret_cast<UINT64>(handle_);
}

void MacOSXThread::start(Task* task) {
	if (task == 0) {
		LOGW("No task specified for thread to run.");
		return;
	}
    if (pthread_mutex_lock(&mutex_)) {
        LOGW("Unable to lock mutex.");
    }
	running_ = true;
    if (pthread_mutex_unlock(&mutex_)) {
        LOGW("Unable to release mutex.");
    }
	task_ = task;
	if (pthread_create(&handle_, 0, &MacOSXThread::staticRun, (void*) this)) {
		LOGE("Unable to create thread.");
		return;
	}
    LOGD("Starting thread with id: %ld", getId());
}

bool MacOSXThread::isRunning() {
	bool ret;
    if (pthread_mutex_lock(&mutex_)) {
        LOGW("Unable to lock mutex.");
    }
	ret = running_;
    if (pthread_mutex_unlock(&mutex_)) {
        LOGW("Unable to release mutex.");
    }
	return ret;
}

void* MacOSXThread::staticRun(void* thread) {
	return (void*) (UINT64) ((MacOSXThread*) thread)->run();
}

int MacOSXThread::run() {
	task_->run();
    if (pthread_mutex_lock(&mutex_)) {
        LOGW("Unable to lock mutex.");
    }
	running_ = false;
    if (pthread_mutex_unlock(&mutex_)) {
        LOGW("Unable to release mutex.");
    }
	return 1;
}
