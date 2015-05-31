/*
 * LinuxServiceLocator.cpp
 *
 *  Created on: 2013.03.26
 *      Author: Martynas Šustavičius
 */

#include "LinuxServiceLocator.h"
#include "LinuxGraphicsManager.h"
#include "LinuxFileManager.h"
#include "LinuxSocket.h"
#include "LinuxDatabase.h"
#include "../../Resources/TextureRGBA.h"

LinuxServiceLocator::LinuxServiceLocator() :
		nScreenWidth_(0),
		nScreenHeight_(0),
		exit_(false),
		graphicsManager_(0)
{
	// Initialize managers.
	fileManager_ = NEW LinuxFileManager();
	// Reset timer.
	updateTimer(0.0f);
}

LinuxServiceLocator::~LinuxServiceLocator() {
	if (graphicsManager_ != 0) {
		delete graphicsManager_;
		graphicsManager_ = 0;
		LOGI("Deleted graphics manager.");
	}
	if (fileManager_ != 0) {
		delete fileManager_;
		fileManager_ = 0;
		LOGI("Deleted file manager.");
	}
}

double LinuxServiceLocator::getTimeElapsed() {
	clock_gettime(CLOCK_MONOTONIC, &timeElapsed_);
		double timeElapsed = (timeElapsed_.tv_sec * 1000.0f + timeElapsed_.tv_nsec / 1000000.0f);
		return timeElapsed;
}

double LinuxServiceLocator::getFrameTime() {
	return frameDuration_;
}

void LinuxServiceLocator::setScreenWidth(int width) {
	nScreenWidth_ = width;
}

void LinuxServiceLocator::setScreenHeight(int height) {
	nScreenHeight_ = height;
}

int LinuxServiceLocator::getScreenWidth() {
	return nScreenWidth_;
}

int LinuxServiceLocator::getScreenHeight() {
	return nScreenHeight_;
}

void LinuxServiceLocator::exit() {
	exit_ = true;
}

bool LinuxServiceLocator::isFinished() {
	return exit_;
}

double LinuxServiceLocator::updateTimer(float sleep) {
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

UINT32 LinuxServiceLocator::getCurrentThreadId() {
	return static_cast<UINT32>(pthread_self());
}

Socket* LinuxServiceLocator::createSocket(SocketParams sp) {
	return NEW LinuxSocket(sp);
}

GraphicsManager* LinuxServiceLocator::getGraphicsManager() {
	if (graphicsManager_ == 0) {
		graphicsManager_ = NEW LinuxGraphicsManager(this);
	}
	return graphicsManager_;
}

FileManager* LinuxServiceLocator::getFileManager() {
	return fileManager_;
}

SoundManager* LinuxServiceLocator::getSoundManager() {
	if (soundManager_ == 0) {
		soundManager_ = 0; //NEW LinuxSoundManager();
		LOGD("Sound manager initialized.");
	}
	return soundManager_;
}

Database* LinuxServiceLocator::getDB() {
	if (database_ == 0) {
		database_ = 0; //NEW LinuxDatabase(getFileManager());
		LOGD("Database initialized.");
	}
	return database_;
}
