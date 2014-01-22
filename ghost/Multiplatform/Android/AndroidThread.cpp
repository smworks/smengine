/*
 * AndroidThread.cpp
 *
 *  Created on: 2014.01.09
 *      Author: Martynas Šustavičius
 */

#include "AndroidThread.h"

AndroidThread::AndroidThread() :
	handle_(0),
	running_(false),
	task_(0)
{
	if (pthread_mutex_init(&mutex_, 0)) {
		LOGE("Unable to create mutex.");
		return;
	}
}

AndroidThread::~AndroidThread() {
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
	LOGD("Deleted thread with id: %llu.", getId());
}

UINT64 AndroidThread::getId() {
	return (UINT64) handle_;
}

void AndroidThread::start(Task* task) {
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
	if (pthread_create(&handle_, 0, &AndroidThread::staticRun, (void*) this)) {
		LOGE("Unable to create thread.");
		return;
	}
	LOGD("Started thread with id: %llu.", getId());
}

bool AndroidThread::isRunning() {
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

void* AndroidThread::staticRun(void* thread) {
	return (void*) ((AndroidThread*) thread)->run();
}

int AndroidThread::run() {
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
