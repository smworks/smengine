/*
 * SoundManager.cpp
 *
 *  Created on: 2013.02.03
 *      Author: Martynas Šustavičius
 */

#include "SoundManager.h"
#include "Multiplatform/ServiceLocator.h"
#include "Multiplatform/FileManager.h"
#include "Resources/Sound.h"
#include "../dependencies/includes/fmod/fmod_errors.h"

SoundManager::SoundManager(ServiceLocator* services) :
	services_(services),
	soundSystem_(0),
	soundChannel_(0),
	sound_(0)
{
//#ifdef SMART_DEBUG
//	FMOD::Debug_SetLevel(FMOD_DEBUG_LEVEL_ERROR | FMOD_DEBUG_LEVEL_WARNING);
//#endif
	FMOD_RESULT result;
	UINT32 version;
	result = FMOD::System_Create(&soundSystem_);
	if (result != FMOD_OK) {
		LOGW("Failed creating fmod sound_System_.");
	}
	result = soundSystem_->getVersion(&version);
	if (result != FMOD_OK) {
		LOGW("Failed getting fmod version.");
	}
    if (version < FMOD_VERSION) {
        printf("Error! You are using an old version of FMOD %08x. "
			"This program requires %08x\n", version, FMOD_VERSION);
    }
	result = soundSystem_->init(2, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK) {
		LOGW("Fmod initialization failed.");
	}
	reset();
	LOGD("Created sound manager.");
}

SoundManager::~SoundManager() {
	FMOD_RESULT result;
	if (sound_ != 0) {
		result = sound_->release();
		if (result != FMOD_OK) {
			LOGW("Unable to release sound.");
		}
	}
	//if (soundChannel_ != 0) {
	//	result = soundChannel_->stop();
	//	if (result != FMOD_OK) {
	//		LOGW("Unable to stop sound channel.");
	//	}
	//}
	if (soundSystem_ != 0) {
		result = soundSystem_->close();
		if (result != FMOD_OK) {
			LOGW("Unable to close sound system.");
		}
		result = soundSystem_->release();
		if (result != FMOD_OK) {
			LOGW("Unable to release sound.");
		}
	}
	LOGD("Deleted sound manager.");
}

void SoundManager::reset() {
}

void SoundManager::play(Sound* sound) {
	FMOD_RESULT result;
	int flags = FMOD_HARDWARE | FMOD_2D | FMOD_OPENMEMORY | FMOD_CREATESTREAM;
	if (sound->isRepeatable()) {
		flags |= FMOD_LOOP_NORMAL;
	}
	else {
		flags |= FMOD_LOOP_OFF;
	}
	FMOD_CREATESOUNDEXINFO settings;
	memset(&settings, 0, sizeof(FMOD_CREATESOUNDEXINFO));
	settings.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	settings.length = sound->getBufferSize();
	result = soundSystem_->createSound(
		reinterpret_cast<char* const>(sound->getBuffer()), flags, &settings, &sound_);
	if (result != FMOD_OK) {
		LOGW("Unable to create sound: %s.", sound->getName().c_str());
		return;
	}
	result = soundSystem_->playSound(
		FMOD_CHANNEL_FREE, sound_, false, &soundChannel_);
	if (result != FMOD_OK) {
		LOGW("Unable to play sound: %s.", sound->getName().c_str());
	}
}

void SoundManager::update() {
	soundSystem_->update();
}