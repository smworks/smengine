/*
 * AndroidServiceLocator.cpp
 *
 *  Created on: 2013.03.31
 *      Author: Martynas Šustavičius
 */

#include "AndroidServiceLocator.h"
#include "AndroidThread.h"
#include "AndroidGraphicsManager.h"
#include "AndroidFileManager.h"
#include "AndroidSocket.h"
#include "../../Resources/TextureRGBA.h"

AndroidServiceLocator::AndroidServiceLocator(JNIEnv* env, jobject obj) :
	nScreenWidth_(0),
	nScreenHeight_(0),
	exit_(false),
	graphicsManager_(0),
	fileManager_(0),
	env_(env),
	obj_(obj)
{
	LOGD("Created Android system.");
	// Reset timer.
	clock_gettime(CLOCK_MONOTONIC, &timeBefore_);
}

AndroidServiceLocator::~AndroidServiceLocator() {
	if (graphicsManager_ != 0) {
		delete graphicsManager_;
		graphicsManager_ = 0;
	}
	if (fileManager_ != 0) {
		delete fileManager_;
		fileManager_ = 0;
	}
	LOGD("Deleted Android system.");
}

double AndroidServiceLocator::getTimeElapsed() {
	clock_gettime(CLOCK_MONOTONIC, &timeElapsed_);
	double timeElapsed = (timeElapsed_.tv_sec * 1000.0f + timeElapsed_.tv_nsec / 1000000.0f);
	return timeElapsed;
}

double AndroidServiceLocator::getFrameTime() {
	return frameDuration_;
}

void AndroidServiceLocator::setScreenWidth(int width) {
	nScreenWidth_ = width;
}

void AndroidServiceLocator::setScreenHeight(int height) {
	nScreenHeight_ = height;
}

int AndroidServiceLocator::getScreenWidth() {
	return nScreenWidth_;
}

int AndroidServiceLocator::getScreenHeight() {
	return nScreenHeight_;
}

void AndroidServiceLocator::exit() {
	exit_ = true;
}

bool AndroidServiceLocator::isFinished() {
	return exit_;
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

Thread* AndroidServiceLocator::createThread() {
	return NEW AndroidThread();
}

UINT32 AndroidServiceLocator::getCurrentThreadId() {
	return static_cast<UINT32>(pthread_self());
}

Socket* AndroidServiceLocator::createSocket() {
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