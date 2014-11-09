/*
 * GUIInput.h
 *
 *  Created on: 2013.08.28
 *      Author: Martynas Šustavičius
 */

#ifndef GUIINPUT_H_
#define GUIINPUT_H_

#include "GUIText.h"

class GUIInput : public GUIText {
public:
	GUIInput(ServiceLocator* services);
	~GUIInput();

	/**
	 * @see GUISurface
	 */
	void hasFocus();

	/**
	 * @see GUISurface
	 */
	void update();
protected:
    /**
     * @see GUISurface
     */
    bool create();
};

#endif
