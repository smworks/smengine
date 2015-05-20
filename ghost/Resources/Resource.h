/*
 * Resource.h
 * Interface containing methods used by
 * resource manager, so all
 * resource type classes
 * must implement it.
 *
 *  Created on: 2012.06.20
 *      Author: Martynas Šustavičius
 */

#ifndef RESOURCE_H_
#define RESOURCE_H_

#include "../Multiplatform/Ghost.h"
#include "Attributes.h"

class ThreadManager;
class ServiceLocator;
class ServiceLocator;
class Node;

class Resource {
public:
	enum Type {
		NULL_RESOURCE,
		UNKNOWN,
		SHADER,
		SCRIPT,
		MODEL,
		DYNAMIC_OBJECT,
		SPRITE,
		TEXTURE_2D,
		SYMBOL,
		CUBE_MAP,
		SOUND,
		PHYSICS,
		LIGHT,
		SCENARIO,
		GUI_SURFACE
	};
	static const string VAL_NOT_SET;
	static const string VAL_TRUE;
	static const string VAL_FALSE;
	static const string ATTR_FILE;
	static const string ATTR_WIDTH;
	static const string ATTR_HEIGHT;
	static const string ATTR_DEPTH;
	static const string ATTR_RADIUS;
	static const string ATTR_VERTICAL_LOD;
	static const string ATTR_HORIZONTAL_LOD;
	static const string ATTR_APPROXIMATION;
	static const string ATTR_AMBIENT;
	static const string ATTR_DIFFUSE;
	static const string ATTR_SPECULAR;
	static const string ATTR_CULL_FACE;
	static const string ATTR_ROWS;
	static const string ATTR_COLS;
	static const string ATTR_BLENDING;
	static const string ATTR_TYPE;
	static const string VAL_MESH;
	static const string VAL_SHAPE;
	static const string VAL_PLANE;
	static const string VAL_SPHERE;
	static const string VAL_TERRAIN;
	static const string VAL_OBJ;
	static const string VAL_WATER;
	static const string ATTR_PHYSICS;
	static const string ATTR_MASS;
	static const string ATTR_FRICTION;
	static const string ATTR_LINEAR_DAMPING;
	static const string ATTR_ANGULAR_DAMPING;
	static const string ATTR_RESTITUTION;
	static const string ATTR_LAYOUT_TYPE;
	static const string VAL_RELATIVE;
	static const string VAL_LINEAR;
	static const string VAL_FILL;
	static const string VAL_WRAP;
	static const string ATTR_MARGIN_LEFT;
	static const string ATTR_MARGIN_BOTTOM;
	static const string ATTR_MARGIN_RIGHT;
	static const string ATTR_MARGIN_TOP;
	static const string ATTR_BACKGROUND;
	static const string ATTR_ALIGNMENT;
	static const string VAL_LEFT;
	static const string VAL_CENTER_H;
	static const string VAL_RIGHT;
	static const string VAL_TOP;
	static const string VAL_CENTER_V;
	static const string VAL_BOTTOM;
	static const string ATTR_TRANSPARENCY;
	static const string ATTR_LEFT;
	static const string ATTR_RIGHT;
	static const string ATTR_TOP;
	static const string ATTR_BOTTOM;
	static const string ATTR_SCREEN_LEFT;
	static const string ATTR_SCREEN_RIGHT;
	static const string ATTR_SCREEN_TOP;
	static const string ATTR_SCREEN_BOTTOM;
	static const string ATTR_BACKGROUND_SELECTED;
	static const string ATTR_TEXT;
	static const string ATTR_SIZE;
	static const string ATTR_COLOR;
    static const string ATTR_SPRITE_COUNT;
	static const string ATTR_BUFFER;
public:
	Resource(ServiceLocator* services);
	virtual ~Resource();

	/**
	 * Creates resource object.
	 */
	virtual bool create() = 0;

	/**
	 * Releases resource data.
	 */
	virtual void release() = 0;

	/**
	 * @return Resource object size in bytes.
	 */
	virtual SIZE getSize() = 0;

	/**
	 * @return Resource tpe.
	 */
	virtual Type getType() = 0;

	/**
	 * This method is called when screen resolution
	 * is changed.
	 */
	virtual void resize() {}

	/**
	 * @return True if this resource implements Renderable interface.
	 */
	bool isRenderable();

	/**
	 * @return True if this resource implements GUISurface abstract class.
	 */
    bool isGUIElement();

	/**
	 * Checks main resource components
	 * to make sure they are in
	 * expected state.
	 * @return True if resource is valid.
	 */
	virtual bool isValid() = 0;

	/**
	 * If resource header is not null,
	 * this method searches it's array of
	 * attributes and if it finds required type
	 * attribute, returns its value.
	 * @param name - attribute name.
	 * @return Value of attribute.
	 */
	string getAttribute(const string& name);

	/**
	 * If resource contains specified attribute then specified
	 * value is assigned to that attribute, otherwise new
	 * attribute is inserted.
	 * @param name - attribute name.
	 * @param value - attribute value.
	 */
	void setAttribute(const string& name, string value);

	/**
	 * Set resource name.
	 * @param name - resource name.
	 */
	void setName(const string& name);

	/**
	 * @return Name of the resource.
	 */
	const string& getName();

	/**
	 * Set node that this resource belongs to.
	 * @param node - pointer to node that contains this resource.
	 */
	 void setNode(Node* node);

	 /**
	  * @return Pointer to node that contains this resoruce.
	  */
	 Node* getNode();

	 /**
	  * Provide resource with valid service locator object.
	  * @param services - service locator object.
	  */
	 void setServiceLocator(ServiceLocator* services);

	 /**
	  * @return Pointer to service locator object.
	  */
	 ServiceLocator* getServiceLocator();

	 /**
	  * @return Reference to resource attributes object.
	  */
	 Attributes& getAttributes();

	/**
	 * Converts string type to
	 * enum type from Resource class.
	 * @param type - string containing
	 * type of resource.
	 * @return Enum indicating type.
	 */
	static Resource::Type stringToType(const string& type);

	/**
	 * Converts enum type to string.
	 * @param type - enum with resource type.
	 * @return String representing enum type.
	 */
	 static string typeToString(Type type);
private:
	/** Attributes used to create and restore resource. */
	Attributes attributes_;
	/** Resource name. */
	string name_;
	/** Pointer to all engine services. */
	ServiceLocator* services_;
	/** Pointer to node that contains this resource. */
	Node* node_;
};

#endif
