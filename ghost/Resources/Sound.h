/*
 * Sound.h
 *
 *  Created on: 2013 02 12
 *      Author: Martynas Šustavičius
 */

#ifndef SOUND_H_
#define SOUND_H_

#include "Resource.h"
class Sound : public Resource {
public:
	Sound(ServiceLocator* services);
	~Sound();

	/**
	 * @see Resource
	 */
	bool create();

	/**
	 * @see Resource
	 */
	void release();

	/**
	 * @see Resource
	 */
	SIZE getSize();

	/**
	 * @see Resource
	 */
	Resource::Type getType();

	/**
	 * @see Resource
	 */
	bool isValid();

	/**
	 * @return Name of the sound file.
	 */
	string& getName();

	/**
	 * @return Pointer to sound file buffer.
	 */
	INT8* getBuffer();

	/**
	 * @return Size of sound buffer.
	 */
	SIZE getBufferSize();

	/**
	 * @param state - if true, the sound will be repeated.
	 */
	void setRepeat(bool state);

	/**
	 * @return True if sound should loop infinitely.
	 */
	bool isRepeatable();
private:
	string name_;
	INT8* buffer_;
	SIZE size_;
	bool repeat_;
};

#endif