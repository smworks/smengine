/*
 * AndroidSoundManager.h
 *
 *  Created on: 2013.03.31
 *      Author: Martynas Šustavičius
 */
#ifndef ANDROIDSOUNDMANAGER_H_
#define ANDROIDSOUNDMANAGER_H_

#include "../SoundManager.h"

#ifndef ENABLE_SOUND
class AndroidSoundManager : public SoundManager {
public:
	AndroidSoundManager() {}
	~AndroidSoundManager() {}
	void reset() {}
	void play(Sound* sound) {}
	void play(string name, bool repeat) {}
	void update() {}
};
#else
#include "../Ghost.h"
#include "../../Resources/Sound.h"
#include "../../../dependencies/includes/fmod/android/fmod.hpp"
#include "../../../dependencies/includes/fmod/android/fmod_errors.h"

#include <vector>
#include <algorithm>

class Martynas : public SoundManager {
public:
    Martynas();
    ~Martynas();
    	void reset() {}
    	void play(Sound* sound) {}
    	void play(string name, bool repeat) {}
    	void update() {}
};

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
	void play(Sound* sound);

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
#endif
