/*
 * WindowsSystem.cpp
 *
 *  Created on: 2012.07.05
 *      Author: MS
 */

#include "WindowsSystem.h"
#include "WindowsThread.h"
#include "WindowsSocket.h"
#include "WindowsGraphicsManager.h"
#include "WindowsFileManager.h"

WindowsSystem::WindowsSystem() :
		exit_(false),
		nScreenWidth_(0),
		nScreenHeight_(0),
		graphicsManager_(0),
		fileManager_(0)
{
	// Initialize counter.
	LARGE_INTEGER pf;
	QueryPerformanceFrequency(&pf);
	cpms_ = 1000.0f / pf.QuadPart;
	QueryPerformanceCounter(&timeBefore_);
	// Reset timer.
	updateTimer(0.0f);
	LOGD("Created Windows system.");
}

WindowsSystem::~WindowsSystem() {
	if (graphicsManager_ != 0) {
		delete graphicsManager_;
		graphicsManager_ = 0;
	}
	if (fileManager_ != 0) {
		delete fileManager_;
		fileManager_ = 0;
	}
	LOGD("Deleted Windows system.");
}

double WindowsSystem::getFrameTime() {
	return frameDuration_;
}

void WindowsSystem::setScreenWidth(int width) {
	nScreenWidth_ = width;
}

void WindowsSystem::setScreenHeight(int height) {
	nScreenHeight_ = height;
}

int WindowsSystem::getScreenWidth() {
	return nScreenWidth_;
}

int WindowsSystem::getScreenHeight() {
	return nScreenHeight_;
}

void WindowsSystem::exit() {
	exit_ = true;
}

bool WindowsSystem::isFinished() {
	return exit_;
}

double WindowsSystem::updateTimer(float sleep) {
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

Thread* WindowsSystem::createThread() {
	return NEW WindowsThread();
}

UINT32 WindowsSystem::getCurrentThreadId() {
	return GetCurrentThreadId();
}

Socket* WindowsSystem::createSocket() {
	return NEW WindowsSocket();
}

GraphicsManager* WindowsSystem::getGraphicsManager() {
	if (graphicsManager_ == 0) {
		graphicsManager_ = NEW WindowsGraphicsManager(this);
	}
	return graphicsManager_;
}

FileManager* WindowsSystem::getFileManager() {
	if (fileManager_ == 0) {
		fileManager_ = NEW WindowsFileManager();
	}
	return fileManager_;
}