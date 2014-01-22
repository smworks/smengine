/*
 * MacOSXServiceLocator.h
 *
 *  Created on: 2013.08.24
 *      Author: Martynas Šustavičius
 */

#ifndef MACOSXSERVICELOCATOR_H_
#define MACOSXSERVICELOCATOR_H_

#include "../Ghost.h"
#include "../ServiceLocator.h"
class Thread;
class GraphicsManager;
class FileManager;
class Socket;


class MacOSXServiceLocator : public ServiceLocator {
public:
	MacOSXServiceLocator();
	~MacOSXServiceLocator();

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
	int nScreenWidth_;
	int nScreenHeight_;
	bool exit_;
private:
	SIZE timeBefore_, timeAfter_, timeElapsed_;
	double frameDuration_;
	GraphicsManager* graphicsManager_;
	FileManager* fileManager_;
};

#endif
