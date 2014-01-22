/*
 * LinuxSystem.cpp
 *
 *  Created on: 2013.03.26
 *      Author: Martynas Šustavičius
 */

#include "LinuxSystem.h"
#include "LinuxThread.h"
#include "LinuxSoundManager.h"
#include "LinuxGraphicsManager.h"
#include "LinuxFileManager.h"
#include "LinuxSocket.h"
#include "../../Resources/TextureRGBA.h"

LinuxSystem::LinuxSystem() :
		nScreenWidth_(0),
		nScreenHeight_(0),
		exit_(false),
		graphicsManager_(0)
{
	// Initialize managers.
	soundManager_ = NEW LinuxSoundManager();
	fileManager_ = NEW LinuxFileManager();
	// Reset timer.
	updateTimer(0.0f);
}

LinuxSystem::~LinuxSystem() {
	if (soundManager_ != 0) {
		delete soundManager_;
		soundManager_ = 0;
		LOGI("Deleted sound manager.");
	}
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

double LinuxSystem::getTimeElapsed() {
	clock_gettime(CLOCK_MONOTONIC, &timeElapsed_);
		double timeElapsed = (timeElapsed_.tv_sec * 1000.0f + timeElapsed_.tv_nsec / 1000000.0f);
		return timeElapsed;
}

double LinuxSystem::getFrameTime() {
	return frameDuration_;
}

void LinuxSystem::setScreenWidth(int width) {
	nScreenWidth_ = width;
}

void LinuxSystem::setScreenHeight(int height) {
	nScreenHeight_ = height;
}

int LinuxSystem::getScreenWidth() {
	return nScreenWidth_;
}

int LinuxSystem::getScreenHeight() {
	return nScreenHeight_;
}

void LinuxSystem::exit() {
	exit_ = true;
}

bool LinuxSystem::isFinished() {
	return exit_;
}

double LinuxSystem::updateTimer(float sleep) {
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

Thread* LinuxSystem::createThread() {
	return NEW LinuxThread();
}

UINT32 LinuxSystem::getCurrentThreadId() {
	return static_cast<UINT32>(pthread_self());
}

Socket* LinuxSystem::createSocket() {
	return NEW LinuxSocket();
}

SoundManager* LinuxSystem::getSoundManager() {
	return soundManager_;
}

GraphicsManager* LinuxSystem::getGraphicsManager() {
	if (graphicsManager_ == 0) {
		graphicsManager_ = NEW LinuxGraphicsManager(this);
	}
	return graphicsManager_;
}

FileManager* LinuxSystem::getFileManager() {
	return fileManager_;
}
