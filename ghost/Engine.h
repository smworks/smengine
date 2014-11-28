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

#ifdef SMART_DEBUG
	//#define SMART_DEBUG_TEXT
#endif

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

#ifdef SMART_DEBUG
//#define INIT_TIMER(name) \
//	UINT64 g_##name = 0;\
//	UINT64 g_##name##Tmp = 0;\
//	UINT64 g_##name##Min = UINT_MAX;\
//	UINT64 g_##name##Max = 0;
//#define MEASURE_BEFORE_TIMER(name) \
//	g_##name##Tmp = getMicroseconds();
//#define MEASURE_AFTER_TIMER(name) \
//	g_##name##Tmp = (getMicroseconds() - g_##name##Tmp);\
//	if (g_##name##Tmp > g_##name##Max) { g_##name##Max = g_##name##Tmp; }\
//	if (g_##name##Tmp < g_##name##Min) { g_##name##Min = g_##name##Tmp; }\
//	g_##name += g_##name##Tmp;\
//
//#define DISPLAY_TIMER(name) \
//	#name": " << (double) g_##name / (double) g_fpsCount * 0.001 <<\
//	" (min: " << (double) g_##name##Min * 0.001 <<\
//	", max: " << (double) g_##name##Max * 0.001 << ")\n"
//#define RESET_TIMER(name) g_##name = 0; g_##name##Min = UINT_MAX; g_##name##Max = 0;
//	//UINT64 startInput = getMicroseconds();
//	//	services_->getInput()->update();
//	//	g_inputTime += (getMicroseconds() - startInput);
//	UINT32 g_fpsCount;
//	INIT_TIMER(Frame)
//	INIT_TIMER(Physics)
//	INIT_TIMER(Render)
//	UINT64 g_renderModelTime;
//	UINT64 g_renderSpriteTime;
//	UINT64 g_renderTextTime;
//	INIT_TIMER(Input)
//	INIT_TIMER(Scripts)
//	INIT_TIMER(Nodes)
//	INIT_TIMER(Camera)
//	INIT_TIMER(GUI)
//	INIT_TIMER(Sounds)
//	UINT64 g_tmTime;
//	double g_time;
//	Sprite* g_monoAtlas;
//	Node* g_monoNode;
//	Sprite* g_rgbaAtlas;
//	Node* g_rgbaNode;
	GUIText* debugText_;
	Node* debugNode_;
	UINT32 debugFpsCount_;
	double debugTime_;
#endif
};

#endif /* ENGINE_H_ */
