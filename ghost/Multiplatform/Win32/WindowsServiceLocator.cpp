/*
 * WindowsServiceLocator.cpp
 *
 *  Created on: 2012.07.05
 *      Author: MS
 */

#include "WindowsSoundManager.h"
#include "WindowsServiceLocator.h"
#include "WindowsThread.h"
#include "WindowsSocket.h"
#include "WindowsGraphicsManager.h"
#include "WindowsFileManager.h"

WindowsServiceLocator::WindowsServiceLocator() :
		exit_(false),
		nScreenWidth_(0),
		nScreenHeight_(0),
		graphicsManager_(0),
		fileManager_(0),
		soundManager_(0)
{
	// Initialize counter.
	LARGE_INTEGER pf;
	QueryPerformanceFrequency(&pf);
	cpms_ = 1000.0f / pf.QuadPart;
	QueryPerformanceCounter(&timeBefore_);
	graphicsManager_ = NEW WindowsGraphicsManager(this);
	// Reset timer.
	updateTimer(0.0f);
	LOGD("Created Windows service locator.");
}

WindowsServiceLocator::~WindowsServiceLocator() {
	graphicsManager_->release();
	delete graphicsManager_;
	if (fileManager_ != 0) {
		delete fileManager_;
		fileManager_ = 0;
	}
	LOGD("Deleted Windows service locator.");
}

double WindowsServiceLocator::getFrameTime() {
	return frameDuration_;
}

void WindowsServiceLocator::setScreenWidth(int width) {
	nScreenWidth_ = width;
}

void WindowsServiceLocator::setScreenHeight(int height) {
	nScreenHeight_ = height;
}

int WindowsServiceLocator::getScreenWidth() {
	return nScreenWidth_;
}

int WindowsServiceLocator::getScreenHeight() {
	return nScreenHeight_;
}

void WindowsServiceLocator::exit() {
	exit_ = true;
}

bool WindowsServiceLocator::isFinished() {
	return exit_;
}

double WindowsServiceLocator::updateTimer(float sleep) {
	QueryPerformanceCounter(&timeAfter_);
	double timeElapsed =
		(timeAfter_.QuadPart - timeBefore_.QuadPart) * cpms_;
	timeBefore_ = timeAfter_;
	while (timeElapsed < sleep) {
		Sleep(1);
		timeElapsed += updateTimer(0.0f);
	}
	return frameDuration_ = timeElapsed;
}

Thread* WindowsServiceLocator::createThread() {
	return NEW WindowsThread();
}

UINT32 WindowsServiceLocator::getCurrentThreadId() {
	return GetCurrentThreadId();
}

Socket* WindowsServiceLocator::createSocket() {
	return NEW WindowsSocket();
}

GraphicsManager* WindowsServiceLocator::getGraphicsManager() {
	return graphicsManager_;
}

FileManager* WindowsServiceLocator::getFileManager() {
	if (fileManager_ == 0) {
		fileManager_ = NEW WindowsFileManager();
	}
	return fileManager_;
}

SoundManager* WindowsServiceLocator::getSoundManager() {
	if (soundManager_ == 0) {
		soundManager_ = NEW WindowsSoundManager();
		LOGD("Sound manager initialized.");
	}
	return soundManager_;
}
