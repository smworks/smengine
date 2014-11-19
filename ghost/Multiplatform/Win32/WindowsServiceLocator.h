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

	/**
	 * @see ServiceLocator
	 */
	double getFrameTime();

	/**
	 * @see ServiceLocator
	 */
	void setScreenWidth(int width);

	/**
	 * @see ServiceLocator
	 */
	void setScreenHeight(int height);

	/**
	 * @see ServiceLocator
	 */
	int	getScreenWidth();

	/**
	 * @see ServiceLocator
	 */
	int getScreenHeight();

	/**
	 * @see ServiceLocator
	 */
	void exit();

	/**
	 * @see ServiceLocator
	 */
	bool isFinished();

	/**
	 * @see ServiceLocator
	 */
	double updateTimer(float sleep = 0.0f);

	/**
	 * @see ServiceLocator
	 */
	Thread* createThread();

	/**
	 * @see ServiceLocator.
	 */
	UINT32 getCurrentThreadId();

	/**
	 * @see ServiceLocator.
	 */
	Socket* createSocket();

	/**
	 * @see ServiceLocator
	 */
	GraphicsManager* getGraphicsManager();

	/**
	 * @see ServiceLocator
	 */
	FileManager* getFileManager();

	/**
	 * @see ServiceLocator
	 */
	SoundManager* getSoundManager();
private:
private:
	int nScreenWidth_;
	int nScreenHeight_;
	bool exit_;
private:
	double cpms_; // Counts per millisecond.
	LARGE_INTEGER timeBefore_,
		timeAfter_, timeElapsed_;
	double frameDuration_;
	GraphicsManager* graphicsManager_;
	FileManager* fileManager_;
	SoundManager* soundManager_;
};

#endif
