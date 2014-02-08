/*
 * SceneManager.h
 *
 *  Created on: 2014.01.31
 *      Author: Martynas Šustavičius
 */

#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include "Multiplatform/Ghost.h"

class ServiceLocator;
class Node;
#define MAX_BACKGROUND_COUNT 10

class SceneManager {
public:
	struct Background {
		Node* sprite;
		float scrollRatio;
	};
public:
	/**
	 * @param system - pointer to service locator.
	 */
	SceneManager(ServiceLocator* services);
	~SceneManager();

	/**
	 * Executes all scenarios.
	 * @param delta - time delta in miliseconds.
	 */
	void update(double delta);

	/**
	 * Sets level background.
	 * @param sprite - node containing background sprite.
	 * @param index - background index in array.
	 * @param scrollRatio - specifies how fast will background
	 * scroll in regards to camera.
	 */
	void setBackground(Node* sprite, SIZE index = 0, float scrollRatio = 1.0f);

	/**
	 * @param target - node that will get camera focus.
	 */
	void setTarget(Node* target);

	/**
	 * To make game look same or similar on a variety of
	 * resolutions original scene height in pixels
	 * must be specified, so the engine could
	 * scale resources to appropriate size.
	 * @NOTE Must be specified first.
	 * @param height - original scene height.
	 */
	void setSceneHeight(SIZE height);

	/**
	 * @return Width of background at index 0.
	 */
	float getBackgroundWidth();

	/**
	 * @return Height of background at index 0.
	 */
	float getBackgroundHeight();
private:
	ServiceLocator* services_;
	Node* target_;
	Background backgrounds_[MAX_BACKGROUND_COUNT];
	SIZE sceneHeight_;
};

#endif