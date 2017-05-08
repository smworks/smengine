/*
 * SoundManager.h
 *
 *  Created on: 2013.08.24
 *      Author: Martynas Šustavičius
 */
#ifndef MACOSSOUNDMANAGER_H_
#define MACOSSOUNDMANAGER_H_

#include "../Ghost.h"
#include "../../Resources/Sound.h"
//#include "../../../dependencies/includes/fmod/osx/fmod.hpp"
//#include "../../../dependencies/includes/fmod/osx/fmod_errors.h"

#include "../SoundManager.h"

class MacOSSoundManager : public SoundManager {
public:
    MacOSSoundManager() {}
    ~MacOSSoundManager() {}

	/**
	 * @see SoundManager
	 */
    void reset() {}
    
    void play(Sound* sound) {};

	/**
	 * @see SoundManager
	 */
    void play(string name, bool repeat) {}

	/**
	 * @see SoundManager
	 */
    void update() {}
protected:
	//FMOD::System *soundSystem_;
    //FMOD::Channel* soundChannel_;
	//FMOD::Sound* sound_;
};

#endif
