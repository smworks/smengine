/*
 * ResourceManager.h
 *
 *  Created on: 2012.06.23
 *      Author: Martynas Šustavičius
 */

#ifndef SCENELOADER_H_
#define SCENELOADER_H_

#include "Multiplatform/Ghost.h"
#include "Resources/Resource.h"
#include "Vec3.h"

class ServiceLocator;
class StaticObject;
class DynamicObject;
class Sprite;
class Node;
class XmlNode;

class ResourceManager {
public:
	static const string ROOT_NODE;
	static const string NODE;
	static const string NAME;
	static const string RESOURCE;
	static const string TYPE;
	static const string FILE;
	static const string ATTRIBUTE;
	static const string VALUE;
	static const string POS;
	static const string ROT;
	static const string SCALE;
	static const string X;
	static const string Y;
	static const string Z;
	static const string RENDERABLE;
public:
	ResourceManager(ServiceLocator* services);
	~ResourceManager();

	/**
	 * Deletes all resources and sets resource
	 * manager into default state.
	 */
	void reset();

	/**
	 * Adds resouce to memory.
	 * @param name - name of the resource.
	 * @param resource - pointer to resource
	 * object.
	 */
	void add(const string& name, Resource* resource);

	/**
	 * Checks if required resource is
	 * available.
	 * @param type - type of resource.
	 * @param name - name of the resource.
	 * @return True if resource is available.
	 */
	bool has(
		Resource::Type type,
		const string& name);

	/**
	 * Returns pointer to resource.
	 * @param type - type of resource.
	 * @param name - name of the resource.
	 * @return Pointer to required resource or
	 * 0 if resource not found.
	 */
	Resource* get(
		Resource::Type type,
		const string& name);

	/**
	 * Creates resource of specified type.
	 * @param type - resource type.
	 */
	Resource* create(Resource::Type type);

	/**
	 * Removes resource that is
	 * no longer required.
	 * @param type - type of resource.
	 * @param name - name of the resource.
	 */
	void remove(
		Resource::Type type,
		const string& name);

	/**
	 * Loads resources from specified
	 * scene XML file.
	 * @param name - name of the XML file.
	 * @return Pointer to root scene node.
	 * If something went wrong,
	 * zero will be returned.
	 */
	Node* loadScene(
		const string& name);
private:
	/**
	 * Reads node tag content from
	 * scene XML file.
	 * @param xmlNode - pointer to XML node
	 * that contains node tag.
	 * @param node - pointer to parent node.
	 */
	void loadNodes(XmlNode* xmlNode, Node* node);

	/**
	 * Reads node resources and loads them.
	 * @param xmlNode - pointer to XML node
	 * that contains node tag.
	 * @param node - pointer to parent node.
	 */
	void loadResources(XmlNode* xmlNode, Node* node);

	/**
	 * Loads node transformations if
	 * available, also checks if node
	 * has enough required data
	 * to be complete.
	 * @param xmlNode - pointer to XML node
	 * that contains node tag.
	 * @param node - pointer to parent node.
	 */
	void validateNode(XmlNode* xmlNode, Node* node);

	/**
	 * Loads position, rotation and scale
	 * attributes.
	 * @param xmlNode - pointer to XML node
	 * that contains transformation data.
	 * @param node - pointer to node that
	 * will contain transformation data.
	 */
	void loadTransformations(XmlNode* xmlNode, Node* node);

	/**
	 * Loads x, y and z attributes from
	 * specified node.
	 * @param xmlNode - pointer to XML node
	 * that contains transformation data.
	 * @return Vector containing x, y and
	 * z attributes.
	 */
	Vec3 loadVector(XmlNode* xmlNode);
public:
	ServiceLocator* services_;
	Node* rootNode_;
	unordered_map<string, Resource*> resources_;
	unordered_map<string, Resource*>::iterator it_;
};

#endif /* SCENELOADER_H_ */
