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

	/**
	 * @see System
	 */
	double getFrameTime();

	/**
	 * @see System
	 */
	void setScreenWidth(int width);

	/**
	 * @see System
	 */
	void setScreenHeight(int height);

	/**
	 * @see System
	 */
	int	getScreenWidth();

	/**
	 * @see System
	 */
	int getScreenHeight();

	/**
	 * @see System
	 */
	void exit();

	/**
	 * @see System
	 */
	bool isFinished();

	/**
	 * @see System
	 */
	double updateTimer(float sleep = 0.0f);

	/**
	 * @see System
	 */
	Thread* createThread();

	/**
	 * @see System
	 */
	UINT32 getCurrentThreadId();

	/**
	 * @see System.
	 */
	Socket* createSocket();

	/**
	 * @see System
	 */
	GraphicsManager* getGraphicsManager();

	/**
	 * @see System
	 */
	FileManager* getFileManager();
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
};

#endif
