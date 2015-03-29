/*
 * Engine.cpp
 *
 *  Created on: 2012.06.19
 *      Author: Martynas Šustavičius
 */

#include "Engine.h"
#include "Multiplatform/Ghost.h"
#include "Multiplatform/ServiceLocator.h"
#include "Multiplatform/GraphicsManager.h"
#include "Multiplatform/SoundManager.h"
#include "Multiplatform/Database.h"
#include "PhysicsManager.h"
#include "ResourceManager.h"
#include "Node.h"
#include "Resources/Shader.h"
#include "Xml.h"
#include "ResourceManager.h"
#include "Resources/TextureRGBA.h"
#include "Resources/AtlasTexture.h"
#include "Resources/Sprite.h"
#include "Input.h"
#include "Environment.h"
#include "ScriptManager.h"
#include "TextManager.h"
#include "ThreadManager.h"
#include "Network/NetworkManager.h"
#include "Network/HttpRequest.h"
#include "Resources/Animatable.h"
#include "Camera.h"
#include "GUIManager.h"
#include "Resources/GUIText.h"
#include "TextureAtlas.h"
#include "ScenarioManager.h"
#include "SceneManager.h"
#include "NodeManager.h"
#include "Console.h"
#include "Thread.h"

mutex* Engine::mut = 0;
queue<Task*>* Engine::tasks = 0;

Engine::operator void*() {
	return error_ ? 0 : this;
}

Engine::Engine(ServiceLocator* services) :
	services_(services),
	error_(false),
	time_(0.0f),
	running_(false),
	console(0)
#ifdef SMART_DEBUG_TEXT
	,debugText_(0),
	debugNode_(0),
	debugFpsCount_(0),
	debugTime_(0.0)
#endif

{
	mut = new mutex();
	tasks = new queue<Task*>();
//    LOGD("Size of integer: %u", (UINT32) sizeof(int));
//    LOGD("Size of long: %u", (UINT32) sizeof(long));
//    LOGD("Size of float: %u", (UINT32) sizeof(float));
//    LOGD("Size of char: %u", (UINT32) sizeof(char));
//    LOGD("Size of short: %u", (UINT32) sizeof(short));
//	LOGD("Size of UINT8: %u", (UINT32) sizeof(UINT8));
//	LOGD("Size of UINT16: %u", (UINT32) sizeof(UINT16));
//	LOGD("Size of UINT32: %u", (UINT32) sizeof(UINT32));
//	LOGD("Size of UINT64: %u", (UINT32) sizeof(UINT64));
//	LOGD("Size of SIZE: %u", (UINT32) sizeof(SIZE));
//	LOGD("Size of POINTER: %u", (UINT32) sizeof(POINTER));
	PROFILE("Started creating engine object.");
	// Provide services that do not depend on others at this point.
	services_->provide(NEW ScriptManager());
	services_->provide(NEW Input());
	services_->provide(NEW Environment());
	services_->provide(NEW Camera(services_->getDB()));
	services_->provide(NEW NodeManager(services_));
	services_->provide(NEW SceneManager(services_));
	services_->provide(NEW ScenarioManager(services_));
	services_->provide(NEW PhysicsManager(services_));
	services_->provide(NEW ResourceManager(services_));
	services_->provide(NEW TextManager(services_));
	services_->provide(NEW GUIManager(services_));
	services_->provide(NEW ThreadManager(services_));
	services_->provide(NEW NetworkManager(services_));
	// Provide or initialize other resourses.
	services_->getGraphicsManager()->create();
	services_->provide(NEW TextureAtlas(services_));
	services_->getPhysicsManager()->setGraphicsManager(services_->getGraphicsManager());
	services_->getDB()->setScene("start.xml");
	services_->getScriptManager()->initialize(services_);
	PROFILE("Finished creating engine object.");
	loadScene();
	time_ = 0;
	console = new Console(services_);
}

Engine::~Engine() {
	delete console;
	services_->getScriptManager()->quit();
	services_->release();
	delete services_;
#ifdef SMART_DEBUG_TEXT
    debugNode_ = 0;
#endif
	delete tasks;
	delete mut;
}

void Engine::loadScene() {
    PROFILE("Started loading scene.");
	services_->getSoundManager()->reset();
	services_->getPhysicsManager()->reset();
	if (services_->getRootNode() != 0) {
		delete services_->getRootNode();
	}
	services_->getThreadManager()->joinAll();
	services_->getDB()->setSceneLoaded(true);
	// Load scene hierarchy.
	services_->provide(services_->getRM()->loadScene(
		GHOST_SCENES + services_->getDB()->getScene()));
	if (services_->getRootNode() == 0) {
		LOGE("Error loading scene. Root node is null.");
		error_ = true;
		return;
	}
#ifdef SMART_DEBUG_TEXT
	// FPS message creation.
    if (debugNode_ == 0) {
        debugNode_ = NEW Node;
        debugNode_->setName("fps");
        debugText_ = NEW GUIText(services_);
        debugText_->setNode(debugNode_);
        debugText_->getAttributes().setString(GUIText::ATTR_WIDTH, "512px");
        debugText_->getAttributes().setString(GUIText::ATTR_HEIGHT, "32px");
        debugText_->getAttributes().setString(GUIText::ATTR_TEXT, "fps");
        debugText_->getAttributes().setString(GUIText::ATTR_COLOR, "#FFFF00FF");
        debugText_->getAttributes().setString(GUIText::ATTR_BACKGROUND, "#00000088");
        debugText_->getAttributes().setString(GUIText::ATTR_SIZE, "12px");
        debugText_->getAttributes().setString(GUIText::ATTR_SCREEN_LEFT, "true");
        debugText_->getAttributes().setString(GUIText::ATTR_SCREEN_TOP, "true");
        debugText_->create();
        services_->getRM()->add("debug text", debugText_);
        debugNode_->addResource(debugText_);
        debugNode_->setState(Node::RENDERABLE, true);
        services_->getRootNode()->addChild(debugNode_);
    }
#endif
	// Other data creation.
	services_->getGUIManager()->refreshNodes(services_->getRootNode());
	services_->getScriptManager()->start();
	resizeResources(services_->getRootNode());
    PROFILE("Finished loading scene.")
}


void Engine::computeFrame() {
	if (!running_) {
		return;
	}
	getMutex().lock();
    services_->updateTimer(
		services_->getDB()->getFloat(Database::FRAME_DURATION));
    time_ = services_->getFrameTime();
	if (error_) {
		return;
	}
#ifdef SMART_DEBUG_TEXT
	debugFpsCount_++;
	debugTime_ += time_;
	if (debugTime_ >= 1000.0f) {
		stringstream ss;
		ss << "FPS: " << debugFpsCount_ << fixed << setprecision(3) << "\n"
			<< "Resolution: " << services_->getScreenWidth()
			<< "x" << services_->getScreenHeight() << "px\n";
        debugText_->setText(ss.str());
		debugFpsCount_ = 0;
		debugTime_ = 0.0f;
	}
#endif
	services_->getInput()->update();
	if (services_->getInput()->keyReleased(Input::TILDE)) {
		console->show();
	}
	services_->getSoundManager()->update();
	services_->getPhysicsManager()->update(time_);
	services_->getThreadManager()->update();
	services_->getScriptManager()->update();
	services_->getCamera()->update(time_);
	updateNodes(services_->getRootNode());
    services_->getGUIManager()->update();
	if (getServiceLocator()->isGuiAvailable())
		services_->getGraphicsManager()->render();
	executeTasks();
	getMutex().unlock();
}
void Engine::resume() {
	running_ = true;
	services_->getScriptManager()->resume();
}

void Engine::pause() {
	services_->getScriptManager()->pause();
	running_ = false;
}

void Engine::updateNodes(Node* node, bool force) {
	if (!node->getState(Node::UPDATABLE)) {
		return;
	}
	static Mat4 tempMatrix;
	if (node->hasResource(Resource::DYNAMIC_OBJECT)) {
		Resource* res = node->getResource(Resource::DYNAMIC_OBJECT);
		Animatable* anim = dynamic_cast<Animatable*>(res);
		anim->nextFrame(time_);
	}
	bool update = force;
	if (node->getParent() != 0 && node->getState(Node::TRANSFORMABLE)) {
		if (node->getState(Node::POSITION)) { // Position update.
			Matrix::translate(
				node->getMatPos(), node->getPos().getX(),
				node->getPos().getY(), node->getPos().getZ());
			update = true;
		}
		if (node->getState(Node::ROTATION)) { // Rotation update.
			Matrix::copy(node->getRot().getMatrix(), node->getMatRot());
			update = true;
		}
		if (node->getState(Node::SCALING)) { // Scaling update.
			Matrix::scale(
				node->getMatScale(), node->getScale().getX(),
				node->getScale().getY(), node->getScale().getZ());
			update = true;
		}
		if (update) { // Combined matrix update.
			Matrix::multiply(
				node->getMatPos(), node->getMatRot(), node->getMatrix());
			Matrix::multiply(
				node->getMatrix(), node->getMatScale(), tempMatrix);
			Matrix::multiply(node->getParent()->getMatrix(),
				tempMatrix, node->getMatrix());
			node->setState(Node::POSITION, false);
			node->setState(Node::ROTATION, false);
			node->setState(Node::SCALING, false);
		}
	}
	// Children update.
	const vector<Node*>& children = node->getChildren();
	vector<Node*>::const_iterator it = children.begin();
	while (it != children.end()) {
		updateNodes((*it), update);
		it++;
	}
}

void Engine::resizeScreen(UINT32 width, UINT32 height) {
	PROFILE("Resizing window. New width=%d, height=%d.", width, height);
	if (width == 0 || height == 0) {
		return;
	}
	services_->setScreenWidth(width);
	services_->setScreenHeight(height);
	services_->getCamera()->updateProjections(width, height,
		services_->getDB()->getFloat(Database::FIELD_OF_VIEW),
		services_->getDB()->getFloat(Database::NEAR_PLANE_DISTANCE),
		services_->getDB()->getFloat(Database::FAR_PLANE_DISTANCE));
	if (services_->isGuiAvailable())
		services_->getGraphicsManager()->resize(width, height);
	resizeResources(services_->getRootNode());
	services_->getScriptManager()->resize();
#ifdef SMART_DEBUG_TEXT
	debugTime_ = 1001.0f;
#endif
    PROFILE("Window resized.");
}

void Engine::resizeResources(Node* node) {
	if (node == 0) {
		return;
	}
	const vector<Resource*>& res = node->getResources();
	for (UINT32 i = 0; i < res.size(); i++) {
		res[i]->resize();
	}
	const vector<Node*>& children = node->getChildren();
	vector<Node*>::const_iterator it = children.begin();
	while (it != children.end()) {
		resizeResources((*it));
		it++;
	}
}

ServiceLocator* Engine::getServiceLocator() {
	return services_;
}

mutex& Engine::getMutex() {
	static mutex mut;
	return mut;
}

void Engine::executeOnMainThread(Task* task) {
	mut->lock();
	tasks->push(task);
	mut->unlock();
}

void Engine::executeTasks() {
	mut->lock();
	while (!tasks->empty()) {
		Task* task = tasks->front();
		task->run();
		delete task;
		tasks->pop();
	}
	mut->unlock();
}