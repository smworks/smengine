/*
 * AndroidServiceLocator.cpp
 *
 *  Created on: 2013.03.31
 *      Author: Martynas Šustavičius
 */

#include "AndroidServiceLocator.h"
#include "AndroidSoundManager.h"
#include "AndroidGraphicsManager.h"
#include "AndroidFileManager.h"
#include "AndroidSocket.h"
#include "AndroidDatabase.h"

AndroidServiceLocator::AndroidServiceLocator(JNIEnv* env, jobject obj) :
	graphicsManager_(0),
	fileManager_(0),
	soundManager_(0),
    database_(0),
	env_(env),
	obj_(obj)
{
	LOGD("Created Android system.");
	// Reset timer.
	clock_gettime(CLOCK_MONOTONIC, &timeBefore_);
}

AndroidServiceLocator::~AndroidServiceLocator() {
	graphicsManager_->release();
    delete graphicsManager_;
	if (fileManager_ != 0) {
		delete fileManager_;
		fileManager_ = 0;
	}
    if (database_ != 0) {
        delete database_;
        database_ = 0;
    }
	if (soundManager_ != 0) {
		delete soundManager_;
		soundManager_ = 0;
	}
	LOGD("Deleted Android system.");
}

double AndroidServiceLocator::getFrameTime() {
	return frameDuration_;
}

double AndroidServiceLocator::updateTimer(float sleep) {
	clock_gettime(CLOCK_MONOTONIC, &timeAfter_);
	double timeElapsed = ((timeAfter_.tv_sec * 1000.0f + timeAfter_.tv_nsec / 1000000.0f) -
			(timeBefore_.tv_sec * 1000.0f + timeBefore_.tv_nsec / 1000000.0f));
	timeBefore_ = timeAfter_;
	while (timeElapsed < sleep) {
		usleep(1000);
		timeElapsed += updateTimer(0.0f);
	}
	return frameDuration_ = timeElapsed;
}

Socket* AndroidServiceLocator::createSocket(SocketParams params) {
	return NEW AndroidSocket();
}

GraphicsManager* AndroidServiceLocator::getGraphicsManager() {
	if (graphicsManager_ == 0) {
		graphicsManager_ = NEW AndroidGraphicsManager(this);
		LOGD("Graphics manager initialized.");
	}
	return graphicsManager_;
}

FileManager* AndroidServiceLocator::getFileManager() {
	if (fileManager_ == 0) {
		fileManager_ = NEW AndroidFileManager(env_, obj_);
		LOGD("File manager initialized.");
	}
	return fileManager_;
}

SoundManager* AndroidServiceLocator::getSoundManager() {
	if (soundManager_ == 0) {
		soundManager_ = NEW AndroidSoundManager();
		LOGD("Sound manager initialized.");
	}
	return soundManager_;
}

Database* AndroidServiceLocator::getDB() {
    if (database_ == 0) {
        database_ = NEW AndroidDatabase();
    }
    return database_;
}