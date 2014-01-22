/*
 * LinuxSoundManager.h
 *
 *  Created on: 2013.03.26
 *      Author: Martynas Šustavičius
 */
#ifndef WINDOWSSOUNDMANAGER_H_
#define WINDOWSSOUNDMANAGER_H_

#include "../Ghost.h"
#include "../../Resources/Sound.h"
#include "../../../dependencies/includes/fmod/linux/fmod.hpp"
#include "../../../dependencies/includes/fmod/linux/fmod_errors.h"

#include "../SoundManager.h"
#include <vector>
#include <algorithm>

class LinuxSoundManager : public SoundManager {
public:
	LinuxSoundManager();
	~LinuxSoundManager();

	/**
	 * @see SoundManager
	 */
	void reset();

	/**
	 * @see SoundManager
	 */
	void play(string name, bool repeat);

	/**
	 * @see SoundManager
	 */
	void update();
protected:
	FMOD::System *soundSystem_;
    FMOD::Channel* soundChannel_;
	FMOD::Sound* sound_;
};

#endif
