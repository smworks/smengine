/*
 * SoundManager.h
 *
 *  Created on: 2013.08.24
 *      Author: Martynas Šustavičius
 */
#ifndef MACOSXSOUNDMANAGER_H_
#define MACOSXSOUNDMANAGER_H_

#include "../Ghost.h"
#include "../../Resources/Sound.h"
#include "../../../dependencies/includes/fmod/osx/fmod.hpp"
#include "../../../dependencies/includes/fmod/osx/fmod_errors.h"

#include "../SoundManager.h"

class MacOSXSoundManager : public SoundManager {
public:
	MacOSXSoundManager();
	~MacOSXSoundManager();

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
