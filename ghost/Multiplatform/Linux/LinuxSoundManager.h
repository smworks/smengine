#ifndef LINUXSOUNDMANAGER_H_
#define LINUXSOUNDMANAGER_H_

#ifdef DISABLE_SOUND
class LinuxSoundManager : public SoundManager {
public:
	LinuxSoundManager() {}
	~LinuxSoundManager() {}
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

#include "../SoundManager.h"

#include <vector>
#include <algorithm>

class LinuxSoundManager : public SoundManager {
//public:
//	LinuxSoundManager();
//	~LinuxSoundManager();
//
//	/**
//	 * @see SoundManager
//	 */
//	void reset();
//
//	/**
//	 * @see SoundManager
//	 */
//	void play(Sound* sound);
//
//	/**
//	 * @see SoundManager
//	 */
//	void play(string name, bool repeat);
//
//	/**
//	 * @see SoundManager
//	 */
//	void update();
//protected:
//	FMOD::System *soundSystem_;
//    FMOD::Channel* soundChannel_;
//	FMOD::Sound* sound_;
public:
	LinuxSoundManager() {}
	~LinuxSoundManager() {}
	void reset() {}
	void play(Sound* sound) {}
	void play(string name, bool repeat) {}
	void update() {}
};

#endif
#endif
