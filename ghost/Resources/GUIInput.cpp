/*
 * GUIInput.cpp
 *
 *  Created on: 2013.08.28
 *      Author: Martynas Šustavičius
 */

#include "GUIInput.h"
#include "TextureRGBA.h"
#include "../Shapes.h"
#include "../ResourceManager.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Node.h"
#include "../Input.h"
#include "../GUIManager.h"

GUIInput::GUIInput(ServiceLocator* services) : GUIText(services) {}

GUIInput::~GUIInput() {}

Resource::Type GUIInput::getType() {
	return Resource::GUI_INPUT;
}

void GUIInput::hasFocus() {
}

void GUIInput::update() {
	if (getServiceLocator()->getGUIManager()->getSelectedSurface() == this) {
		if (getServiceLocator()->getInput()->keyReleased(Input::L)) {
			setText("L");
		}
		if (getServiceLocator()->getInput()->keyReleased(Input::M)) {
			setText("M");
		}
	}
}

bool GUIInput::create() {
    bool state = GUIText::create();
    setBackground("#00FFFF00");
    getDiffuse().setRGBA("#000000FF");
    return state;
}
