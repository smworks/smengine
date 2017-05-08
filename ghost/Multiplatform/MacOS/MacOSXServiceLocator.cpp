#include "MacOSXServiceLocator.h"
#include "Thread.h"
#include "MacOSXGraphicsManager.h"
#include "FileManager.h"
#include "Socket.h"
#include "../../Resources/TextureRGBA.h"
#include "../NullGraphicsManager.h"
#include "MacOSDatabase.h"
#include "MacOSSoundManager.h"

MacOSXServiceLocator::MacOSXServiceLocator() :
    graphicsManager_(0),
    fileManager_(0),
    soundManager_(0),
    database_(0),
    timeAfter_(0)
{
	updateTimer(0.0f);
    PROFILE("MacOSXServiceLocator object created.");
}

MacOSXServiceLocator::~MacOSXServiceLocator() {
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
    PROFILE("MacOSXServiceLocator object deleted.");
}

double MacOSXServiceLocator::getFrameTime() {
	return frameDuration_;
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

Socket* MacOSXServiceLocator::createSocket(SocketParams paramss) {
    return NEW MacOSXSocket();
}

GraphicsManager* MacOSXServiceLocator::getGraphicsManager() {
    if (graphicsManager_ == 0) {
        if (isGuiAvailable())
            graphicsManager_ = NEW MacOSXGraphicsManager(this);
        else
            graphicsManager_ = NEW NullGraphicsManager(this);
    }
    return graphicsManager_;
}

FileManager* MacOSXServiceLocator::getFileManager() {
    if (fileManager_ == 0) {
        fileManager_ = NEW MacOSXFileManager();
    }
    return fileManager_;
}

SoundManager* MacOSXServiceLocator::getSoundManager() {
    if (soundManager_ == 0) {
        soundManager_ = new MacOSSoundManager();
    }
    return soundManager_;
}

Database* MacOSXServiceLocator::getDB() {
    if (database_ == 0) {
        database_ = NEW MacOSDatabase();
    }
    return database_;
}

//Thread* MacOSXServiceLocator::createThread() {
//	return NEW MacOSXThread();
//}
//
//UINT32 MacOSXServiceLocator::getCurrentThreadId() {
//	return (UINT32) (SIZE) pthread_self();
//}