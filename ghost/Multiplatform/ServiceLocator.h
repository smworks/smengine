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
class ScriptManager;
class Settings;
class SoundManager;
class TextManager;
class TextureAtlas;
class ThreadManager;
class NetworkManager;

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

	/**
	 * Sets pointer to camera.
	 * @param camera - pointer to camera object.
	 */
	void provide(Camera* camera);

	/**
	 * @return Pointer to camera object, or 0 if no pointer was set.
	 */
	Camera* getCamera();

	/**
	 * Sets pointer to environment object.
	 * @param environment - pointer to environment object.
	 */
	void provide(Environment* environment);

	/**
	 * @return Pointer to environment object.
	 */
	Environment* getEnv();

	/**
	 * Sets pointer to GUI manager object.
	 * @param guiManager - pointer to GUI manager object.
	 */
	void provide(GUIManager* guiManager);

	/**
	 * @return Pointer to GUI manager object.
	 */
	GUIManager* getGUIManager();

	/**
	 * Sets up input manager.
	 * @param input - object of type Input.
	 */
	void provide(Input* input);

	/**
	 * @return Pointer to input object.
	 */
	Input* getInput();

	/**
	 * Sets up root node.
	 * @param rootNode - pointer to root node.
	 */
	void provide(Node* rootNode);

	/**
	 * @return Pointer to root node.
	 */
	Node* getRootNode();

	/**
	 * Sets up physics manager.
	 * @param physicsManager - object of type PhysicsManger.
	 */
	void provide(PhysicsManager* physicsManager);

	/**
	 * @return Pointer to physics manager object.
	 */
	PhysicsManager* getPM();

	/**
	 * Sets pointer to resource manager.
	 * @param resourceManager - pointer to resource manager.
	 */
	void provide(ResourceManager* resourceManager);

	/**
	 * @return Pointer to resource manager object.
	 */
	ResourceManager* getRM();

	/**
	 * Sets pointer to script manager.
	 * @param sm - pointer to new script manager.
	 */
	void provide(ScriptManager* sm);

	/**
	 * @return Pointer to script manager object.
	 */
	ScriptManager* getScriptManager();

	/**
	 * Sets pointer to settings object.
	 * @param settings - pointer to settings object.
	 */
	void provide(Settings* settings);

	/**
	 * @return Pointer to settings object.
	 */
	Settings* getSettings();

	/**
	 * Sets pointer to sound manager object.
	 * @param soundManager - pointer to sound manager object.
	 */
	void provide(SoundManager* soundManager);

	/**
	 * @return Pointer to sound manager object.
	 */
	SoundManager* getSM();

	/**
	 * Sets pointer to text manager object.
	 * @param textManager - pointer to text manager object.
	 */
	void provide(TextManager* textManager);

	/**
	 * @return Pointer to text manager object or, 0 if no pointer was set.
	 */
	TextManager* getTextManager();

	/**
	 * Sets pointer to texture atlas object.
	 * @param textureAtlas - pointer to texture atlas object.
	 */
	void provide(TextureAtlas* textureAtlas);

	/**
	 * @return Pointer to texture atlas object or, 0 if no pointer was set.
	 */
	TextureAtlas* getTextureAtlas();

	/**
	 * Sets pointer to thread manager object.
	 * @param threadManager - pointer to thread manager object.
	 */
	void provide(ThreadManager* threadManager);

	/**
	 * @return Pointer to thread manager object or, 0 if no pointer was set.
	 */
	ThreadManager* getTM();

	/**
	 * Sets pointer to network manager object.
	 * @param networkManager - pointer to network manager object.
	 */
	void provide(NetworkManager* networkManager);

	/**
	 * @return Pointer to network manager object or, 0 if no pointer was set.
	 */
	NetworkManager* getNM();

private:
	Camera* camera_;
	Environment* environment_;
	GUIManager* guiManager_;
	Input* input_;
	Node* rootNode_;
	PhysicsManager* physicsManager_;
	ResourceManager* resourceManager_;
	ScriptManager* scriptManager_;
	Settings* settings_;
	SoundManager* soundManager_;
	TextManager* textManager_;
	TextureAtlas* textureAtlas_;
	ThreadManager* threadManager_;
	NetworkManager* networkManager_;
};

#endif
