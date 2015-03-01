/*
 * ServiceLocator.cpp
 *
 *  Created on: 2014.01.16
 *      Author: Martynas Šustavičius
 */

#include "ServiceLocator.h"
#include "../Camera.h"
#include "../Environment.h"
#include "../GUIManager.h"
#include "../Input.h"
#include "../Node.h"
#include "../PhysicsManager.h"
#include "../ResourceManager.h"
#include "../SceneManager.h"
#include "../ScenarioManager.h"
#include "../ScriptManager.h"
#include "../TextManager.h"
#include "../TextureAtlas.h"
#include "../ThreadManager.h"
#include "../NetworkManager.h"
#include "../NodeManager.h"

ServiceLocator::ServiceLocator() :
	camera_(0),
	environment_(0),
	guiManager_(0),
	input_(0),
	rootNode_(0),
	physicsManager_(0),
	resourceManager_(0),
	scenarioManager_(0),
	sceneManager_(0),
	scriptManager_(0),
	textManager_(0),
	textureAtlas_(0),
	threadManager_(0),
	networkManager_(0),
	nodeManager_(0),
	screenWidth(0),
	screenHeight(0),
	programClosed(false),
	guiAvailable(true)
{
	LOGD("Service locator created.");
}

ServiceLocator::ServiceLocator(const ServiceLocator& services) {
    LOGE("ServiceLocator class does not support copy constructor.");
}

const ServiceLocator& ServiceLocator::operator=(const ServiceLocator& services) {
    LOGE("Assignment operation is not supported for ServiceLocator.");
    return services;
}

void ServiceLocator::setScreenWidth(int width) {
	screenWidth = width;
}

void ServiceLocator::setScreenHeight(int height) {
	screenHeight = height;
}

int ServiceLocator::getScreenWidth() {
	return screenWidth;
}

int ServiceLocator::getScreenHeight() {
	return screenHeight;
}

void ServiceLocator::exit() {
	programClosed = true;
}

bool ServiceLocator::isFinished() {
	return programClosed;
}

void ServiceLocator::disableGui() {
	guiAvailable = false;
}

bool ServiceLocator::isGuiAvailable() {
	return guiAvailable;
}

ServiceLocator::~ServiceLocator() {
	LOGD("Service locator deleted.");
}

void ServiceLocator::release() {
	if (threadManager_ != 0) { // Must be deleted first.
		delete threadManager_;
		threadManager_ = 0;
	}
	if (scriptManager_ != 0) {
		delete scriptManager_;
		scriptManager_ = 0;
	}
	if (sceneManager_ != 0) {
		delete sceneManager_;
		sceneManager_ = 0;
	}
	if (camera_ != 0) {
		delete camera_;
		camera_ = 0;
	}
	if (environment_ != 0) {
		delete environment_;
		environment_ = 0;
	}
	if (guiManager_ != 0) {
        delete guiManager_;
        guiManager_ = 0;
	}
	if (input_ != 0) {
		delete input_;
		input_ = 0;
	}
	if (physicsManager_ != 0) {
		delete physicsManager_;
		physicsManager_ = 0;
	}
	if (resourceManager_ != 0) {
		delete resourceManager_;
		resourceManager_ = 0;
	}
	if (textManager_ != 0) {
		delete textManager_;
		textManager_ = 0;
	}
	if (textureAtlas_ != 0) {
        delete textureAtlas_;
        textureAtlas_ = 0;
	}
	if (networkManager_ != 0) {
		delete networkManager_;
		networkManager_ = 0;
	}
	if (scenarioManager_ != 0) {
		delete scenarioManager_;
		scenarioManager_ = 0;
	}
	if (nodeManager_ != 0) {
		delete nodeManager_;
		nodeManager_ = 0;
	}
	if (rootNode_ != 0) {
		delete rootNode_;
		rootNode_ = 0;
		LOGD("Node hierarchy deleted.");
	}
}

void ServiceLocator::provide(Camera* camera) {
	camera_ = camera;
}

Camera* ServiceLocator::getCamera() {
	return camera_;
}

void ServiceLocator::provide(Environment* environment) {
	environment_ = environment;
}

Environment* ServiceLocator::getEnv() {
	return environment_;
}

void ServiceLocator::provide(GUIManager* guiManager) {
	guiManager_ = guiManager;
}

GUIManager* ServiceLocator::getGUIManager() {
	return guiManager_;
}

void ServiceLocator::provide(Input* input) {
	input_ = input;
}

Input* ServiceLocator::getInput() {
	return input_;
}

void ServiceLocator::provide(Node* rootNode) {
	rootNode_ = rootNode;
}

Node* ServiceLocator::getRootNode() {
	return rootNode_;
}

void ServiceLocator::provide(PhysicsManager* physicsManager) {
	physicsManager_ = physicsManager;
}

PhysicsManager* ServiceLocator::getPhysicsManager() {
	return physicsManager_;
}

void ServiceLocator::provide(ResourceManager* resourceManager) {
	resourceManager_ = resourceManager;
}

ResourceManager* ServiceLocator::getRM() {
	return resourceManager_;
}

void ServiceLocator::provide(ScenarioManager* scenarioManager) {
	scenarioManager_ = scenarioManager;
}

ScenarioManager* ServiceLocator::getScenarioManager() {
	return scenarioManager_;
}

void ServiceLocator::provide(SceneManager* sceneManager) {
	sceneManager_ = sceneManager;
}

SceneManager* ServiceLocator::getSceneManager() {
	return sceneManager_;
}

void ServiceLocator::provide(ScriptManager* sm) {
	scriptManager_ = sm;
}

ScriptManager* ServiceLocator::getScriptManager() {
	return scriptManager_;
}

void ServiceLocator::provide(TextManager* textManager) {
	textManager_ = textManager;
}

TextManager* ServiceLocator::getTextManager() {
	return textManager_;
}

void ServiceLocator::provide(TextureAtlas* textureAtlas) {
	textureAtlas_ = textureAtlas;
}

TextureAtlas* ServiceLocator::getTextureAtlas() {
	return textureAtlas_;
}

void ServiceLocator::provide(ThreadManager* threadManager) {
	threadManager_ = threadManager;
}

ThreadManager* ServiceLocator::getThreadManager() {
	return threadManager_;
}

void ServiceLocator::provide(NetworkManager* networkManager) {
	networkManager_ = networkManager;
}

NetworkManager* ServiceLocator::getNetworkManager() {
	return networkManager_;
}

void ServiceLocator::provide(NodeManager* nodeManager) {
	nodeManager_ = nodeManager;
}

NodeManager* ServiceLocator::getNodeManager() {
	return nodeManager_;
}
