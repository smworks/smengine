/*
 * WindowsServiceLocator.cpp
 *
 *  Created on: 2012.07.05
 *      Author: MS
 */

#include "WindowsSoundManager.h"
#include "WindowsServiceLocator.h"
#include "WindowsSocket.h"
#include "WindowsGraphicsManager.h"
#include "WindowsFileManager.h"
#include "WindowsDatabase.h"
#include "../NullGraphicsManager.h"

WindowsServiceLocator::WindowsServiceLocator() :
		graphicsManager_(0),
		fileManager_(0),
		soundManager_(0),
		database_(0)
{
	// Initialize counter.
	LARGE_INTEGER pf;
	QueryPerformanceFrequency(&pf);
	cpms_ = 1000.0f / pf.QuadPart;
	QueryPerformanceCounter(&timeBefore_);
	// Reset timer.
	updateTimer(0.0f);
	LOGD("Created Windows service locator.");
}

WindowsServiceLocator::~WindowsServiceLocator() {
	graphicsManager_->release();
	delete graphicsManager_;
	graphicsManager_ = 0;
	if (soundManager_ != 0) {
		delete soundManager_;
		soundManager_ = 0;
	}
	if (database_ != 0) {
		delete database_;
		database_ = 0;
	}
	if (fileManager_ != 0) {
		delete fileManager_;
		fileManager_ = 0;
	}
	LOGD("Deleted Windows service locator.");
}

double WindowsServiceLocator::getFrameTime() {
	return frameDuration_;
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

Socket* WindowsServiceLocator::createSocket(SocketParams params)
{

	return NEW WindowsSocket(params);
}

GraphicsManager* WindowsServiceLocator::getGraphicsManager() {
	if (graphicsManager_ == 0) {
		if (isGuiAvailable())
			graphicsManager_ = NEW WindowsGraphicsManager(this);
		else
			graphicsManager_ = NEW NullGraphicsManager(this);
	}
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
	}
	return soundManager_;
}

Database* WindowsServiceLocator::getDB() {
	if (database_ == 0) {
		database_ = NEW WindowsDatabase();
	}
	return database_;
}
