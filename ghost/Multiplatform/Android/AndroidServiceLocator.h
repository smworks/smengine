/*
 * AndroidSystem.h
 *
 *  Created on: 2013.03.31
 *      Author: Martynas Šustavičius
 */

#ifndef ANDROIDSYSTEM_H_
#define ANDROIDSYSTEM_H_

#include "../Ghost.h"
#include "../ServiceLocator.h"

class AndroidServiceLocator : public ServiceLocator {
public:

	/**
	 * @param env - java environment.
	 * @param obj - class object in java that contains
	 * methods used by native code.
	 */
	AndroidServiceLocator(JNIEnv* env, jobject obj);
	~AndroidServiceLocator();

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

	/**
	 * @see System
	 */
	SoundManager* getSoundManager();
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
	JNIEnv* env_;
	jobject obj_;
};

#endif
