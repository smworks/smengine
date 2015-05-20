/*
 * ResourceManager.cpp
 *
 *  Created on: 2012.06.23
 *      Author: Martynas Šustavičius
 */

#include "ResourceManager.h"
#include "Multiplatform/ServiceLocator.h"
#include "Multiplatform/FileManager.h"
#include "Multiplatform/Database.h"
#include "Node.h"
#include "Xml.h"
#include "Vec3.h"
#include "Matrix.h"
#include "Shapes.h"
#include "PhysicsManager.h"
#include "ScriptManager.h"
#include "Resources/TextureRGBA.h"
#include "Resources/CubeMap.h"
#include "Resources/Shader.h"
#include "Resources/Sprite.h"
#include "Resources/Model.h"
#include "Resources/DynamicObject.h"
#include "Resources/Script.h"
#include "Resources/Sound.h"
#include "Resources/Light.h"
#include "Resources/Scenario.h"
#include "Resources/GUIImage.h"
#include "Resources/GUIButton.h"
#include "Resources/GUIInput.h"
#include "Resources/Renderable.h"
#include "Resources/NullResource.h"

const string ResourceManager::ROOT_NODE = "root_node";
const string ResourceManager::NODE = "node";
const string ResourceManager::NAME = "name";
const string ResourceManager::RESOURCE = "resource";
const string ResourceManager::TYPE = "type";
const string ResourceManager::FILE = "file";
const string ResourceManager::ATTRIBUTE = "attribute";
const string ResourceManager::VALUE = "value";
const string ResourceManager::POS = "position";
const string ResourceManager::ROT = "rotation";
const string ResourceManager::SCALE = "scaling";
const string ResourceManager::X = "x";
const string ResourceManager::Y = "y";
const string ResourceManager::Z = "z";
const string ResourceManager::RENDERABLE = "renderable";

#define CREATE_KEY(name, type) name + "_" + Resource::typeToString(type)

#define GET_VALUE(var, node, name) string* var = node->getAttr(name); \
	if (var == 0) { var = node->getContent(name); }

ResourceManager::ResourceManager(ServiceLocator* services) :
	services_(services),
	rootNode_(0)
{
	LOGD("Created resource manager.");
}

ResourceManager::~ResourceManager() {
	reset();
	LOGD("Deleted resource manager.");
}

void ResourceManager::reset() {
	it_ = resources_.begin();
	while (it_ != resources_.end()) {
		//LOGD("  Deleting: %s", it_->first.c_str());
		delete (*it_).second;
		it_++;
	}
	resources_.clear();
}

void ResourceManager::add(const string& name, Resource* resource) {
	string key = CREATE_KEY(name, resource->getType());
    unordered_map<string, Resource*>::iterator it = resources_.find(key);
	if (resources_.end() != it) {
		LOGD("Resource \"%s\" is already in resource manager.", name.c_str());
		if (resource != it->second) {
			LOGW("Resource in resource manager represents different object, so removing it.");
			remove(resource->getType(), name);
		}
	}
	resource->setName(name);
	resources_.insert(pair<string, Resource*>(key, resource));
}

bool ResourceManager::has(
	Resource::Type type, const string& name)
{
	return resources_.end() !=
		resources_.find(CREATE_KEY(name, type));
}

Resource* ResourceManager::get(Resource::Type type, const string& name) {
	it_ = resources_.find(CREATE_KEY(name, type));
	if (it_ != resources_.end()) {
		return (*it_).second;
	}
	return 0;
}

Resource* ResourceManager::create(Resource::Type type) {
	switch (type) {
	case Resource::TEXTURE_2D: return NEW TextureRGBA(services_);
	case Resource::SHADER: return NEW Shader(services_);
	case Resource::MODEL: return NEW Model(services_);
	case Resource::DYNAMIC_OBJECT: return NEW DynamicObject(services_);
	case Resource::SPRITE: return NEW Sprite(services_);
	case Resource::SCRIPT: return NEW Script(services_);
	case Resource::CUBE_MAP: return NEW CubeMap(services_);
	case Resource::SOUND: return NEW Sound(services_);
	case Resource::LIGHT: return NEW Light(services_);
	case Resource::SCENARIO: return NEW Scenario(services_);
	default:
		return 0;
	}
}

void ResourceManager::remove(Resource::Type type, const string& name) {
	it_ = resources_.begin();
	string key = CREATE_KEY(name, type);
	while (it_ != resources_.end()) {
		if (it_->first == key) {
			delete (*it_).second;
			resources_.erase(it_);
			return;
		}
		it_++;
	}
}

Node* ResourceManager::loadScene(const string& path) {
	if (services_ == 0) {
		LOGW("Service locator not specified.");
		return 0;
	}
	string content;
	PROFILE("Started parsing scene file.");
	services_->getFileManager()->loadText(content, path.c_str());
	stringReplace(content, "\\n", "\n");
	Xml xml(content);
	XmlNode* xmlNode = xml.getRootNode();
	PROFILE("Finished parsing scene file.");
	if (xmlNode == 0) {
		return 0;
	}
	Resource* resource = NEW NullResource(services_);
	add("rootNode", resource);
	rootNode_ = NEW Node(services_->getDB()->getString(ROOT_NODE), resource);
	loadNodes(xmlNode, rootNode_);
	return rootNode_;
}

void ResourceManager::loadNodes(
	XmlNode* xmlParent, Node* parent)
{
	vector<XmlNode*> xmlNodes = xmlParent->getNodes(NODE);
	SIZE nodeCount = xmlNodes.size();
	XmlNode* xmlNode = 0;
	for (SIZE k = 0; k < nodeCount; k++) {
		xmlNode = xmlNodes[k];
		Node* node = createNode(xmlNode, parent);
		// Is node renderable.
		GET_VALUE(renderable, xmlNode, RENDERABLE);
		if (renderable != 0) {
			bool state = toBool(*renderable);
			node->setState(Node::RENDERABLE, state);
		} else {
			node->setState(Node::RENDERABLE,
				parent->getState(Node::RENDERABLE));
		}
		// Load children nodes for current node.
		if (xmlNode->countNodes(NODE) != 0) {
			loadNodes(xmlNode, node);
		}
		validateNode(xmlNode, node);
		parent->addChild(node);
	}
}

Node* ResourceManager::createNode(XmlNode* xmlNode, Node* parent) {
	GET_VALUE(name, xmlNode, NAME);
	if (name == 0) {
		THROWEXEXT("Node name was not specified. This might indicate error in XML file. Parent node name is \"%s\".",
			parent->getName().c_str());
	}
	LOGD("Parsing node: %s.", name->c_str());
	vector<XmlNode*> res = xmlNode->getNodes(RESOURCE);
	if (res.size() != 1) {
		THROWEXEXT("Node %s must contain one resource.", name->c_str());
	}
	XmlNode* r = res[0];
	string* type = r->getAttr(TYPE);
	string* file = r->getAttr(FILE);
	if (type == 0) {
		THROWEXEXT("Type attribute not specified for node %s.", name->c_str());
	}
	Resource::Type resType = Resource::stringToType(*type);
	Resource* resource = create(resType);
	if (resource == 0) {
		THROWEXEXT("Node \"%s\" contains unknown resource type: %s.",
			name->c_str(), type->c_str());
	}
	Attributes& attr = resource->getAttributes();
	if (file != 0) {
		attr.setString(Resource::ATTR_FILE, *file);
	}
	vector<XmlNode*> attrib = r->getNodes(ATTRIBUTE);
	for (UINT32 j = 0; j < attrib.size(); j++) {
		XmlNode* a = attrib[j];
		GET_VALUE(rn, a, TYPE);
		GET_VALUE(rv, a, VALUE);
		if (rn == 0 || rv == 0) {
			continue;
		}
		attr.setString(*rn, *rv);
	}
    string resName = *name + "_" + attr.getString(FILE);
    if (has(resType, resName)) {
        delete resource;
        resource = get(resType, resName);
    } else {
        add(resName, resource);
        resource->create();
    }
	Node* node = NEW Node(*name, resource);
	resource->setNode(node);
	node->setParent(parent);
	return node;
}

void ResourceManager::validateNode(
	XmlNode* xmlNode, Node* node)
{
	// Initialize node transformations.
	loadTransformations(xmlNode, node);

	// If node has sprite resource and unspecified scale,
	// set scale to original image dimensions.
	if (node->getResource()->getType() == Resource::SPRITE) {
		if (node->getScale().getX() - 1.0f < GHOST_DELTA
			|| node->getScale().getY() - 1.0f < GHOST_DELTA)
		{
			Resource* res = node->getResource();
			if (res != 0) {
				Sprite* spr = dynamic_cast<Sprite*>(res);
				TextureRGBA* tex = (TextureRGBA*) spr->getPointerToTexture();
				if (spr != 0) {
					node->getScale().setXYZ((float) tex->getWidth(), (float) tex->getHeight(), 1.0f);
				}
			}
		}
	}
	// If node contains physics resource, add node to physics manager.
	// Also make sure the bounding box is available.
	Resource* model = node->getResource();
	if (model != 0 && model->getAttribute(Resource::ATTR_PHYSICS) == Resource::VAL_TRUE) {
		services_->getPhysicsManager()->add(node);
	}
	// If node contains script resource, add node to script manager.
	if (node->getResource()->getType() == Resource::SCRIPT) {
		services_->getScriptManager()->add(node);
	}
}

void ResourceManager::loadTransformations(XmlNode* xmlNode, Node* node) {
	XmlNode* vecNode = 0;
	vecNode = xmlNode->getNode(POS);
	if (vecNode != 0) {
		node->getPos().setXYZ(loadVector(vecNode));
		node->setState(Node::POSITION, true);
	}
	vecNode = xmlNode->getNode(ROT);
	if (vecNode != 0) {
		Vec3 r = loadVector(vecNode);
		node->getRot().setXYZ(r.getX(), r.getY(), r.getZ());
		node->setState(Node::ROTATION, true);
	}
	vecNode = xmlNode->getNode(SCALE);
	if (vecNode != 0) {
		node->getScale().setXYZ(loadVector(vecNode));
		node->setState(Node::SCALING, true);
	}
}

Vec3 ResourceManager::loadVector(XmlNode* xmlNode) {
	Vec3 vec;
	GET_VALUE(x, xmlNode, X);
	if (x != 0) {
		vec.setX(toFloat(x->c_str()));
	}
	GET_VALUE(y, xmlNode, Y);
	if (y != 0) {
		vec.setY(toFloat(y->c_str()));
	}
	GET_VALUE(z, xmlNode, Z);
	if (z != 0) {
		vec.setZ(toFloat(z->c_str()));
	}
	return vec;
}
