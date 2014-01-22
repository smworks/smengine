/*
 * Sound.cpp
 *
 *  Created on: 2013 02 12
 *      Author: Martynas Šustavičius
 */

#include "Sound.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Multiplatform/FileManager.h"

Sound::Sound(ServiceLocator* services) :
	Resource(services), name_(""), buffer_(0), size_(0), repeat_(false)
{}

Sound::~Sound() {
	release();
}

bool Sound::create() {
	name_ = getAttribute(ATTR_FILE);
	if (name_.length() == 0) {
		return false;
	}
	if (!getServiceLocator()->getFileManager()->loadRaw(
		buffer_, size_, (GHOST_SOUNDS + name_).c_str()))
	{
		LOGW("Unable to load sound file: %s.", name_.c_str());
	}
	return true;
}

void Sound::release() {
	delete [] buffer_;
	buffer_ = 0;
	size_ = 0;
}

SIZE Sound::getSize() {
	return sizeof(Sound) + size_ + name_.size();
}

Resource::Type Sound::getType() {
	return Resource::SOUND;
}

bool Sound::isValid() {
	return name_.length() > 0;
}

string& Sound::getName() {
	return name_;
}

INT8* Sound::getBuffer() {
	return buffer_;
}

SIZE Sound::getBufferSize() {
	return size_;
}

void Sound::setRepeat(bool state) {
	repeat_ = state;
}

bool Sound::isRepeatable() {
	return repeat_;
}