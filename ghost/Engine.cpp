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
#include "Input.h"
#include "Environment.h"
#include "ScriptManager.h"
#include "TextManager.h"
#include "ThreadManager.h"
#include "Network/NetworkManager.h"
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
#include "Resources/NullResource.h"
#include "Resources/Script.h"

mutex* Engine::mut = nullptr;
queue<Task*>* Engine::tasks = nullptr;

Engine::Engine(ServiceLocator* services) :
	ServiceProvider(services),
	time(0.0f),
	running(false),
	fpsText(nullptr),
	fpsNode(nullptr),
	fpsCount(0),
	fpsTime(0.0),
	console(nullptr)

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
	services->provide(NEW ScriptManager());
	services->provide(NEW Input());
	services->provide(NEW Environment());
	services->provide(NEW Camera());
	services->provide(NEW NodeManager(services));
	services->provide(NEW SceneManager(services));
	services->provide(NEW ScenarioManager(services));
	services->provide(NEW PhysicsManager(services));
	services->provide(NEW ResourceManager(services));
	services->provide(NEW TextManager(services));
	services->provide(NEW GUIManager(services));
	services->provide(NEW ThreadManager(services));
	services->provide(NEW NetworkManager(services));
	// Provide or initialize other resourses.
	services->getGraphicsManager()->create();
	services->provide(NEW TextureAtlas(services));
	services->getPhysicsManager()->setGraphicsManager(services->getGraphicsManager());
	services->getScriptManager()->initialize(services);
	PROFILE("Finished creating engine object.");

	fpsText = NEW GUIText(getServiceLocator());
	fpsText->setAttribute(GUIText::ATTR_WIDTH, "512px");
	fpsText->setAttribute(GUIText::ATTR_HEIGHT, "32px");
	fpsText->setAttribute(GUIText::ATTR_TEXT, "fps");
	fpsText->setAttribute(GUIText::ATTR_COLOR, "#FFFF00FF");
	fpsText->setAttribute(GUIText::ATTR_BACKGROUND, "#00000088");
	fpsText->setAttribute(GUIText::ATTR_SIZE, "12px");
	fpsText->setAttribute(GUIText::ATTR_SCREEN_LEFT, "true");
	fpsText->setAttribute(GUIText::ATTR_SCREEN_TOP, "true");
	fpsText->create();
	getResourceManager()->add("debug text", fpsText);

	loadScene(getDatabase()->getString(Database::START_SCRIPT));
	time = 0;
	console = new Console(services);
}

Engine::~Engine()
{
	delete console;
	getScriptManager()->quit();
	getServiceLocator()->release();
	delete getServiceLocator();
	fpsNode = nullptr;
	delete tasks;
	delete mut;
}

void Engine::loadScene(string script)
{
	PROFILE("Started loading scene.");
	getSoundManager()->reset();
	getPhysicsManager()->reset();

	if (getRootNode() != nullptr)
	{
		delete getRootNode();
	}
	getThreadManager()->joinAll();

	// Load scene hierarchy.
	Resource* resource = NEW NullResource(getServiceLocator());
	getResourceManager()->add(Database::ROOT_NODE, resource);
	getServiceLocator()->provide(NEW Node(getDatabase()->getString(Database::ROOT_NODE), resource));
	Resource* scriptResource = NEW Script(getServiceLocator());
	scriptResource->setAttribute(Resource::ATTR_FILE, script);
	scriptResource->create();
	getResourceManager()->add(script, scriptResource);
	Node* scriptNode = NEW Node(script, scriptResource);
	scriptResource->setNode(scriptNode);
	getRootNode()->addChild(scriptNode);
	getScriptManager()->add(scriptNode);
	fpsNode = NEW Node("fps", fpsText);
	fpsText->setNode(fpsNode);
	fpsNode->setState(Node::RENDERABLE, false);
	getRootNode()->addChild(fpsNode);


	// Other data creation.
	getGUIManager()->refreshNodes(getRootNode());
	getScriptManager()->start();
	resizeResources(getRootNode());

	// Make sure scene will not be loaded again
	getDatabase()->setString(Database::LOAD_SECOND_SCRIPT, "");
	PROFILE("Finished loading scene.")
}

void Engine::computeFrame()
{
	if (!running)
	{
		return;
	}
	getMutex().lock();
	getServiceLocator()->updateTimer(getDatabase()->getFloat(Database::FRAME_DURATION));
	time = getServiceLocator()->getFrameTime();

	updateFPS();

	if (getDatabase()->getString(Database::LOAD_SECOND_SCRIPT).length() > 0)
	{
		loadScene(getDatabase()->getString(Database::LOAD_SECOND_SCRIPT));
	}

	getInput()->update();
	if (getInput()->keyReleased(Input::TILDE))
	{
		console->show();
	}
	else if (getInput()->keyReleased(Input::P))
	{
		getPhysicsManager()->setDebugRendering(!getPhysicsManager()->isDebugRenderingEnabled());
	}
	else if (getInput()->keyReleased(Input::F))
	{
		fpsNode->setState(Node::RENDERABLE, !fpsNode->getState(Node::RENDERABLE));
	}
	getSoundManager()->update();
	getPhysicsManager()->update(time);
	getThreadManager()->update();
	getScriptManager()->update();
	getCamera()->update(time);
	updateNodes(getRootNode());
	getGUIManager()->update();
	if (getServiceLocator()->isGuiAvailable())
	{
		getGraphicsManager()->render();
	}

	executeTasks();
	getMutex().unlock();
}


void Engine::updateFPS()
{
	fpsCount++;
	fpsTime += time;
	if (fpsTime >= 1000.0f)
	{
		stringstream ss;
		ss << "FPS: " << fpsCount << fixed << setprecision(3) << "\n"
			<< "Resolution: " << getServiceLocator()->getScreenWidth()
			<< "x" << getServiceLocator()->getScreenHeight() << "px\n";
		fpsText->setText(ss.str());
		fpsCount = 0;
		fpsTime = 0.0f;
	}
}

void Engine::resume()
{
	running = true;
	getScriptManager()->resume();
}

void Engine::pause()
{
	getScriptManager()->pause();
	running = false;
}

void Engine::updateNodes(Node* node, bool force) const
{
	if (!node->getState(Node::UPDATABLE))
	{
		return;
	}
	static Mat4 tempMatrix;
	if (node->getResource()->getType() == Resource::DYNAMIC_OBJECT)
	{
		Resource* res = node->getResource();
		Animatable* anim = dynamic_cast<Animatable*>(res);
		anim->nextFrame(time);
	}
	bool update = force;
	if (node->getParent() != 0 && node->getState(Node::TRANSFORMABLE))
	{
		if (node->getState(Node::POSITION))
		{ // Position update.
			Matrix::translate(
				node->getMatPos(), node->getPos().getX(),
				node->getPos().getY(), node->getPos().getZ());
			update = true;
		}
		if (node->getState(Node::ROTATION))
		{ // Rotation update.
			Matrix::copy(node->getRot().getMatrix(), node->getMatRot());
			update = true;
		}
		if (node->getState(Node::SCALING))
		{ // Scaling update.
			Matrix::scale(
				node->getMatScale(), node->getScale().getX(),
				node->getScale().getY(), node->getScale().getZ());
			update = true;
		}
		if (update)
		{ // Combined matrix update.
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
	while (it != children.end())
	{
		updateNodes((*it), update);
		it++;
	}
}

void Engine::resizeScreen(UINT32 width, UINT32 height)
{
	PROFILE("Resizing window. New width=%d, height=%d.", width, height);
	if (width == 0 || height == 0)
	{
		return;
	}
	getServiceLocator()->setScreenWidth(width);
	getServiceLocator()->setScreenHeight(height);
	getCamera()->updateProjections(width, height,
		getDatabase()->getFloat(Database::FIELD_OF_VIEW),
		getDatabase()->getFloat(Database::NEAR_PLANE_DISTANCE),
		getDatabase()->getFloat(Database::FAR_PLANE_DISTANCE));
	if (getServiceLocator()->isGuiAvailable())
	{
		getGraphicsManager()->resize(width, height);
	}
	resizeResources(getRootNode());
	getScriptManager()->resize();
	fpsTime = 1001.0f;
	PROFILE("Window resized.");
}

void Engine::resizeResources(Node* node)
{
	if (node == nullptr)
	{
		return;
	}
	node->getResource()->resize();
	auto& children = node->getChildren();
	auto it = children.begin();
	while (it != children.end())
	{
		resizeResources((*it));
		++it;
	}
}

mutex& Engine::getMutex()
{
	static mutex mut;
	return mut;
}

void Engine::executeOnMainThread(Task* task)
{
	mut->lock();
	tasks->push(task);
	mut->unlock();
}

void Engine::executeTasks()
{
	mut->lock();
	while (!tasks->empty())
	{
		Task* task = tasks->front();
		task->run();
		delete task;
		tasks->pop();
	}
	mut->unlock();
}
