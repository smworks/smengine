/*
 * WindowsServiceLocator.h
 *
 *  Created on: 2012.06.19
 *      Author: MS
 */

#ifndef WINDOWSSYSTEM_H_
#define WINDOWSSYSTEM_H_

#include "../Ghost.h"
#include "../ServiceLocator.h"

class WindowsServiceLocator : public ServiceLocator {
public:
	WindowsServiceLocator();
	~WindowsServiceLocator();
	double getFrameTime();
	double updateTimer(float sleep = 0.0f);
	Socket* createSocket(SocketParams params);
	GraphicsManager* getGraphicsManager();
	FileManager* getFileManager();
	SoundManager* getSoundManager();
	Database* getDB();
private:
	double cpms_; // Counts per millisecond.
	LARGE_INTEGER timeBefore_,
		timeAfter_, timeElapsed_;
	double frameDuration_;
	GraphicsManager* graphicsManager_;
	FileManager* fileManager_;
	SoundManager* soundManager_;
	Database* database_;
};

#endif
