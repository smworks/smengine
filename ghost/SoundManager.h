/*
 * SoundManager.h
 *
 *  Created on: 2013.02.03
 *      Author: Martynas Šustavičius
 */

#ifndef SOUNDMANAGER_H_
#define SOUNDMANAGER_H_

#include "Multiplatform/Ghost.h"
#include "../dependencies/includes/fmod/fmod.hpp"
class ServiceLocator;
class Sound;

class SoundManager {
public:
	SoundManager(ServiceLocator* services);
	~SoundManager();

	/**
	 * Release sound resources. Set sound
	 * manager to initial state.
	 */
	void reset();

	/**
	 * @param sound - sound resource object.
	 */
	void play(Sound* sound);

	/**
	 * Updates sound synchronization.
	 * Removes finished sounds.
	 */
	void update();
protected:
	ServiceLocator* services_;
	FMOD::System *soundSystem_;
    FMOD::Channel* soundChannel_;
	FMOD::Sound* sound_;
};

#endif