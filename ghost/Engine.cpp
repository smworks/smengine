﻿/*
 * Engine.cpp
 *
 *  Created on: 2012.06.19
 *      Author: Martynas Šustavičius
 */

#include "Engine.h"
#include "Multiplatform/Ghost.h"
#include "Multiplatform/ServiceLocator.h"
#include "Multiplatform/GraphicsManager.h"
#include "SoundManager.h"
#include "PhysicsManager.h"
#include "ResourceManager.h"
#include "Node.h"
#include "Resources/Shader.h"
#include "Xml.h"
#include "ResourceManager.h"
#include "Resources/TextureRGBA.h"
#include "Resources/AtlasTexture.h"
#include "Resources/Sprite.h"
#include "Settings.h"
#include "Input.h"
#include "Environment.h"
#include "ScriptManager.h"
#include "TextManager.h"
#include "ThreadManager.h"
#include "NetworkManager.h"
#include "HttpRequest.h"
#include "Resources/Animatable.h"
#include "Camera.h"
#include "GUIManager.h"
#include "Resources/GUIText.h"
#include "TextureAtlas.h"

Engine::operator void*() {
	return error_ ? 0 : this;
}

#ifdef SMART_DEBUG
GUIText* g_debugText;
Node* g_debugNode;
UINT32 g_fpsCount = 0;
UINT64 g_frameTime = 0;
UINT64 g_physicsTime = 0;
UINT64 g_renderTime = 0;
UINT64 g_renderModelTime = 0;
UINT64 g_renderSpriteTime = 0;
UINT64 g_renderTextTime = 0;
UINT64 g_inputTime = 0;
UINT64 g_smTime = 0;
UINT64 g_tmTime = 0;
UINT64 g_scriptManagerTime = 0;
UINT64 g_cameraTime = 0;
UINT64 g_nodeUpdateTime = 0;
UINT64 g_guiManagerTime = 0;
double g_time = 0.0f;
Sprite* g_monoAtlas;
Node* g_monoNode;
Sprite* g_rgbaAtlas;
Node* g_rgbaNode;
#endif

Engine::Engine(ServiceLocator* services) :
	services_(services),
	error_(false),
	time_(0.0f),
	running_(false)
{
    LOGD("Size of integer: %u", (UINT32) sizeof(int));
    LOGD("Size of long: %u", (UINT32) sizeof(long));
    LOGD("Size of float: %u", (UINT32) sizeof(float));
    LOGD("Size of char: %u", (UINT32) sizeof(char));
    LOGD("Size of short: %u", (UINT32) sizeof(short));
	LOGD("Size of UINT8: %u", (UINT32) sizeof(UINT8));
	LOGD("Size of UINT16: %u", (UINT32) sizeof(UINT16));
	LOGD("Size of UINT32: %u", (UINT32) sizeof(UINT32));
	LOGD("Size of UINT64: %u", (UINT32) sizeof(UINT64));
	LOGD("Size of SIZE: %u", (UINT32) sizeof(SIZE));
	LOGD("Size of POINTER: %u", (UINT32) sizeof(POINTER));
    LOGD("Started creating engine object.");
	// Provide services that do not depend on others at this point.
	services_->provide(NEW ScriptManager());
	services_->provide(NEW Input());
	services_->provide(NEW Environment());
	services_->provide(NEW Camera());
	services_->provide(NEW SoundManager(services_));
	services_->provide(NEW PhysicsManager(services_));
	services_->provide(NEW ResourceManager(services_));
	services_->provide(NEW TextManager(services_));
	services_->provide(NEW GUIManager(services_));
	services_->provide(NEW ThreadManager(services_));
	services_->provide(NEW NetworkManager(services_));
	// Provide or initialize other resourses.
	services_->provide(NEW Settings(services_->getFileManager()));
	services_->getGraphicsManager()->create();
	services_->provide(NEW TextureAtlas(services_));
	services_->getGraphicsManager()->resize(
		services_->getScreenWidth(),
		services_->getScreenHeight());
	services_->getPM()->setGraphicsManager(services_->getGraphicsManager());
	services_->getSettings()->setScene("start.xml");
	services_->getScriptManager()->initialize(services_);
    PROFILE("Finished creating engine object.");
}

Engine::~Engine() {
	services_->getScriptManager()->quit();
	services_->release();
	delete services_;
}

void Engine::loadScene() {
    PROFILE("Started loading scene.");
	services_->getSM()->reset();
	services_->getPM()->reset();
	//services_->getRM()->reset();
	if (services_->getRootNode() != 0) {
		delete services_->getRootNode();
	}
	services_->getTM()->joinAll();
	services_->getSettings()->setSceneLoaded(true);
	// Load scene hierarchy.
	services_->provide(services_->getRM()->loadScene(
		GHOST_SCENES + services_->getSettings()->getScene()));
	//rootNode_->printNodes(">");
	if (services_->getRootNode() == 0) {
		LOGE("Error loading scene. Root node is null.");
		error_ = true;
		return;
	}
#ifdef SMART_DEBUG
	// FPS message creation.
    g_debugNode = NEW Node;
	g_debugNode->setName("fps");
	g_debugText = NEW GUIText(services_);
	g_debugText->setNode(g_debugNode);
	g_debugText->getAttributes().setString(GUIText::ATTR_WIDTH, "512px");
	g_debugText->getAttributes().setString(GUIText::ATTR_HEIGHT, "32px");
	g_debugText->getAttributes().setString(GUIText::ATTR_TEXT, "fps");
	g_debugText->getAttributes().setString(GUIText::ATTR_COLOR, "#FFFF00FF");
	g_debugText->getAttributes().setString(GUIText::ATTR_BACKGROUND, "#00000088");
	g_debugText->getAttributes().setString(GUIText::ATTR_SIZE, "12px");
	g_debugText->getAttributes().setString(GUIText::ATTR_SCREEN_LEFT, "true");
	g_debugText->getAttributes().setString(GUIText::ATTR_SCREEN_TOP, "true");
	g_debugText->create();
	services_->getRM()->add("debug text", g_debugText);
	g_debugNode->addResource(g_debugText);
    g_debugNode->setState(Node::RENDERABLE, true);
	services_->getRootNode()->addChild(g_debugNode);
	// Texture atlas node creation.
	g_monoNode = NEW Node;
	g_monoNode->setName("monoAtlasNode");
	g_monoAtlas = NEW Sprite(services_);
	g_monoAtlas->setNode(g_monoNode);
	g_monoAtlas->create();
	Texture* monoText = services_->getTextureAtlas()->getTexture(Texture::MONO);
	g_monoAtlas->addTexture(monoText);
	services_->getRM()->add("monoAtlasSprite", g_monoAtlas);
	g_monoNode->addResource(g_monoAtlas);
	g_monoNode->setState(Node::RENDERABLE, false);
	services_->getRootNode()->addChild(g_monoNode);
	g_monoNode->getScale().setXYZ(
		(float) monoText->getWidth(), (float) monoText->getHeight(), 1.0f);
	g_rgbaNode = NEW Node;
	g_rgbaNode->setName("rgbaAtlasNode");
	g_rgbaAtlas = NEW Sprite(services_);
	g_rgbaAtlas->setNode(g_rgbaNode);
	g_rgbaAtlas->create();
	Texture* rgbaText = services_->getTextureAtlas()->getTexture(Texture::RGBA);
	g_rgbaAtlas->addTexture(rgbaText);
	services_->getRM()->add("rgbaAtlasSprite", g_rgbaAtlas);
	g_rgbaNode->addResource(g_rgbaAtlas);
	g_rgbaNode->setState(Node::RENDERABLE, false);
	services_->getRootNode()->addChild(g_rgbaNode);
	g_rgbaNode->getScale().setXYZ(
		(float) rgbaText->getWidth(), (float) rgbaText->getHeight(), 1.0f);
#endif
	// Other data creation.
	services_->getGUIManager()->setRootNode(services_->getRootNode());
	services_->getScriptManager()->start();
	services_->getScriptManager()->resume();
	resizeResources(services_->getRootNode());
	glFinish();
    PROFILE("Finished loading scene.")
}


bool g_readyToLoad = false;
void Engine::computeFrame() {
	if (!running_) {
		return;
	}
	UINT64 startTime = getMicroseconds();
    services_->updateTimer(
		services_->getSettings()->getFloat(Settings::FRAME_DURATION));
    time_ = services_->getFrameTime();
#ifdef SMART_DEBUG
    if (services_->getInput()->keyReleased(Input::F)) {
        g_debugNode->setState(Node::RENDERABLE,
			!g_debugNode->getState(Node::RENDERABLE));
    }
#endif
	if (error_) {
		return;
	}
	if (!services_->getSettings()->isSceneLoaded()) {
		if (!g_readyToLoad) {
			services_->getGraphicsManager()->renderLoading();
			g_readyToLoad = true;
			return;
		}
		else {
			loadScene();
			g_readyToLoad = false;
			time_ = 0;
		}
	}
#ifdef SMART_DEBUG
	// Rezerve area for 128x128 texture in monochrome texture atlas.
	if (services_->getInput()->keyReleased(Input::B)) {
		UINT32 id;
		services_->getTextureAtlas()->create(id, 128, 128, Texture::MONO);
	}
	// Turn on/off physics debug draw.
	static bool useDebugDraw = true;
	if (services_->getInput()->keyReleased(Input::P)) {
		if (useDebugDraw) {
			services_->getPM()->setDebug(true);
			LOGD("Enabling physics debug draw.");
		}
		else {
			services_->getPM()->setDebug(false);
			LOGD("Disabling physics debug draw.");
		}
		useDebugDraw = !useDebugDraw;
	}
	// Show/hide monochrome texture atlas.
	static bool showMonoAtlas = true;
	if (services_->getInput()->keyReleased(Input::F2)) {
		if (showMonoAtlas) {
			g_monoNode->setState(Node::RENDERABLE, true);
			LOGD("Enabling mono texture atlas.");
		}
		else {
			g_monoNode->setState(Node::RENDERABLE, false);
			LOGD("Disabling mono texture atlas.");
		}
		showMonoAtlas = !showMonoAtlas;
	}
	// Show/hide rgba texture atlas.
	static bool showRgbaAtlas = true;
	if (services_->getInput()->keyReleased(Input::F3)) {
		if (showRgbaAtlas) {
			g_rgbaNode->setState(Node::RENDERABLE, true);
			LOGD("Enabling rgba texture atlas.");
		}
		else {
			g_rgbaNode->setState(Node::RENDERABLE, false);
			LOGD("Disabling rgba texture atlas.");
		}
		showRgbaAtlas = !showRgbaAtlas;
	}
	// Test socket.
	if (services_->getInput()->keyReleased(Input::F4)) {
		PROFILE("Sending data.");
		services_->getNM()->execute(NEW HttpRequest("http://i2.kym-cdn.com/photos/images/original/000/370/259/7a9.png", HttpRequest::GET));
		PROFILE("Function returned.");
	}
	UINT64 startInput = getMicroseconds();
	services_->getInput()->update();
	g_inputTime += (getMicroseconds() - startInput);
	UINT64 startSM = getMicroseconds();
	services_->getSM()->update();
	g_smTime += (getMicroseconds() - startSM);
	UINT64 startPhysics = getMicroseconds();
	services_->getPM()->update(time_);
	g_physicsTime += (getMicroseconds() - startPhysics);
	UINT64 startTM = getMicroseconds();
	services_->getTM()->update();
	g_tmTime += (getMicroseconds() - startTM);
	UINT64 startScriptManager = getMicroseconds();
	services_->getScriptManager()->update();
	g_scriptManagerTime += (getMicroseconds() - startScriptManager);
	UINT64 startCamera = getMicroseconds();
	services_->getCamera()->update(time_);
	g_cameraTime += (getMicroseconds() - startCamera);
	UINT64 startNodeUpdate = getMicroseconds();
	updateNodes(services_->getRootNode());
	g_nodeUpdateTime += (getMicroseconds() - startNodeUpdate);
	UINT64 startGUIManager = getMicroseconds();
    services_->getGUIManager()->update();
	g_guiManagerTime += (getMicroseconds() - startGUIManager);
	UINT64 startRender = getMicroseconds();
	services_->getGraphicsManager()->render();
	g_renderTime += (getMicroseconds() - startRender);
	// Output debug message.
	g_fpsCount++;
	g_time += time_;
	g_frameTime += (getMicroseconds() - startTime);
	if (g_time >= 1000.0f) {
		stringstream ss;
		double frameTime = (double) g_frameTime / (double) g_fpsCount * 0.001;
		double physicsTime = (double) g_physicsTime / (double) g_fpsCount * 0.001;
		double renderTime = (double) g_renderTime / (double) g_fpsCount * 0.001;
		double renderModelTime = (double) g_renderModelTime / (double) g_fpsCount * 0.001;
		double renderSpriteTime = (double) g_renderSpriteTime / (double) g_fpsCount * 0.001;
		double renderTextTime = (double) g_renderTextTime / (double) g_fpsCount * 0.001;
		double inputTime = (double) g_inputTime / (double) g_fpsCount * 0.001;
		double smTime = (double) g_smTime / (double) g_fpsCount * 0.001;
		double scriptManagerTime = (double) g_scriptManagerTime / (double) g_fpsCount * 0.001;
		double cameraTime = (double) g_cameraTime / (double) g_fpsCount * 0.001;
		double nodeUpdateTime = (double) g_nodeUpdateTime / (double) g_fpsCount * 0.001;
		double guiManagerTime = (double) g_guiManagerTime / (double) g_fpsCount * 0.001;
		ss << "FPS: " << g_fpsCount << fixed << setprecision(3)
			<< "\nFrame time: " << frameTime
			<< "ms\nInput: " << inputTime
			<< "ms\nSounds: " << smTime
			<< "ms\nScripts: " << scriptManagerTime
			<< "ms\nCamera: " << cameraTime
			<< "ms\nNodes: " << nodeUpdateTime
			<< "ms\nGUI: " << guiManagerTime
			<< "ms\nPhysics: " << physicsTime
			<< "ms\nRender: " << renderTime
			<< "ms\n  Models: " << renderModelTime
			<< "ms\n  Sprites: " << renderSpriteTime
			<< "ms\n  Text: " << renderTextTime
			<< "ms\nResolution: " << services_->getScreenWidth()
			<< "x" << services_->getScreenHeight() << "px\n";
		g_debugText->setText(ss.str());
		g_fpsCount = 0;
		g_time = 0.0f;
		g_frameTime = 0;
		g_physicsTime = 0;
		g_renderTime = 0;
		g_renderModelTime = 0;
		g_renderSpriteTime = 0;
		g_renderTextTime = 0;
		g_inputTime = 0;
		g_smTime = 0;
		g_tmTime = 0;
		g_scriptManagerTime = 0;
		g_cameraTime = 0;
		g_nodeUpdateTime = 0;
		g_guiManagerTime = 0;
	}
#else
	services_->getInput()->update();
	services_->getSM()->update();
	services_->getPM()->update(time_);
	services_->getTM()->update();
	services_->getScriptManager()->update();
	services_->getCamera()->update(time_);
	updateNodes(services_->getRootNode());
    services_->getGUIManager()->update();
	services_->getGraphicsManager()->render();
#endif
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
		services_->getSettings()->getFloat(Settings::FIELD_OF_VIEW),
		services_->getSettings()->getFloat(Settings::NEAR_PLANE_DISTANCE),
		services_->getSettings()->getFloat(Settings::FAR_PLANE_DISTANCE));
	services_->getGraphicsManager()->resize(width, height);
	resizeResources(services_->getRootNode());
	services_->getScriptManager()->resize();
#ifdef SMART_DEBUG
	g_time = 1001.0f;
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