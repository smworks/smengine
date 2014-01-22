/*
 * AndroidSoundManager.h
 *
 *  Created on: 2013.03.31
 *      Author: Martynas Šustavičius
 */
#ifndef WINDOWSSOUNDMANAGER_H_
#define WINDOWSSOUNDMANAGER_H_

#include "../Ghost.h"
#include "../../Resources/Sound.h"
#include "../../../dependencies/includes/fmod/android/fmod.hpp"
#include "../../../dependencies/includes/fmod/android/fmod_errors.h"

#include "../SoundManager.h"
#include <vector>
#include <algorithm>

class AndroidSoundManager : public SoundManager {
public:
	AndroidSoundManager();
	~AndroidSoundManager();

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
