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
class GUIText;
class Console;
class Task;

#ifdef SMART_DEBUG
	#define SMART_DEBUG_TEXT
#endif

class Engine {
public:
	operator void*();

	Engine(ServiceLocator* services);
	~Engine();

	void loadScene();
	void computeFrame();
	void resume();
	void pause();

	/**
	 * Update node logic down the hierarchy.
	 * recalculate all node matrices that belong to this node.
	 * @param node - node that will be updated.
	 */
	void updateNodes(Node* node, bool force = false);
	void resizeScreen(UINT32 width, UINT32 height);

	/**
	 * Informs all resources that screen size has changed.
	 * @param node - pointer to parent scene node.
	 */
	void resizeResources(Node* node);
	ServiceLocator* getServiceLocator();

	static mutex& getMutex();
	static void executeOnMainThread(Task* task);
private:
	void executeTasks();

	ServiceLocator* services_;
	bool error_;
	double time_;
	bool running_;

#ifdef SMART_DEBUG
	GUIText* debugText_;
	Node* debugNode_;
	UINT32 debugFpsCount_;
	double debugTime_;
#endif
	Console* console;
	static queue<Task*>* tasks;
	static mutex* mut;
};

#endif /* ENGINE_H_ */
