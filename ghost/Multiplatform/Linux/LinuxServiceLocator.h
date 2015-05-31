/*
 * LinuxServiceLocator.h
 *
 *  Created on: 2013.03.26
 *      Author: Martynas Šustavičius
 */

#ifndef LINUXSERVICELOCATOR_H_
#define LINUXSERVICELOCATOR_H_

#include "../Ghost.h"
#include "../ServiceLocator.h"
class LinuxThread;
class LinuxSoundManager;
class LinuxGraphicsManager;
class LinuxFileManager;
class LinuxSocket;
class TextureRGBA;

class LinuxServiceLocator : public ServiceLocator {
public:
	LinuxServiceLocator();
	~LinuxServiceLocator();

	/**
	 * @see System
	 */
	double getTimeElapsed();
	double getFrameTime();
	void setScreenWidth(int width);
	void setScreenHeight(int height);
	int	getScreenWidth();
	int getScreenHeight();
	void exit();
	bool isFinished();
	double updateTimer(float sleep = 0.0f);
	UINT32 getCurrentThreadId();
	Socket* createSocket(SocketParams sp);
	GraphicsManager* getGraphicsManager();
	FileManager* getFileManager();
	SoundManager* getSoundManager();
	Database* getDB();
private:
private:
	int nScreenWidth_;
	int nScreenHeight_;
	bool exit_;
private:
	timespec timeBefore_,
			timeAfter_, timeElapsed_;
	double frameDuration_;
	GraphicsManager* graphicsManager_;
	FileManager* fileManager_;
	SoundManager* soundManager_;
	Database* database_;
};

#endif
