/*
 * GUIButton.cpp
 *
 *  Created on: 2013.08.09
 *      Author: Martynas Ðustavièius
 */

#include "GUIButton.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../ResourceManager.h"
#include "../ScriptManager.h"
#include "../Input.h"
#include "../Utils.h"

GUIButton::GUIButton(ServiceLocator* services) : GUIText(services) {}

GUIButton::~GUIButton() {
}

bool GUIButton::create() {
	string bckgSelected = getAttribute(ATTR_BACKGROUND_SELECTED);
	if (bckgSelected.length() > 0) {
		colorBckgSelected_.setRGBA(bckgSelected);
	}
    bool ret = GUIText::create();
	colorBckg_ = getAmbient();
	return ret;
}

Resource::Type GUIButton::getType() {
	return Resource::GUI_BUTTON;
}

void GUIButton::hasFocus() {
	if (getServiceLocator()->getInput()->keyPressed(Input::MOUSE_L)) {
		setAmbient(colorBckgSelected_);
	}
	else {
		setAmbient(colorBckg_);
	}
}