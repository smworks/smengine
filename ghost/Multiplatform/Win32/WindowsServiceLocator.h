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
	 * @see System.
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
	double cpms_; // Counts per millisecond.
	LARGE_INTEGER timeBefore_,
		timeAfter_, timeElapsed_;
	double frameDuration_;
	GraphicsManager* graphicsManager_;
	FileManager* fileManager_;
};

#endif
