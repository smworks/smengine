#ifndef SERVICEPROVIDER_H_
#define SERVICEPROVIDER_H_

class ServiceLocator;
class GraphicsManager;
class FileManager;
class SoundManager;
class Database;
class Camera;
class Environment;
class GUIManager;
class Input;
class Node;
class PhysicsManager;
class ResourceManager;
class ScenarioManager;
class SceneManager;
class ScriptManager;
class TextManager;
class TextureAtlas;
class ThreadManager;
class NetworkManager;
class NodeManager;

class ServiceProvider
{
public:
	explicit ServiceProvider(ServiceLocator* serviceLocator);
    ServiceLocator* getServiceLocator() const;
protected:
	GraphicsManager* getGraphicsManager() const;
	FileManager* getFileManager() const;
	SoundManager* getSoundManager() const;
	Database* getDatabase() const;
	Camera* getCamera() const;
	Environment* getEnvironment() const;
	GUIManager* getGUIManager() const;
	Input* getInput() const;
	Node* getRootNode() const;
	PhysicsManager* getPhysicsManager() const;
	ResourceManager* getResourceManager() const;
	ScenarioManager* getScenarioManager() const;
	SceneManager* getSceneManager() const;
	ScriptManager* getScriptManager() const;
	TextManager* getTextManager() const;
	TextureAtlas* getTextureAtlas() const;
	ThreadManager* getThreadManager() const;
	NetworkManager* getNetworkManager() const;
	NodeManager* getNodeManager() const;
private:
	ServiceLocator* serviceLocator;
};

#endif
