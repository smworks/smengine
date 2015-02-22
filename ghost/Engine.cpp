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
#include "NetworkManager.h"
#include "HttpRequest.h"
#include "Resources/Animatable.h"
#include "Camera.h"
#include "GUIManager.h"
#include "Resources/GUIText.h"
#include "TextureAtlas.h"
#include "ScenarioManager.h"
#include "SceneManager.h"
#include "NodeManager.h"
#include "Console.h"

Engine::operator void*() {
	return error_ ? 0 : this;
}

Engine::Engine(ServiceLocator* services) :
	services_(services),
	error_(false),
	time_(0.0f),
	running_(false)
#ifdef SMART_DEBUG_TEXT
	,debugText_(0),
	debugNode_(0),
	debugFpsCount_(0),
	debugTime_(0.0)
#endif

{
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
	//services_->provide(NEW SoundManager(services_));
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
}

Engine::~Engine() {
	services_->getScriptManager()->quit();
	services_->release();
	delete services_;
#ifdef SMART_DEBUG_TEXT
    debugNode_ = 0;
#endif
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
	Console c(services_);
	//// Texture atlas node creation.
 //   if (g_monoNode == 0) {
 //       g_monoNode = NEW Node;
 //       g_monoNode->setName("monoAtlasNode");
 //       g_monoAtlas = NEW Sprite(services_);
 //       g_monoAtlas->setNode(g_monoNode);
 //       g_monoAtlas->create();
 //       Texture* monoText = services_->getTextureAtlas()->getTexture(Texture::MONO);
 //       g_monoAtlas->addTexture(monoText);
 //       services_->getRM()->add("monoAtlasSprite", g_monoAtlas);
 //       g_monoNode->addResource(g_monoAtlas);
 //       g_monoNode->setState(Node::RENDERABLE, false);
 //       services_->getRootNode()->addChild(g_monoNode);
 //       g_monoNode->getScale().setXYZ((float) monoText->getWidth(), (float) monoText->getHeight(), 1.0f);
 //   }
 //   if (g_rgbaNode == 0) {
 //       g_rgbaNode = NEW Node;
 //       g_rgbaNode->setName("rgbaAtlasNode");
 //       g_rgbaAtlas = NEW Sprite(services_);
 //       g_rgbaAtlas->setNode(g_rgbaNode);
 //       g_rgbaAtlas->create();
 //       Texture* rgbaText = services_->getTextureAtlas()->getTexture(Texture::RGBA);
 //       g_rgbaAtlas->addTexture(rgbaText);
 //       services_->getRM()->add("rgbaAtlasSprite", g_rgbaAtlas);
 //       g_rgbaNode->addResource(g_rgbaAtlas);
 //       g_rgbaNode->setState(Node::RENDERABLE, false);
 //       services_->getRootNode()->addChild(g_rgbaNode);
 //       g_rgbaNode->getScale().setXYZ(
 //           (float) rgbaText->getWidth(), (float) rgbaText->getHeight(), 1.0f);
 //   }
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
//#ifdef SMART_DEBUG
//	MEASURE_BEFORE_TIMER(Frame)
//#endif
    services_->updateTimer(
		services_->getDB()->getFloat(Database::FRAME_DURATION));
    time_ = services_->getFrameTime();
//#ifdef SMART_DEBUG
//    if (services_->getInput()->keyReleased(Input::F)) {
//        g_debugNode->setState(Node::RENDERABLE,
//			!g_debugNode->getState(Node::RENDERABLE));
//    }
//#endif
	if (error_) {
		return;
	}
#ifdef SMART_DEBUG_TEXT
	//// Rezerve area for 128x128 texture in monochrome texture atlas.
	//if (services_->getInput()->keyReleased(Input::B)) {
	//	UINT32 id;
	//	services_->getTextureAtlas()->create(id, 128, 128, Texture::MONO);
	//}
	//// Turn on/off physics debug draw.
	//static bool useDebugDraw = true;
	//if (services_->getInput()->keyReleased(Input::P)) {
	//	if (useDebugDraw) {
	//		services_->getPhysicsManager()->setDebug(true);
	//		LOGD("Enabling physics debug draw.");
	//	}
	//	else {
	//		services_->getPhysicsManager()->setDebug(false);
	//		LOGD("Disabling physics debug draw.");
	//	}
	//	useDebugDraw = !useDebugDraw;
	//}
	//// Show/hide monochrome texture atlas.
	//static bool showMonoAtlas = true;
	//if (services_->getInput()->keyReleased(Input::F2)) {
	//	if (showMonoAtlas) {
	//		g_monoNode->setState(Node::RENDERABLE, true);
	//		LOGD("Enabling mono texture atlas.");
	//	}
	//	else {
	//		g_monoNode->setState(Node::RENDERABLE, false);
	//		LOGD("Disabling mono texture atlas.");
	//	}
	//	showMonoAtlas = !showMonoAtlas;
	//}
	//// Show/hide rgba texture atlas.
	//static bool showRgbaAtlas = true;
	//if (services_->getInput()->keyReleased(Input::F3)) {
	//	if (showRgbaAtlas) {
	//		g_rgbaNode->setState(Node::RENDERABLE, true);
	//		LOGD("Enabling rgba texture atlas.");
	//	}
	//	else {
	//		g_rgbaNode->setState(Node::RENDERABLE, false);
	//		LOGD("Disabling rgba texture atlas.");
	//	}
	//	showRgbaAtlas = !showRgbaAtlas;
	//}
	//// Test socket.
	//if (services_->getInput()->keyReleased(Input::F4)) {
	//	PROFILE("Sending data.");
	//	services_->getNetworkManager()->execute(NEW HttpRequest("http://i2.kym-cdn.com/photos/images/original/000/370/259/7a9.png", HttpRequest::GET));
	//	PROFILE("Function returned.");
	//}
	//MEASURE_BEFORE_TIMER(Input)
	//services_->getInput()->update();
	//MEASURE_AFTER_TIMER(Input)
	//MEASURE_BEFORE_TIMER(Sounds)
	//services_->getSoundManager()->update();
	//MEASURE_AFTER_TIMER(Sounds)
	//MEASURE_BEFORE_TIMER(Physics)
	//services_->getPhysicsManager()->update(time_);
	//MEASURE_AFTER_TIMER(Physics)
	//services_->getThreadManager()->update();
	//MEASURE_BEFORE_TIMER(Scripts)
	//services_->getScenarioManager()->update(time_);
	//services_->getSceneManager()->update(time_);
	//services_->getScriptManager()->update();
	//MEASURE_AFTER_TIMER(Scripts)
	//MEASURE_BEFORE_TIMER(Camera)
	//services_->getCamera()->update(time_);
	//MEASURE_AFTER_TIMER(Camera)
	//MEASURE_BEFORE_TIMER(Nodes)
	//updateNodes(services_->getRootNode());
	//MEASURE_AFTER_TIMER(Nodes)
	//MEASURE_BEFORE_TIMER(GUI)
 //   services_->getGUIManager()->update();
	//MEASURE_AFTER_TIMER(GUI)
	//MEASURE_BEFORE_TIMER(Render)
	//services_->getGraphicsManager()->render();
	//MEASURE_AFTER_TIMER(Render)
	//// Output debug message.
	//MEASURE_AFTER_TIMER(Frame)
	debugFpsCount_++;
	debugTime_ += time_;
	if (debugTime_ >= 1000.0f) {
		stringstream ss;
		//double renderModelTime = (double) g_renderModelTime / (double) g_fpsCount * 0.001;
		//double renderSpriteTime = (double) g_renderSpriteTime / (double) g_fpsCount * 0.001;
		//double renderTextTime = (double) g_renderTextTime / (double) g_fpsCount * 0.001;
		ss << "FPS: " << debugFpsCount_ << fixed << setprecision(3) << "\n"
			//<< DISPLAY_TIMER(Frame)
			//<< DISPLAY_TIMER(Input)
			//<< DISPLAY_TIMER(Sounds)
			//<< DISPLAY_TIMER(Scripts)
			//<< DISPLAY_TIMER(Camera)
			//<< DISPLAY_TIMER(Nodes)
			//<< DISPLAY_TIMER(GUI)
			//<< DISPLAY_TIMER(Physics)
			//<< DISPLAY_TIMER(Render)
			//<< "  Models: " << renderModelTime
			//<< "ms\n  Sprites: " << renderSpriteTime
			//<< "ms\n  Text: " << renderTextTime << "ms\n"
			<< "Resolution: " << services_->getScreenWidth()
			<< "x" << services_->getScreenHeight() << "px\n";
        debugText_->setText(ss.str());
		debugFpsCount_ = 0;
		debugTime_ = 0.0f;
		//RESET_TIMER(Frame)
		//RESET_TIMER(Physics)
		//RESET_TIMER(Render)
		//g_renderModelTime = 0;
		//g_renderSpriteTime = 0;
		//g_renderTextTime = 0;
		//RESET_TIMER(Input)
		//RESET_TIMER(Sounds)
		//g_tmTime = 0;
		//RESET_TIMER(Scripts)
		//RESET_TIMER(Camera)
		//RESET_TIMER(Nodes)
		//RESET_TIMER(GUI)
	}
#endif
	services_->getInput()->update();
	services_->getSoundManager()->update();
	services_->getPhysicsManager()->update(time_);
	services_->getThreadManager()->update();
	services_->getScriptManager()->update();
	services_->getCamera()->update(time_);
	updateNodes(services_->getRootNode());
    services_->getGUIManager()->update();
	services_->getGraphicsManager()->render();
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
