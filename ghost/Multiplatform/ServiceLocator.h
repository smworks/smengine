/*
 * ServiceLocator.h
 *
 *  Created on: 2012.06.19
 *      Author: Martynas Šustavičius
 */

#ifndef SERVICELOCATOR_H_
#define SERVICELOCATOR_H_

#include "../Multiplatform/Ghost.h"

class ScenarioManager;
class Socket;
#include "../Network/SocketParams.h"
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


	void setScreenWidth(int width);
	void setScreenHeight(int height) ;
	int	getScreenWidth();
	int getScreenHeight();
	void exit();

	/**
	 * Returns true if program was
	 * closed externaly or
	 * method exit was called.
	 * @return true if program needs to be closed.
	 */
	virtual bool isFinished();
	void disableGui();
	bool isGuiAvailable();

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
	virtual UINT32 getCurrentThreadId() = 0;
	virtual Socket* createSocket(SocketParams sp) = 0;
	virtual GraphicsManager* getGraphicsManager() = 0;
	virtual FileManager* getFileManager() = 0;
	virtual SoundManager* getSoundManager() = 0;
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

	int screenWidth;
	int screenHeight;
	bool programClosed;
	bool guiAvailable;
};

#endif
