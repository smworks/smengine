/*
 * MacOSXServiceLocator.cpp
 *
 *  Created on: 2013.08.24
 *      Author: Martynas Šustavičius
 */

#include "MacOSXServiceLocator.h"
#include "Thread.h"
#include "MacOSXGraphicsManager.h"
#include "FileManager.h"
#include "Socket.h"
#include "../../Resources/TextureRGBA.h"

MacOSXServiceLocator::MacOSXServiceLocator() :
    timeBefore_(0),
    timeAfter_(0),
    timeElapsed_(0),
    nScreenWidth_(0),
    nScreenHeight_(0),
    exit_(false),
    graphicsManager_(0)
{
    PROFILE("Creating MacOSXServiceLocator object.");
	fileManager_ = NEW MacOSXFileManager();
    graphicsManager_ = NEW MacOSXGraphicsManager(this);
	// Reset timer.
	updateTimer(0.0f);
    PROFILE("MacOSXServiceLocator object created.");
}

MacOSXServiceLocator::~MacOSXServiceLocator() {
    PROFILE("Deleting MacOSXServiceLocator object.");
	if (graphicsManager_ != 0) {
		delete graphicsManager_;
		graphicsManager_ = 0;
	}
	if (fileManager_ != 0) {
		delete fileManager_;
		fileManager_ = 0;
	}
    PROFILE("MacOSXServiceLocator object deleted.");
}

double MacOSXServiceLocator::getFrameTime() {
	return frameDuration_;
}

void MacOSXServiceLocator::setScreenWidth(int width) {
	nScreenWidth_ = width;
}

void MacOSXServiceLocator::setScreenHeight(int height) {
	nScreenHeight_ = height;
}

int MacOSXServiceLocator::getScreenWidth() {
	return nScreenWidth_;
}

int MacOSXServiceLocator::getScreenHeight() {
	return nScreenHeight_;
}

void MacOSXServiceLocator::exit() {
	exit_ = true;
}

bool MacOSXServiceLocator::isFinished() {
	return exit_;
}

double MacOSXServiceLocator::updateTimer(float sleep) {
	timeAfter_ = getMicroseconds();
    double timeElapsed = ((double) (timeAfter_ - timeBefore_)) * 0.001f;
	timeBefore_ = timeAfter_;
	while (timeElapsed < sleep) {
		usleep(1000);
		timeElapsed += updateTimer(0.0f);
	}
	return frameDuration_ = timeElapsed;
}

Thread* MacOSXServiceLocator::createThread() {
	return NEW MacOSXThread();
}

UINT32 MacOSXServiceLocator::getCurrentThreadId() {
	return (UINT32) (SIZE) pthread_self();
}

Socket* MacOSXServiceLocator::createSocket() {
	return NEW MacOSXSocket();
}

GraphicsManager* MacOSXServiceLocator::getGraphicsManager() {
	return graphicsManager_;
}

FileManager* MacOSXServiceLocator::getFileManager() {
	return fileManager_;
}
