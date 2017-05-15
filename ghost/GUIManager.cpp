/*
 * GUIManager.cpp
 *
 *  Created on: 2013.07.11
 *      Author: Martynas Šustavičius
 */

#include "GUIManager.h"
#include "Multiplatform/ServiceLocator.h"
#include "Input.h"
#include "Node.h"
#include "ScriptManager.h"
#include "Resources/Resource.h"
#include "Resources/GUISurface.h"

GUIManager::GUIManager(ServiceLocator* serviceLocator) :
	ServiceProvider(serviceLocator),
	selectedSurface(nullptr)
{
	LOGD("Created GUI manager.");
}

GUIManager::~GUIManager() {
	LOGD("Deleted GUI manager.");
}

void GUIManager::refreshNodes(Node* node) {
    nodes.clear();
    node->toGUIArray(nodes);
}

void GUIManager::update() {
    int x = getInput()->getPointerX();
    int y = getServiceLocator()->getScreenHeight() - getInput()->getPointerY();
	for (auto it = nodes.rbegin(); it != nodes.rend(); it++) {
		Node* node = *it;
    	if (node->getPos().getX() < x
			&& node->getScale().getX() + node->getPos().getX() > x
			&& node->getPos().getY() < y
			&& node->getScale().getY() + node->getPos().getY() > y)
		{
            GUISurface* surface = dynamic_cast<GUISurface*>(node->getResource());
			surface->hasFocus();
			if (getInput()->keyReleased(Input::MOUSE_L)
				|| getInput()->keyReleased(Input::TOUCH))
			{
				selectedSurface = surface;
				getScriptManager()->provideEventGUI(node, CLICK);
				return;
			}
		}
    }
}

GUISurface* GUIManager::getSelectedSurface() {
	return selectedSurface;
}
