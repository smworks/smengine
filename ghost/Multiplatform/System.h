/*
 * System.h
 *
 *  Created on: 2012.06.19
 *      Author: Martynas Šustavičius
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "../Multiplatform/Ghost.h"

class Thread;
class Socket;
class SoundManager;
class GraphicsManager;
class FileManager;

class System {
public:
	virtual ~System() {};

	/**
	 * Returns time value in milliseconds.
	 * @return Time frame took to execute.
	 */
	virtual double getFrameTime() = 0;

	/**
	 * Sets new screen width value.
	 * @param width - new screen width in pixels.
	 */
	virtual void setScreenWidth(int width) = 0;

	/**
	 * Sets new screen height value.
	 * @param height - new screen height in pixels.
	 */
	virtual void setScreenHeight(int height) = 0;

	/**
	 * @return Current screen width in pixels.
	 */
	virtual int	getScreenWidth() = 0;

	/**
	 * @return Current screen height in pixels.
	 */
	virtual int getScreenHeight() = 0;

	/**
	 * Sets program quit indicator to true.
	 * Doesn't quit immedietly.
	 */
	virtual void exit() = 0;

	/**
	 * Returns true if program was
	 * closed externaly or
	 * method exit was called.
	 * @return true if program needs to be closed.
	 */
	virtual bool isFinished() = 0;

	/**
	 * @param sleep - optional parameter,
	 * makes sure that all program is
	 * sent into sleep mode until
	 * atleast the specified amount
	 * of time has elapsed.
	 * @return Amount of time that has
	 * passed since last call to this
	 * method.
	 */
	virtual double updateTimer(float sleep = 0.0f) = 0;

	/**
	 * Return thread instance.
	 * @return Pointer to thread.
	 */
	virtual Thread* createThread() = 0;

	/**
	 * Returns unique index for thread calling this method.
	 * @return Unique index.
	 */
	virtual UINT32 getCurrentThreadId() = 0;

	/**
	 * Return socket instance.
	 * @return Pointer to socket object.
	 */
	virtual Socket* createSocket() = 0;

	/**
	 * Return graphics manager instance.
	 * @return Pointer to graphics manager.
	 */
	virtual GraphicsManager* getGraphicsManager() = 0;

	/**
	 * Return file manager instance.
	 * @return Pointer to file manager.
	 */
	virtual FileManager* getFileManager() = 0;
};

#endif
