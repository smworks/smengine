/*
 * LinuxSoundManager.cpp
 *
 *  Created on: 2013.03.26
 *      Author: Martynas Šustavičius
 */

#include "LinuxSoundManager.h"

LinuxSoundManager::LinuxSoundManager() :
	soundSystem_(0),
	soundChannel_(0),
	sound_(0)
{
#ifdef _DEBUG
	FMOD::Debug_SetLevel(FMOD_DEBUG_LEVEL_ERROR | FMOD_DEBUG_LEVEL_WARNING);
#endif
	FMOD_RESULT result;
	UINT32 version;
	result = FMOD::System_Create(&soundSystem_);
	if (result != FMOD_OK) {
		LOGW("Failed creating fmod sound_System_.");
	}
	result = soundservices_->getVersion(&version);
	if (result != FMOD_OK) {
		LOGW("Failed getting fmod version.");
	}
    if (version < FMOD_VERSION) {
        printf("Error! You are using an old version of FMOD %08x. "
			"This program requires %08x\n", version, FMOD_VERSION);
    }
	result = soundservices_->init(2, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK) {
		LOGW("Fmod initialization failed.");
	}
	reset();
}

LinuxSoundManager::~LinuxSoundManager() {
	FMOD_RESULT result;
	if (sound_ != 0) {
		result = sound_->release();
		if (result != FMOD_OK) {
			LOGW("Unable to release sound.");
		}
	}
	if (soundSystem_ != 0) {
		result = soundservices_->close();
		if (result != FMOD_OK) {
			LOGW("Unable to close sound system.");
		}
		result = soundservices_->release();
		if (result != FMOD_OK) {
			LOGW("Unable to release sound.");
		}
	}
}

void LinuxSoundManager::reset() {
}

void LinuxSoundManager::play(string name, bool repeat) {
	FMOD_RESULT result;
	int flags = FMOD_HARDWARE | FMOD_2D;
	if (repeat) {
		flags |= FMOD_LOOP_NORMAL;
	}
	else {
		flags |= FMOD_LOOP_OFF;
	}

	result = soundservices_->createStream(
		(GHOST_SOUNDS + name).c_str(), flags, 0, &sound_);
	if (result != FMOD_OK) {
		LOGW("Unable to create stream from file \"%s\"", name.c_str());
		return;
	}
	result = soundservices_->playSound(
		FMOD_CHANNEL_FREE, sound_, false, &soundChannel_);
	if (result != FMOD_OK) {
		LOGW("Unable to play sound.");
	}
}

void LinuxSoundManager::update() {
	soundservices_->update();
}