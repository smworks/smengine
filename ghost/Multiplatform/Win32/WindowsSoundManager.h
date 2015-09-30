/*
 * WindowsSoundManager.h
 *
 *  Created on: 2013.02.03
 *      Author: Martynas Šustavičius
 */

#ifndef WINDOWSSOUNDMANAGER_H_
#define WINDOWSSOUNDMANAGER_H_

#include "../SoundManager.h"

#ifndef ENABLE_SOUND
class WindowsSoundManager : public SoundManager {
public:
	WindowsSoundManager() {}
	~WindowsSoundManager() {}
	void reset() {}
	void play(Sound* sound) {}
	void play(string name, bool repeat) {}
	void update() {}
};
#else

#include "../../Resources/Sound.h"
#include "../../../dependencies/includes/fmod/win32/fmod.hpp"
#include "../../../dependencies/includes/fmod/win32/fmod_errors.h"

#ifdef SMART_DEBUG
	#pragma comment(lib, "fmodexL_vc.lib")
#else
	#pragma comment(lib, "fmodex_vc.lib")
#endif

#include <vector>
#include <algorithm>

class WindowsSoundManager : public SoundManager {
public:
	WindowsSoundManager();
	~WindowsSoundManager();

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