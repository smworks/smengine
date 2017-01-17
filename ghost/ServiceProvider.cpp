#include "ServiceProvider.h"
#include "Multiplatform/ServiceLocator.h"

ServiceProvider::ServiceProvider(ServiceLocator* serviceLocator): serviceLocator(serviceLocator)
{
}

ServiceLocator* ServiceProvider::getServiceLocator() const
{
	return serviceLocator;
}

GraphicsManager* ServiceProvider::getGraphicsManager() const
{
	return serviceLocator->getGraphicsManager();
}

FileManager* ServiceProvider::getFileManager() const
{
	return serviceLocator->getFileManager();
}

SoundManager* ServiceProvider::getSoundManager() const
{
	return serviceLocator->getSoundManager();
}

Database* ServiceProvider::getDatabase() const
{
	return serviceLocator->getDB();
}

Camera* ServiceProvider::getCamera() const
{
	return serviceLocator->getCamera();
}

Environment* ServiceProvider::getEnvironment() const
{
	return serviceLocator->getEnv();
}

GUIManager* ServiceProvider::getGUIManager() const
{
	return serviceLocator->getGUIManager();
}

Input* ServiceProvider::getInput() const
{
	return serviceLocator->getInput();
}

Node* ServiceProvider::getRootNode() const
{
	return serviceLocator->getRootNode();
}

PhysicsManager* ServiceProvider::getPhysicsManager() const
{
	return serviceLocator->getPhysicsManager();
}

ResourceManager* ServiceProvider::getResourceManager() const
{
	return serviceLocator->getRM();
}

ScenarioManager* ServiceProvider::getScenarioManager() const
{
	return serviceLocator->getScenarioManager();
}

SceneManager* ServiceProvider::getSceneManager() const
{
	return serviceLocator->getSceneManager();
}

ScriptManager* ServiceProvider::getScriptManager() const
{
	return serviceLocator->getScriptManager();
}

TextManager* ServiceProvider::getTextManager() const
{
	return serviceLocator->getTextManager();
}

TextureAtlas* ServiceProvider::getTextureAtlas() const
{
	return serviceLocator->getTextureAtlas();
}

ThreadManager* ServiceProvider::getThreadManager() const
{
	return serviceLocator->getThreadManager();
}

NetworkManager* ServiceProvider::getNetworkManager() const
{
	return serviceLocator->getNetworkManager();
}

NodeManager* ServiceProvider::getNodeManager() const
{
	return serviceLocator->getNodeManager();
}
