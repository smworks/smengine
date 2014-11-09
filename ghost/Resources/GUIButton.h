/*
 * GUIButton.h
 *
 *  Created on: 2013.08.09
 *      Author: Martynas Šustavičius
 */

#ifndef GUIBUTTON_H_
#define GUIBUTTON_H_

#include "GUIText.h"

class GUIButton : public GUIText {
public:
	GUIButton(ServiceLocator* services);
	~GUIButton();

    /**
     * @see GUISurface
     */
    bool create();

	/**
	 * @see GUISurface
	 */
	void hasFocus();
private:
	Color colorBckgSelected_;
	Color colorBckg_;
};

#endif
