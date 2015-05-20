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

GUIManager::GUIManager(ServiceLocator* services) :
	services_(services),
	selectedSurface_(0)
{
	LOGD("Created GUI manager.");
}

GUIManager::~GUIManager() {
	LOGD("Deleted GUI manager.");
}

void GUIManager::refreshNodes(Node* node) {
    nodes_.clear();
    node->toGUIArray(nodes_);
}

void GUIManager::update() {
    INT32 count = (INT32) nodes_.size();
    int x = services_->getInput()->getPointerX();
    int y = services_->getInput()->getPointerY();
    for (INT32 i = count - 1; i >= 0; i--) {
        Node* node = nodes_[i];
		if (!node->getState(Node::RENDERABLE)) {
			continue;
		}
		GUISurface* surface = dynamic_cast<GUISurface*>(node->getResource());
        if (surface == 0) continue;
		if (surface->getPosX() < x
			&& surface->getWidth() + surface->getPosX() > x
			&& surface->getPosY() < y
			&& surface->getHeight() + surface->getPosY() > y)
		{
			surface->hasFocus();
			if (services_->getInput()->keyReleased(
				Input::MOUSE_L) || services_->getInput()->keyReleased(Input::TOUCH))
			{
				selectedSurface_ = surface;
				services_->getScriptManager()->provideEventGUI(
					node, CLICK);
				return;
			}
		}
    }
}

GUISurface* GUIManager::getSelectedSurface() {
	return selectedSurface_;
}
