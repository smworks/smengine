/*
 * ServiceLocator.h
 *
 *  Created on: 2012.06.19
 *      Author: Martynas Šustavičius
 */

#ifndef SERVICELOCATOR_H_
#define SERVICELOCATOR_H_

#include "../Multiplatform/Ghost.h"

class Thread;
class ScenarioManager;
class Socket;
class SoundManager;
class GraphicsManager;
class FileManager;
class Camera;
class Environment;
class GUIManager;
class Input;
class Node;
class PhysicsManager;
class RenderManager;
class ResourceManager;
class SceneManager;
class ScriptManager;
class SoundManager;
class TextManager;
class TextureAtlas;
class ThreadManager;
class NetworkManager;
class NodeManager;
class Database;

class ServiceLocator {
public:
	/**
	 * Returns time value in milliseconds.
	 * @return Time frame took to execute.
	 */
	virtual double getFrameTime() = 0;

	/**
	 * Sets new screen width value.
	 * @param width - new screen width in pixels.
	 */
	virtual void setScreenWidth(int width) = 0;

	/**
	 * Sets new screen height value.
	 * @param height - new screen height in pixels.
	 */
	virtual void setScreenHeight(int height) = 0;

	/**
	 * @return Current screen width in pixels.
	 */
	virtual int	getScreenWidth() = 0;

	/**
	 * @return Current screen height in pixels.
	 */
	virtual int getScreenHeight() = 0;

	/**
	 * Sets program quit indicator to true.
	 * Doesn't quit immedietly.
	 */
	virtual void exit() = 0;

	/**
	 * Returns true if program was
	 * closed externaly or
	 * method exit was called.
	 * @return true if program needs to be closed.
	 */
	virtual bool isFinished() = 0;

	/**
	 * @param sleep - optional parameter,
	 * makes sure that all program is
	 * sent into sleep mode until
	 * atleast the specified amount
	 * of time has elapsed.
	 * @return Amount of time that has
	 * passed since last call to this
	 * method.
	 */
	virtual double updateTimer(float sleep = 0.0f) = 0;

	/**
	 * Return thread instance.
	 * @return Pointer to thread.
	 */
	virtual Thread* createThread() = 0;

	/**
	 * Returns unique index for thread calling this method.
	 * @return Unique index.
	 */
	virtual UINT32 getCurrentThreadId() = 0;

	/**
	 * Return socket instance.
	 * @return Pointer to socket object.
	 */
	virtual Socket* createSocket() = 0;

	/**
	 * Return graphics manager instance.
	 * @return Pointer to graphics manager.
	 */
	virtual GraphicsManager* getGraphicsManager() = 0;

	/**
	 * Return file manager instance.
	 * @return Pointer to file manager.
	 */
	virtual FileManager* getFileManager() = 0;

	/**
	 * Return sound manager instance.
	 * @return Pointer to sound manager.
	 */
	virtual SoundManager* getSoundManager() = 0;

	/**
	 * Return database instance.
	 * @return Pointer to database.
	 */
	virtual Database* getDB() = 0;
public:
	ServiceLocator();
	ServiceLocator(const ServiceLocator& services);
	const ServiceLocator& operator=(const ServiceLocator& services);
	virtual ~ServiceLocator();

	/**
	 * Must be called before deletion, because
	 * some resources that are freed in deletion
	 * require ServiceLocator functions that are virtual
	 * and cannot be called in destructor.
	 */
	void release();

	void provide(Camera* camera);
	Camera* getCamera();
	void provide(Environment* environment);
	Environment* getEnv();
	void provide(GUIManager* guiManager);
	GUIManager* getGUIManager();
	void provide(Input* input);
	Input* getInput();
	void provide(Node* rootNode);
	Node* getRootNode();
	void provide(PhysicsManager* physicsManager);
	PhysicsManager* getPhysicsManager();
	void provide(ResourceManager* resourceManager);
	ResourceManager* getRM();
	void provide(ScenarioManager* scenarioManager);
	ScenarioManager* getScenarioManager();
	void provide(SceneManager* sceneManager);
	SceneManager* getSceneManager();
	void provide(ScriptManager* scriptManager);
	ScriptManager* getScriptManager();
	void provide(TextManager* textManager);
	TextManager* getTextManager();
	void provide(TextureAtlas* textureAtlas);
	TextureAtlas* getTextureAtlas();
	void provide(ThreadManager* threadManager);
	ThreadManager* getThreadManager();
	void provide(NetworkManager* networkManager);
	NetworkManager* getNetworkManager();
	void provide(NodeManager* nodeManager);
	NodeManager* getNodeManager();
private:
	Camera* camera_;
	Environment* environment_;
	GUIManager* guiManager_;
	Input* input_;
	Node* rootNode_;
	PhysicsManager* physicsManager_;
	ResourceManager* resourceManager_;
	ScenarioManager* scenarioManager_;
	SceneManager* sceneManager_;
	ScriptManager* scriptManager_;
	TextManager* textManager_;
	TextureAtlas* textureAtlas_;
	ThreadManager* threadManager_;
	NetworkManager* networkManager_;
	NodeManager* nodeManager_;
};

#endif
