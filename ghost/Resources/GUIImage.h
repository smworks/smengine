/*
 * GUIImage.h
 *
 *  Created on: 2013.07.07
 *      Author: Martynas Ðustavièius
 */

#ifndef GUIIMAGE_H_
#define GUIIMAGE_H_

#include "GUISurface.h"
class TextureRGBA;

class GUIImage : public GUISurface {
public:
	static const string TAG_SRC;
public:
	GUIImage(ServiceLocator* services);
	~GUIImage();

	/**
	 * @see Resource
	 */
	Resource::Type getType();

	/**
	 * @see GUISurface
	 */
	void hasFocus();

	/**
	 * @see GUISurface
	 */
	void update();
private:
    TextureRGBA* src_;
};

#endif
