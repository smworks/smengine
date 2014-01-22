/*
 * Engine.h
 *
 *  Created on: 2012.06.19
 *      Author: MS
 */

#ifndef ENGINE_H_
#define ENGINE_H_

#include "Multiplatform/Ghost.h"
class Node;
class ServiceLocator;

class Engine {
public:
	/**
	 * Boolean operator used to
	 * check if engine object is valid.
	 */
	operator void*();

	Engine(ServiceLocator* services);
	~Engine();

	/**
	 * Starts loading new scene in background.
	 */
	void loadScene();

	/**
	 * Method called every frame.
	 */
	void computeFrame();

	/**
	 * Resumes engine if it was paused.
	 */
	void resume();

	/**
	 * Pauses engine if it is running.
	 */
	void pause();

	/**
	 * Update node logic down the hierarchy.
	 * recalculate all node matrices that belong to this node.
	 * @param node - node that will be updated.
	 */
	void updateNodes(Node* node, bool force = false);

	/**
	 * This method is called when screen is resized.
	 * @param width - new screen width.
	 * @param height - new screen height.
	 */
	void resizeScreen(UINT32 width, UINT32 height);

	/**
	 * Informs all resources that screen size has changed.
	 * @param node - pointer to parent scene node.
	 */
	void resizeResources(Node* node);

	/**
	 * @return Pointer to service locator.
	 */
	ServiceLocator* getServiceLocator();
private:
	ServiceLocator* services_;
	bool error_;
	double time_;
	bool running_;
};

#endif /* ENGINE_H_ */
