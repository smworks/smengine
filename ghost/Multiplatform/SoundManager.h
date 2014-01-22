/*
 * SoundManager.h
 *
 *  Created on: 2013.02.03
 *      Author: Martynas Šustavičius
 */

#ifndef SOUNDMANAGER_H_
#define SOUNDMANAGER_H_

#include "Ghost.h"

class SoundManager {
public:
	virtual ~SoundManager() {}

	/**
	 * Release sound resources. Set sound
	 * manager to initial state.
	 */
	virtual void reset() = 0;

	/**
	 * @param name - name of sound.
	 * @param repeat - true if repeat infinitely.
	 */
	virtual void play(string name, bool repeat) = 0;

	/**
	 * Updates sound synchronization.
	 * Removes finished sounds.
	 */
	virtual void update() = 0;
};

#endif
