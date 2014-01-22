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
	rootNode_(0),
	selectedSurface_(0)
{
	LOGD("Created GUI manager.");
}
 
GUIManager::~GUIManager() {
	LOGD("Deleted GUI manager.");
}

void GUIManager::setRootNode(Node* node) {
    rootNode_ = node;
    nodes_.clear();
    rootNode_->toGUIArray(nodes_);
}

void GUIManager::update() {
    SIZE count = nodes_.size();
    int x = services_->getInput()->getPointerX();
    int y = services_->getInput()->getPointerY();
    for (SIZE i = 0; i < count; i++) {
        Node* node = nodes_[i];
		if (node->getState(Node::RENDERABLE)
			&& node->getPos().getX() < x
			&& node->getPos().getX() + node->getScale().getX() > x
            && node->getPos().getY() < y
			&& node->getPos().getY() + node->getScale().getY() > y)
        {
            vector<Resource*> resources = node->getResources();
            for (SIZE i = 0; i < resources.size(); i++) {
                GUISurface* surface = dynamic_cast<GUISurface*>(resources[i]);
                if (surface == 0) {
                    continue;
                }
                surface->hasFocus();
                if (services_->getInput()->keyReleased(Input::MOUSE_L)) {
					selectedSurface_ = surface;
					services_->getScriptManager()->provideEventGUI(
						node, CLICK);
				}
            }
        }
    }
}

GUISurface* GUIManager::getSelectedSurface() {
	return selectedSurface_;
}