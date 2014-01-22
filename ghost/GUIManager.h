/*
 * GUIManager.h
 *
 *  Created on: 2013.07.11
 *      Author: Martynas Šustavičius
 */

#ifndef GUIMANAGER_H_
#define GUIMANAGER_H_

#include "Multiplatform/Ghost.h"
class ServiceLocator;
class Node;
class GUISurface;
class GUIContainer;
class GUIImage;
class GUIText;

/**
 * Class handles GUI elements.
 */
class GUIManager {
public:
	enum Action {CLICK, ACTION_COUNT};
public:
    /**
     * @param services - pointer to service locator object.
     */
    GUIManager(ServiceLocator* services);
    ~GUIManager();

    /**
     * @param node - pointer to root node
     * of the scene hierarchy.
     */
    void setRootNode(Node* node);

    /**
     * Update GUI.
     */
    void update();

	/**
	 * @return Currently selected surface.
	 * 0 is returned if no surface is selected.
	 */
	GUISurface* getSelectedSurface();
private:
    ServiceLocator* services_;
    Node* rootNode_;
	GUISurface* selectedSurface_;
    vector<Node*> nodes_;
};

#endif
