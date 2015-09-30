/*
 * Resource.h
 *
 *  Created on: 2012.12.05
 *      Author: Martynas Šustavičius
 */

#include "Resource.h"
#include "../Multiplatform/ServiceLocator.h"
#include "Renderable.h"
#include "GUISurface.h"

const string Resource::VAL_NOT_SET = "";
const string Resource::VAL_TRUE = "true";
const string Resource::VAL_FALSE = "false";
const string Resource::ATTR_FILE = "file";
const string Resource::ATTR_WIDTH = "width";
const string Resource::ATTR_HEIGHT = "height";
const string Resource::ATTR_DEPTH = "depth";
const string Resource::ATTR_RADIUS = "radius";
const string Resource::ATTR_VERTICAL_LOD = "vertical_lod";
const string Resource::ATTR_HORIZONTAL_LOD = "horizontal_lod";
const string Resource::ATTR_APPROXIMATION = "approximation";
const string Resource::ATTR_AMBIENT = "ambient";
const string Resource::ATTR_DIFFUSE = "diffuse";
const string Resource::ATTR_SPECULAR = "specular";
const string Resource::ATTR_CULL_FACE = "cull_face";
const string Resource::ATTR_ROWS = "rows";
const string Resource::ATTR_COLS = "cols";
const string Resource::ATTR_BLENDING = "blending";
const string Resource::ATTR_TYPE = "type";
const string Resource::VAL_MESH = "mesh";
const string Resource::VAL_SHAPE = "shape";
const string Resource::VAL_PLANE = "plane";
const string Resource::VAL_SPHERE = "sphere";
const string Resource::VAL_TERRAIN = "terrain";
const string Resource::VAL_OBJ = "obj";
const string Resource::VAL_WATER = "water";
const string Resource::VAL_POINT = "point";
const string Resource::VAL_SPOT = "spot";
const string Resource::VAL_GLOBAL = "global";
const string Resource::ATTR_PHYSICS = "physics";
const string Resource::ATTR_MASS = "mass";
const string Resource::ATTR_FRICTION = "friction";
const string Resource::ATTR_LINEAR_DAMPING = "linear_damping";
const string Resource::ATTR_ANGULAR_DAMPING = "angular_damping";
const string Resource::ATTR_RESTITUTION = "restitution";
const string Resource::ATTR_DISABLE_ROTATION = "disable_rotation";
const string Resource::ATTR_LAYOUT_TYPE = "layout_type";
const string Resource::VAL_RELATIVE = "relative";
const string Resource::VAL_LINEAR = "linear";
const string Resource::VAL_FILL = "fill";
const string Resource::VAL_WRAP = "wrap";
const string Resource::ATTR_MARGIN_LEFT = "marginLeft";
const string Resource::ATTR_MARGIN_BOTTOM = "marginBottom";
const string Resource::ATTR_MARGIN_RIGHT = "marginRight";
const string Resource::ATTR_MARGIN_TOP = "marginTop";
const string Resource::ATTR_BACKGROUND = "background";
const string Resource::ATTR_ALIGNMENT = "alignment";
const string Resource::VAL_LEFT = "left";
const string Resource::VAL_CENTER_H = "center_h";
const string Resource::VAL_RIGHT = "right";
const string Resource::VAL_TOP = "top";
const string Resource::VAL_CENTER_V = "center_v";
const string Resource::VAL_BOTTOM = "bottom";
const string Resource::ATTR_TRANSPARENCY = "transparency";
const string Resource::ATTR_LEFT = "left";
const string Resource::ATTR_RIGHT = "right";
const string Resource::ATTR_TOP = "top";
const string Resource::ATTR_BOTTOM = "bottom";
const string Resource::ATTR_SCREEN_LEFT = "screen_left";
const string Resource::ATTR_SCREEN_RIGHT = "screen_right";
const string Resource::ATTR_SCREEN_TOP = "screen_top";
const string Resource::ATTR_SCREEN_BOTTOM = "screen_bottom";
const string Resource::ATTR_BACKGROUND_SELECTED = "background_selected";
const string Resource::ATTR_TEXT = "text";
const string Resource::ATTR_SIZE = "size";
const string Resource::ATTR_COLOR = "color";
const string Resource::ATTR_SPRITE_COUNT = "sprite_count";
const string Resource::ATTR_BUFFER = "buffer";

Resource::Resource(ServiceLocator* services) :
	name_(""),
	services_(services),
	node_(0)
{}

Resource::~Resource() {}

bool Resource::isRenderable() {
	if (dynamic_cast<const Renderable*>(this)) {
		return true;
	}
	return false;
}

bool Resource::isGUIElement() {
	if (dynamic_cast<const GUISurface*>(this)) {
		return true;
	}
	return false;
}

string Resource::getAttribute(const string& name) {
	return attributes_.getString(name);
}

void Resource::setAttribute(const string& name, string value) {
	attributes_.setString(name, value);
}

void Resource::setName(const string& name) {
	name_ = name;
}

const string& Resource::getName() {
	return name_;
}

void Resource::setNode(Node* node) {
    node_ = node;
}

Node* Resource::getNode() {
    return node_;
}

void Resource::setServiceLocator(ServiceLocator* services) {
	if (services == 0) {
		LOGE("Service locator object must not be null.");
		return;
	}
	services_ = services;
}

ServiceLocator* Resource::getServiceLocator() {
	return services_;
}

Attributes& Resource::getAttributes() {
	return attributes_;
}

Resource::Type Resource::stringToType(
	const string& type)
{
	if ("texture" == type) {
		return Resource::TEXTURE_2D;
	}
	if ("symbol" == type) {
		return Resource::SYMBOL;
	}
	if ("shader" == type) {
		return Resource::SHADER;
	}
	if ("static_object" == type) {
		return Resource::MODEL;
	}
	if ("dynamic_object" == type) {
		return Resource::DYNAMIC_OBJECT;
	}
	if ("sprite" == type) {
		return Resource::SPRITE;
	}
	if ("script" == type) {
		return Resource::SCRIPT;
	}
	if ("cube_map" == type) {
		return Resource::CUBE_MAP;
	}
	if ("sound" == type) {
		return Resource::SOUND;
	}
	if ("physics" == type) {
		return Resource::PHYSICS;
	}
	if ("light" == type) {
		return Resource::LIGHT;
	}
	if ("scenario" == type) {
		return Resource::SCENARIO;
	}
	if ("gui_surface" == type) {
		return Resource::GUI_SURFACE;
	}
	return Resource::UNKNOWN;
}

string Resource::typeToString(Type type) {
    switch (type) {
    case TEXTURE_2D:
        return "texture";
        break;
	case SYMBOL:
		return "symbol";
		break;
    case SHADER:
        return "shader";
        break;
    case MODEL:
        return "static_object";
        break;
    case DYNAMIC_OBJECT:
        return "dynamic_object";
        break;
    case SPRITE:
        return "sprite";
        break;
    case SCRIPT:
        return "script";
        break;
    case CUBE_MAP:
        return "cube_map";
        break;
    case SOUND:
        return "sound";
        break;
    case PHYSICS:
        return "physics";
        break;
    case LIGHT:
        return "light";
        break;
	case SCENARIO:
		return "scenario";
		break;
    case GUI_SURFACE:
        return "gui_surface";
        break;
    default:
        return "unknown";
        break;
    }
}
