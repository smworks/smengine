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
#include "../../Thread.h"

class AndroidServiceLocator : public ServiceLocator {
public:

	/**
	 * @param env - java environment.
	 * @param obj - class object in java that contains
	 * methods used by native code.
	 */
	AndroidServiceLocator(JNIEnv* env, jobject obj);
	~AndroidServiceLocator();

    double getFrameTime();
    double updateTimer(float sleep = 0.0f);
    Socket* createSocket(SocketParams params);
	GraphicsManager* getGraphicsManager();
	FileManager* getFileManager();
	SoundManager* getSoundManager();
    Database* getDB();
private:
	timespec timeBefore_,
			timeAfter_, timeElapsed_;
	double frameDuration_;
	GraphicsManager* graphicsManager_;
	FileManager* fileManager_;
	SoundManager* soundManager_;
    Database* database_;
	JNIEnv* env_;
	jobject obj_;
};

#endif
