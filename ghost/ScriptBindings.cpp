/*
 * ScriptBindings.cpp
 *
 *  Created on: 2013.11.03
 *      Author: Martynas Šustavičius
 */

#include "ScriptBindings.h"
#include "ScriptManager.h"
#include "Multiplatform/ServiceLocator.h"
#include "Multiplatform/SoundManager.h"
#include "Multiplatform/Database.h"
#include "Input.h"
#include "Node.h"
#include "Camera.h"
#include "PhysicsManager.h"
#include "ResourceManager.h"
#include "Network/NetworkManager.h"
#include "Network/HttpRequest.h"
#include "Network/HttpResponse.h"
#include "ScenarioManager.h"
#include "SceneManager.h"
#include "GUIManager.h"
#include "Resources/Model.h"
#include "Resources/Shader.h"
#include "Resources/Sprite.h"
#include "Resources/Sound.h"
#include "Resources/Texture.h"
#include "Resources/TextureMono.h"
#include "Resources/TextureRGBA.h"
#include "Resources/TextureRGB.h"
#include "Resources/AtlasTexture.h"
#include "Resources/GUIButton.h"
#include "Resources/NullResource.h"
#include "Resources/Scenario.h"
#include "Extensions/Vehicle.h"
#include "ModelData.h"

#define SM_INDEX(index) index + 1
#define SM_GET_ARGUMENT_COUNT(state) lua_gettop(state)
#define SM_IS_NULL(state, index) lua_isnil(state, SM_INDEX(index))
#define SM_IS_BOOL(state, index) lua_isboolean(state, SM_INDEX(index))
#define SM_GET_BOOL(state, index) \
	lua_toboolean(state, SM_INDEX(index)) == 0 ? false : true
#define SM_RETURN_BOOL(state, val) lua_pushboolean(state, val)
#define SM_IS_NUMBER(state, index) lua_isnumber(state, SM_INDEX(index))
#define SM_GET_BYTE(state, index) \
	static_cast<UINT8>(lua_tonumber(state, SM_INDEX(index)))
#define SM_RETURN_BYTE(state, val) lua_pushnumber(state, (UINT8) val);
#define SM_GET_INT(state, index) \
	static_cast<INT32>(lua_tonumber(state, SM_INDEX(index)))
#define SM_RETURN_INT(state, val) lua_pushnumber(state, val);
#define SM_GET_FLOAT(state, index) \
	static_cast<float>(lua_tonumber(state, SM_INDEX(index)))
#define SM_RETURN_FLOAT(state, val) lua_pushnumber(state, val);
#define SM_IS_STRING(state, index) lua_isstring(state, SM_INDEX(index))
#define SM_GET_STRING(state, index) lua_tostring(state, SM_INDEX(index))
#define SM_RETURN_STRING(state, val) lua_pushstring(state, val)
#define SM_IS_OBJECT(state, index) lua_isuserdata(state, SM_INDEX(index))
#define SM_GET_OBJECT(state, index, type) \
	*(static_cast<type**>(lua_touserdata(state, SM_INDEX(index))))
#define SM_RETURN_OBJECT(state, name, type, object) \
	type** udata = (type**) lua_newuserdata(state, sizeof(type*)); \
	*udata = object; \
	luaL_getmetatable(state, name); \
	lua_setmetatable(state, -2)
#define SM_RETURN_NIL(state) lua_pushnil(state)
#define SM_POP_ARGS(state, argc) lua_pop(state, argc)
#define SM_GET_SL() ScriptManager::getServiceLocator()
#define SM_GET_RM() SM_GET_SL()->getRM()
#define SM_GET_PM() SM_GET_SL()->getPhysicsManager()

#define ADD_METHOD(map, method_name, method) \
    map.insert(pair<string, int (*)(lua_State*)>(method_name, method))

#define METHOD_ADD_SET_STATE(class_name, method_name, method_call, state) \
	int method_name(lua_State* L) { \
		ASSERT(SM_GET_ARGUMENT_COUNT(L) == 2, "Bad argument count while adding/setting data." \
			" Method: " #method_name); \
		class_name* obj = SM_GET_OBJECT(L, 0, class_name); \
		float value = SM_GET_FLOAT(L, 1); \
		if (obj != 0) { \
			obj->method_call(value); \
			obj->setState(state, true); \
		} \
		return 0; \
	}

#define METHOD_ADD_SET(class_name, method_name, method_call) \
	int method_name(lua_State* L) { \
		ASSERT(SM_GET_ARGUMENT_COUNT(L) == 2, "Bad argument count while adding/setting data." \
			" Method: " #method_name); \
		class_name* obj = SM_GET_OBJECT(L, 0, class_name); \
		float value = SM_GET_FLOAT(L, 1); \
		if (obj != 0) { \
			obj->method_call(value); \
		} \
		return 0; \
	}

#define METHOD_GET(class_name, method_name, method_call) \
	int method_name(lua_State* L) { \
		class_name* obj = SM_GET_OBJECT(L, 0, class_name); \
		float ret = 0.0f; \
		if (obj != 0) { \
			ret = obj->method_call; \
		} \
		SM_RETURN_FLOAT(L, ret); \
		return 1; \
	}

#define METHOD_ADD_SET_TWO_STATE(class_name, method_name, method1, method2, state) \
	int method_name(lua_State* L) { \
		ASSERT(SM_GET_ARGUMENT_COUNT(L) == 3, "Bad argument count while adding/setting data." \
			" Method: " #method_name); \
		class_name* obj = SM_GET_OBJECT(L, 0, class_name); \
		float param1 = SM_GET_FLOAT(L, 1); \
		float param2 = SM_GET_FLOAT(L, 2); \
		if (obj != 0) { \
			obj->method1(param1); \
			obj->method2(param2); \
			obj->setState(state, true); \
		} \
		return 0; \
	}

#define METHOD_ADD_SET_TWO(class_name, method_name, method1, method2) \
	int method_name(lua_State* L) { \
		ASSERT(SM_GET_ARGUMENT_COUNT(L) == 3, "Bad argument count while adding/setting data." \
			" Method: " #method_name); \
		class_name* obj = SM_GET_OBJECT(L, 0, class_name); \
		float param1 = SM_GET_FLOAT(L, 1); \
		float param2 = SM_GET_FLOAT(L, 2); \
		if (obj != 0) { \
			obj->method1(param1); \
			obj->method2(param2); \
		} \
		return 0; \
	}

#define METHOD_GET_TWO(class_name, method_name, method1, method2) \
	int method_name(lua_State* L) { \
		class_name* obj = SM_GET_OBJECT(L, 0, class_name); \
		float ret1 = 0.0f, ret2 = 0.0f; \
		if (obj != 0) { \
			ret1 = obj->method1; \
			ret2 = obj->method2; \
		} \
		SM_RETURN_FLOAT(L, ret1); \
		SM_RETURN_FLOAT(L, ret2); \
		return 2; \
	}

#define METHOD_ADD_SET_THREE_STATE(class_name, method_name, method, state) \
	int method_name(lua_State* L) { \
		ASSERT(SM_GET_ARGUMENT_COUNT(L) == 4, "Bad argument count while adding/setting data." \
			" Method: " #method_name); \
		class_name* obj = SM_GET_OBJECT(L, 0, class_name); \
		float param1 = SM_GET_FLOAT(L, 1); \
		float param2 = SM_GET_FLOAT(L, 2); \
		float param3 = SM_GET_FLOAT(L, 3); \
		if (obj != 0) { \
			obj->method(param1, param2, param3); \
			obj->setState(state, true); \
		} \
		return 0; \
	}

#define METHOD_ADD_SET_THREE(class_name, method_name, method) \
	int method_name(lua_State* L) { \
		ASSERT(SM_GET_ARGUMENT_COUNT(L) == 4, "Bad argument count while adding/setting data." \
			" Method: " #method_name); \
		class_name* obj = SM_GET_OBJECT(L, 0, class_name); \
		float param1 = SM_GET_FLOAT(L, 1); \
		float param2 = SM_GET_FLOAT(L, 2); \
		float param3 = SM_GET_FLOAT(L, 3); \
		if (obj != 0) { \
			obj->method(param1, param2, param3); \
		} \
		return 0; \
	}

#define METHOD_GET_THREE(class_name, method_name, method) \
	int method_name(lua_State* L) { \
		class_name* obj = SM_GET_OBJECT(L, 0, class_name); \
		float ret1 = 0.0f, ret2 = 0.0f, ret3 = 0.0f; \
		if (obj != 0) { \
			float* ret = obj->method; \
			ret1 = ret[0]; \
			ret2 = ret[1]; \
			ret3 = ret[2]; \
		} \
		SM_RETURN_FLOAT(L, ret1); \
		SM_RETURN_FLOAT(L, ret2); \
		SM_RETURN_FLOAT(L, ret3); \
		return 3; \
	}

#define METHOD_GET_THREE_ROT(class_name, method_name, method1, method2, method3) \
	int method_name(lua_State* L) { \
		class_name* obj = SM_GET_OBJECT(L, 0, class_name); \
		float ret1 = 0.0f, ret2 = 0.0f, ret3 = 0.0f; \
		if (obj != 0) { \
			ret1 = obj->method1; \
			ret2 = obj->method2; \
			ret3 = obj->method3; \
		} \
		SM_RETURN_FLOAT(L, ret1); \
		SM_RETURN_FLOAT(L, ret2); \
		SM_RETURN_FLOAT(L, ret3); \
		return 3; \
	}

int print(lua_State* L) {
	int nArgs = lua_gettop(L);
	int i;
	lua_getglobal(L, "tostring");
	std::string ret;//this is where we will dump the output
	//make sure you start at 1 *NOT* 0
	for (i = 1; i <= nArgs; i++) {
		const char *s;
		lua_pushvalue(L, -1);
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);
		s = lua_tostring(L, -1);
		if(s == 0)
			return luaL_error(L,
				LUA_QL("tostring") " must return a string to ",
				LUA_QL("print"));
		if (i > 1) {
		    ret.append("\t");
		}
		ret.append(s);
		lua_pop(L, 1);
	};
	// Print the message.
	LOGI("%s", ret.c_str());    
	return 0;
}

int pointerIsOver(lua_State* L) {
	int argc = lua_gettop(L);
	Node* node = *(Node**) lua_touserdata(L, argc);
	lua_pop(L, argc);
	Input* input = ScriptManager::getServiceLocator()->getInput();
	float x = (float) input->getPointerX();
	float y = (float) input->getPointerY();
	if (node->getResource()->getType() == Resource::SPRITE) {
		if (x > node->getPos().getX()
			&& x < node->getPos().getX() + node->getScale().getX()
			&& y > node->getPos().getY()
			&& y < node->getPos().getY() + node->getScale().getY())
		{
			lua_pushboolean(L, true);
			return 1;
		}
	} else if (node->getResource()->getType() == Resource::GUI_SURFACE) {
		GUISurface* surface = dynamic_cast<GUIText*>(node->getResource());
		if (x > surface->getMarginLeft()
			&& x < surface->getMarginLeft() + surface->getWidth()
			&& y > surface->getMarginBottom()
			&& y < surface->getMarginBottom() + surface->getHeight())
		{
			lua_pushboolean(L, true);
			return 1;
		}
	}
	lua_pushboolean(L, false);
	return 1;
}

int playSound(lua_State* L) {
	string file = SM_GET_STRING(L, 0);
	bool cond = SM_GET_BOOL(L, 1);
	Sound* sound = static_cast<Sound*>(SM_GET_RM()->get(Resource::SOUND, file));
	if (sound == 0) {
		sound = NEW Sound(SM_GET_SL());
		sound->setAttribute(Resource::ATTR_FILE, file);
		sound->create();
		SM_GET_RM()->add(file, sound);
	}
	sound->setRepeat(cond);
	SM_GET_SL()->getSoundManager()->play(sound);
	return 0;
}

int getTexture(lua_State* L) {
	ASSERT(SM_GET_ARGUMENT_COUNT(L) == 1, "No texture name specified for getTexture()");
	string val = SM_GET_STRING(L, 0);
	Texture* texture = static_cast<Texture*>(SM_GET_RM()->get(Resource::TEXTURE_2D, val));
	if (texture == 0) {
		SM_RETURN_NIL(L);
	} else {
		SM_RETURN_OBJECT(L, "Texture", Texture, texture);
	}
	return 1;
}

int loadScene(lua_State* L) {
	string val = SM_GET_STRING(L, 0);
	SM_GET_SL()->getDB()->setScene(val);
	return 0;
}

int exit(lua_State* L) {
	SM_GET_SL()->exit();
	return 0;
}

int getScreenWidth(lua_State* L) {
	lua_pushnumber(L, SM_GET_SL()->getScreenWidth());
	return 1;
}

int getScreenHeight(lua_State* L) {
	lua_pushnumber(L, SM_GET_SL()->getScreenHeight());
	return 1;
}

int getTimeDelta(lua_State* L) {
	lua_pushnumber(L, SM_GET_SL()->getFrameTime());
	return 1;
}

void registerFunctions() {
	ScriptManager::addFunction("print", print);
	ScriptManager::addFunction("pointerIsOver", pointerIsOver);
	ScriptManager::addFunction("playSound", playSound);
	ScriptManager::addFunction("getTexture", getTexture);
	ScriptManager::addFunction("loadScene", loadScene);
	ScriptManager::addFunction("exit", exit);
	ScriptManager::addFunction("getScreenWidth", getScreenWidth);
	ScriptManager::addFunction("getScreenHeight", getScreenHeight);
	ScriptManager::addFunction("getTimeDelta", getTimeDelta);
}

void registerClasses() {
	registerNode();
	registerInput();
	registerModel();
	registerTexture();
	registerGUISurface();
	registerGUIText();
	registerGUIButton();
	registerSprite();
	registerCamera();
	registerShader();
	registerNetworkManager();
	registerHttpRequest();
	registerHttpResponse();
	//registerVehicle();
	registerScenario();
	registerScenarioManager();
	registerSceneManager();
	registerDatabase();
}

int getNode(lua_State* L) {
    string name = SM_GET_STRING(L, 0);
	Node* node = SM_GET_SL()->getRootNode()->search(name);
	if (node == 0) {
        LOGW("Node with name \"%s\" not found.", name.c_str());
        return 0;
	}
	SM_RETURN_OBJECT(L, "Node", Node, node);
    return 1;
}

int deleteNode(lua_State* L) {
    //LOGI("Deleting node.");
	//Node* node = *(Node**) luaL_checkudata(L, 1, "node");
 //   delete node;
    return 0;
}

int nodeSetParent(lua_State* L) {
	Node* child = SM_GET_OBJECT(L, 0, Node);
	ASSERT(SM_GET_ARGUMENT_COUNT(L) == 2, "Wrong argument count for setParent()");
	Node* parent = SM_GET_OBJECT(L, 1, Node);
	Node* oldParent = child->getParent();
	if (oldParent != 0) {
		oldParent->removeChild(child, false);
	}
	child->setParent(parent);
	parent->addChild(child);
	return 0;
}

int nodeAddChild(lua_State* L) {
	Node* parent = SM_GET_OBJECT(L, 0, Node);
	ASSERT(SM_GET_ARGUMENT_COUNT(L) == 2, "Wrong argument count for addChild()");
	Node* child = SM_GET_OBJECT(L, 1, Node);
	Node* oldParent = child->getParent();
	if (oldParent != 0) {
		oldParent->removeChild(child, false);
	}
	child->setParent(parent);
	parent->addChild(child);
	return 0;
}

int nodeEnablePhysics(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	int argc = SM_GET_ARGUMENT_COUNT(L);
	Model* model = dynamic_cast<Model*>(node->getResource());
	if (model == 0) {
		LOGW("No model resource available for node %s. Physics not enabled",
			node->getName().c_str());
		return 0;
	}
	if (argc == 2) {
		model->setAttribute(Resource::ATTR_MASS, SM_GET_STRING(L, 1));
	}
	SM_GET_PM()->add(node);
	return 0;
}

int nodeDisablePhysics(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	SM_GET_PM()->remove(node);
	return 0;
}

int nodeSetMass(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	Resource* model = node->getResource();
	if (model == 0) {
		LOGW("Unable to set mass for node \"%s\", because there is no model attached to it.",
			node->getName().c_str());
		return 0;
	}
	float value = SM_GET_FLOAT(L, 1);
	stringstream ss;
	ss << value;
    string str = ss.str();
	model->setAttribute(Resource::ATTR_MASS, str);
	SM_GET_PM()->updateNode(node);
	return 0;
}

int nodeAccelerate(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	float x = SM_GET_FLOAT(L, 1);
	float y = SM_GET_FLOAT(L, 2);
	float z = SM_GET_FLOAT(L, 3);
    Vec3 vec = Vec3(x, y, z);
	SM_GET_PM()->move(node, vec);
	//btRaycastVehicle* raycastVehicle = vehicle->getRaycastVehicle();
	//raycastVehicle->applyEngineForce(acceleration, 2);
	//raycastVehicle->applyEngineForce(acceleration, 3);
	//raycastVehicle->setBrake(0.0f, 0);
	//raycastVehicle->setBrake(0.0f, 1);
	return 0;
}

int nodeTurn(lua_State* L) {
	//Vehicle* vehicle = 0;
	//ScriptManager::getObject("Vehicle", vehicle);
	//float rotation = 0.0f;
	//ScriptManager::getParam(rotation);
	//btRaycastVehicle* raycastVehicle = vehicle->getRaycastVehicle();
	//float maxRot = DEG_TO_RAD(vehicle->getWheelTurn());
	//float rot = raycastVehicle->getSteeringValue(0) + DEG_TO_RAD(rotation);
	//if (rot > maxRot) {
	//	rot = maxRot;
	//} else if (rot < -maxRot) {
	//	rot = -maxRot;
	//}
	//raycastVehicle->setSteeringValue(rot, 0);
	//raycastVehicle->setSteeringValue(rot, 1);
	return 0;
}

int nodeSetShader(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	if (SM_IS_NULL(L, 1)) {
		LOGW("Specified shader is null.");
		return 0;
	}
	Shader* shader = SM_GET_OBJECT(L, 1, Shader);
	ASSERT(shader != 0, "No shader specified for setShader method.");
	Resource* resource = node->getResource();
	if (resource->getType() == Resource::MODEL) {
		Model* obj = static_cast<Model*>(resource);
		obj->setShader(shader);
	} else if (resource->getType() == Resource::SPRITE) {
		Sprite* spr = static_cast<Sprite*>(resource);
		spr->setShader(shader);
	}
	SM_POP_ARGS(L, SM_GET_ARGUMENT_COUNT(L));
	return 0;
}

int nodeGetShader(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	Resource* resource = node->getResource();
	if (resource->getType() == Resource::MODEL) {
		Model* obj = static_cast<Model*>(resource);
		SM_RETURN_OBJECT(L, "Shader", Shader, obj->getShader());
	} else if (resource->getType() == Resource::SPRITE) {
		Sprite* spr = static_cast<Sprite*>(resource);
		SM_RETURN_OBJECT(L, "Shader", Shader, spr->getShader());
	}
	return 0;
}

int nodeSetTexture(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	int argc = SM_GET_ARGUMENT_COUNT(L);
	ASSERT(argc == 2 || argc == 3, "addTexture method expects argument of type Texture.");
	Texture* texture = SM_GET_OBJECT(L, 1, Texture);
	if (node->getResource()->getType() == Resource::SPRITE) {
		Sprite* obj = static_cast<Sprite*>(node->getResource());
		if (obj != 0) {
			obj->setTexture(texture);
		}
	} else if (node->getResource()->getType() == Resource::MODEL) {
		Model* obj = static_cast<Model*>(node->getResource());
		if (obj != 0) {
			obj->setTexture(texture, argc == 3 ? SM_GET_INT(L, 2) : 0);
		}
	}
	return 0;
}

int nodeSetSprite(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	INT32 index = SM_GET_INT(L, 1);
	Sprite* obj = static_cast<Sprite*>(node->getResource());
	if (obj != 0) {
		obj->setSpriteIndex(index);
	}
	return 0;
}

int nodeGetSpriteCount(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	Sprite* obj = static_cast<Sprite*>(node->getResource());
    SIZE indexCount = 0;
	if (obj != 0) {
		indexCount = obj->getIndexCount();
	}
	SM_POP_ARGS(L, SM_GET_ARGUMENT_COUNT(L));
	return 1;
}

int getName(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	SM_RETURN_STRING(L, node->getName().c_str());
	return 1;
}

int setVisibility(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	bool state = SM_GET_BOOL(L, 1);
	node->setState(Node::RENDERABLE, state);
	return 0;
}

int getVisibility(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	SM_RETURN_BOOL(L, node->getState(Node::RENDERABLE));
	return 1;
}

/**int getVehicle(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	Vehicle* vehicle = SM_GET_PM()->getVehicle(node);
	SM_RETURN_OBJECT(L, "Vehicle", Vehicle, vehicle);
	return 1;
}*/

int moveX(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	if (node == 0) {
		return 0;
	}
	float distance = SM_GET_FLOAT(L, 1);
	Vec3 move(distance, 0.0f, 0.0f);
	Mat4 inv;
	Vec3 pos = node->getPos();
	Quaternion rot = node->getRot();
	rot.invert();
	Mat4 mPos;
	Matrix::translate(mPos, -pos.getX(), -pos.getY(), -pos.getZ());
	Matrix::multiply(mPos, rot.getMatrix(), inv);
	Matrix::multiply(move, inv);
	node->getPos().addXYZ(move.getX(), move.getY(), move.getZ());
	node->setState(Node::POSITION, true);
	return 0;
}

int moveY(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	if (node == 0) {
		return 0;
	}
	float distance = SM_GET_FLOAT(L, 1);
	Vec3 move(0.0f, distance, 0.0f);
	Mat4 inv;
	Vec3 pos = node->getPos();
	Quaternion rot = node->getRot();
	rot.invert();
	Mat4 mPos;
	Matrix::translate(mPos, -pos.getX(), -pos.getY(), -pos.getZ());
	Matrix::multiply(mPos, rot.getMatrix(), inv);
	Matrix::multiply(move, inv);
	node->getPos().addXYZ(move.getX(), move.getY(), move.getZ());
	node->setState(Node::POSITION, true);
	return 0;
}

int moveZ(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	if (node == 0) {
		return 0;
	}
	float distance = SM_GET_FLOAT(L, 1);
	Vec3 move(0.0f, 0.0f, distance);
	Mat4 inv;
	Vec3 pos = node->getPos();
	Quaternion rot = node->getRot();
	rot.invert();
	Mat4 mPos;
	Matrix::translate(mPos, -pos.getX(), -pos.getY(), -pos.getZ());
	Matrix::multiply(mPos, rot.getMatrix(), inv);
	Matrix::multiply(move, inv);
	node->getPos().addXYZ(move.getX(), move.getY(), move.getZ());
	node->setState(Node::POSITION, true);
	return 0;
}

int nodeGuiTextSetSize(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	GUIText* text = dynamic_cast<GUIText*>(node->getResource());
	if (text == 0) {
		LOGW("Node does not contain GUIText or its children resource.");
		return 0;
	}
	INT32 size = SM_GET_INT(L, 1);
	text->setTextSize(size);
    return 0;
}

int nodeGuiTextSetText(lua_State* L) {
    Node* node = SM_GET_OBJECT(L, 0, Node);
	GUIText* text = dynamic_cast<GUIText*>(node->getResource());
	if (text == 0) {
		LOGW("Node does not contain GUIText or its children resource.");
		return 0;
	}
	string val = SM_GET_STRING(L, 1);
	text->setText(val);
    return 0;
}

int nodeGuiTextSetColor(lua_State* L) {
    Node* node = SM_GET_OBJECT(L, 0, Node);
	GUIText* text = dynamic_cast<GUIText*>(node->getResource());
	if (text == 0) {
		LOGW("Node does not contain GUIText or its children resource.");
		return 0;
	}
	string val = SM_GET_STRING(L, 1);
	text->getDiffuse().setRGBA(val);
	return 0;
}

int nodeGuiSetBackground(lua_State* L) {
    Node* node = SM_GET_OBJECT(L, 0, Node);
	ASSERT(SM_IS_STRING(L, 1) || SM_IS_OBJECT(L, 1),
		"Argument type for setBackground() must be string or texture object.");
	GUISurface* surface = dynamic_cast<GUISurface*>(node->getResource());
	if (surface == 0) {
		LOGW("Node does not contain GUISurface or its children resource.");
		return 0;
	}
	if (SM_IS_STRING(L, 1)) {
		surface->setBackground(SM_GET_STRING(L, 1));
	} else {
		Texture* texture = SM_GET_OBJECT(L, 1, Texture);
		if (texture == 0) {
			LOGW("Specified object to be used as background must be of type Texture.");
			return 0;
		}
		surface->setBackground(texture->getName());
	}
	return 0;
}

int nodeGuiGetBackground(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	GUISurface* surface = dynamic_cast<GUISurface*>(node->getResource());
	if (surface == 0) {
		LOGW("Node does not contain GUISurface resource.");
		return 0;
	}
	SM_RETURN_STRING(L, surface->getBackground().c_str());
	return 1;
}

int nodeGetWidth(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	GUISurface* surface = dynamic_cast<GUISurface*>(node->getResource());
	if (surface == 0) {
		LOGW("Node does not contain GUISurface resource.");
		return 0;
	}
	SM_RETURN_FLOAT(L, surface->getWidth());
	return 1;
}

int nodeGetHeight(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	GUISurface* surface = dynamic_cast<GUISurface*>(node->getResource());
	if (surface == 0) {
		LOGW("Node does not contain GUISurface resource.");
		return 0;
	}
	SM_RETURN_FLOAT(L, surface->getHeight());
	return 1;
}

int nodeSetWidth(lua_State* L) {
    Node* node = SM_GET_OBJECT(L, 0, Node);
	GUISurface* surface = dynamic_cast<GUISurface*>(node->getResource());
	if (surface == 0) {
		LOGW("Node does not contain GUISurface resource.");
		return 0;
	}
	float val = SM_GET_FLOAT(L, 1);
	surface->setWidth(val);
	return 0;
}

int nodeSetHeight(lua_State* L) {
    Node* node = SM_GET_OBJECT(L, 0, Node);
	GUISurface* surface = dynamic_cast<GUIText*>(node->getResource());
	if (surface == 0) {
		LOGW("Node does not contain GUISurface resource.");
		return 0;
	}
	float val = SM_GET_FLOAT(L, 1);
	surface->setWidth(val);
	return 0;
}

int nodeSetMarginLeft(lua_State* L) {
    Node* node = SM_GET_OBJECT(L, 0, Node);
	GUISurface* surface = dynamic_cast<GUISurface*>(node->getResource());
	if (surface == 0) {
		LOGW("Node does not contain GUISurface resource.");
		return 0;
	}
	float val = SM_GET_FLOAT(L, 1);
	surface->setMargins(val, surface->getMarginBottom(),
		surface->getMarginRight(), surface->getMarginTop());
	return 0;
}

int nodeGetMarginLeft(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	GUISurface* surface = dynamic_cast<GUISurface*>(node->getResource());
	if (surface == 0) {
		LOGW("Node does not contain GUISurface resource.");
		return 0;
	}
	SM_RETURN_FLOAT(L, surface->getMarginLeft());
	return 1;
}

int nodeSetMarginBottom(lua_State* L) {
    Node* node = SM_GET_OBJECT(L, 0, Node);
	GUISurface* surface = dynamic_cast<GUISurface*>(node->getResource());
	if (surface == 0) {
		LOGW("Node does not contain GUISurface resource.");
		return 0;
	}
	float val = SM_GET_FLOAT(L, 1);
	surface->setMargins(surface->getMarginLeft(), val,
		surface->getMarginRight(), surface->getMarginTop());
	return 0;
}

int nodeGetMarginBottom(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	GUISurface* surface = dynamic_cast<GUISurface*>(node->getResource());
	if (surface == 0) {
		LOGW("Node does not contain GUISurface resource.");
		return 0;
	}
	SM_RETURN_FLOAT(L, surface->getMarginBottom());
	return 1;
}

int nodeSetAmbient(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	Model* model = dynamic_cast<Model*>(node->getResource());
	ASSERT(model != 0, "Node does not contain model resource.");
	ASSERT(SM_GET_ARGUMENT_COUNT(L) == 2, "Method setAmbient expects 1 parameter of type string.");
	string ambient = SM_GET_STRING(L, 1);
	model->setAmbient(Color(ambient));
	return 0;
}

int nodeSetType(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	Model* model = dynamic_cast<Model*>(node->getResource());
	ASSERT(model != 0, "Node does not contain model resource.");
	ASSERT(SM_GET_ARGUMENT_COUNT(L) == 2, "Method setType expects 1 parameter of type string.");
	string type = SM_GET_STRING(L, 1);
	model->getAttributes().setString(Model::ATTR_TYPE, type);
	model->release();
	model->create();
	return 0;
}

#define NODE_ADD_SET(method_name, method_call, state) \
	METHOD_ADD_SET_STATE(Node, method_name, method_call, state)
#define NODE_GET(method_name, method_call) \
	METHOD_GET(Node, method_name, method_call)
#define NODE_ADD_SET_TWO(method_name, method1, method2, state) \
	METHOD_ADD_SET_TWO_STATE(Node, method_name, method1, method2, state)
#define NODE_GET_TWO(method_name, method1, method2) \
	METHOD_GET_TWO(Node, method_name, method1, method2)
#define NODE_ADD_SET_THREE(method_name, method, state) \
	METHOD_ADD_SET_THREE_STATE(Node, method_name, method, state)
#define NODE_GET_THREE(method_name, method) \
	METHOD_GET_THREE(Node, method_name, method)
#define NODE_GET_THREE_ROT(method_name, method1, method2, method3) \
	METHOD_GET_THREE_ROT(Node, method_name, method1, method2, method3)

NODE_ADD_SET(addPosX, getPos().addX, Node::POSITION)
NODE_ADD_SET(addPosY, getPos().addY, Node::POSITION)
NODE_ADD_SET(addPosZ, getPos().addZ, Node::POSITION)
NODE_ADD_SET(addRotX, getRot().addX, Node::ROTATION)
NODE_ADD_SET(addRotY, getRot().addY, Node::ROTATION)
NODE_ADD_SET(addRotZ, getRot().addZ, Node::ROTATION)
NODE_ADD_SET(addScaleX, getScale().addX, Node::SCALING)
NODE_ADD_SET(addScaleY, getScale().addY, Node::SCALING)
NODE_ADD_SET(addScaleZ, getScale().addZ, Node::SCALING)
NODE_ADD_SET(setPosX, getPos().setX, Node::POSITION)
NODE_ADD_SET(setPosY, getPos().setY, Node::POSITION)
NODE_ADD_SET(setPosZ, getPos().setZ, Node::POSITION)
NODE_ADD_SET(setRotX, getRot().setX, Node::ROTATION)
NODE_ADD_SET(setRotY, getRot().setY, Node::ROTATION)
NODE_ADD_SET(setRotZ, getRot().setZ, Node::ROTATION)
NODE_ADD_SET(setScaleX, getScale().setX, Node::SCALING)
NODE_ADD_SET(setScaleY, getScale().setY, Node::SCALING)
NODE_ADD_SET(setScaleZ, getScale().setZ, Node::SCALING)
NODE_GET(getPosX, getPos().getX())
NODE_GET(getPosY, getPos().getY())
NODE_GET(getPosZ, getPos().getZ())
NODE_GET(getRotX, getRot().getX())
NODE_GET(getRotY, getRot().getY())
NODE_GET(getRotZ, getRot().getZ())
NODE_GET(getScaleX, getScale().getX())
NODE_GET(getScaleY, getScale().getY())
NODE_GET(getScaleZ, getScale().getZ())
NODE_ADD_SET_TWO(addPosXY, getPos().addX, getPos().addY, Node::POSITION)
NODE_ADD_SET_TWO(addPosXZ, getPos().addX, getPos().addZ, Node::POSITION)
NODE_ADD_SET_TWO(addPosYZ, getPos().addY, getPos().addZ, Node::POSITION)
NODE_ADD_SET_TWO(addRotXY, getRot().addX, getRot().addY, Node::ROTATION)
NODE_ADD_SET_TWO(addRotXZ, getRot().addX, getRot().addZ, Node::ROTATION)
NODE_ADD_SET_TWO(addRotYZ, getRot().addY, getRot().addZ, Node::ROTATION)
NODE_ADD_SET_TWO(addScaleXY, getScale().addX, getScale().addY, Node::SCALING)
NODE_ADD_SET_TWO(addScaleXZ, getScale().addX, getScale().addZ, Node::SCALING)
NODE_ADD_SET_TWO(addScaleYZ, getScale().addY, getScale().addZ, Node::SCALING)
NODE_ADD_SET_TWO(setPosXY, getPos().setX, getPos().setY, Node::POSITION)
NODE_ADD_SET_TWO(setPosXZ, getPos().setX, getPos().setZ, Node::POSITION)
NODE_ADD_SET_TWO(setPosYZ, getPos().setY, getPos().setZ, Node::POSITION)
NODE_ADD_SET_TWO(setRotXY, getRot().setX, getRot().setY, Node::ROTATION)
NODE_ADD_SET_TWO(setRotXZ, getRot().setX, getRot().setZ, Node::ROTATION)
NODE_ADD_SET_TWO(setRotYZ, getRot().setY, getRot().setZ, Node::ROTATION)
NODE_ADD_SET_TWO(setScaleXY, getScale().setX, getScale().setY, Node::SCALING)
NODE_ADD_SET_TWO(setScaleXZ, getScale().setX, getScale().setZ, Node::SCALING)
NODE_ADD_SET_TWO(setScaleYZ, getScale().setY, getScale().setZ, Node::SCALING)
NODE_GET_TWO(getPosXY, getPos().getX(), getPos().getY())
NODE_GET_TWO(getPosXZ, getPos().getX(), getPos().getZ())
NODE_GET_TWO(getPosYZ, getPos().getY(), getPos().getZ())
NODE_GET_TWO(getRotXY, getRot().getX(), getRot().getY())
NODE_GET_TWO(getRotXZ, getRot().getX(), getRot().getZ())
NODE_GET_TWO(getRotYZ, getRot().getY(), getRot().getZ())
NODE_GET_TWO(getScaleXY, getScale().getX(), getScale().getY())
NODE_GET_TWO(getScaleXZ, getScale().getX(), getScale().getZ())
NODE_GET_TWO(getScaleYZ, getScale().getY(), getScale().getZ())
NODE_ADD_SET_THREE(addPosXYZ, getPos().setXYZ, Node::POSITION)
NODE_ADD_SET_THREE(addRotXYZ, getRot().setXYZ, Node::ROTATION)
NODE_ADD_SET_THREE(addScaleXYZ, getScale().setXYZ, Node::SCALING)
NODE_ADD_SET_THREE(setPosXYZ, getPos().setXYZ, Node::POSITION)
NODE_ADD_SET_THREE(setRotXYZ, getRot().setXYZ, Node::ROTATION)
NODE_ADD_SET_THREE(setScaleXYZ, getScale().setXYZ, Node::SCALING)
NODE_GET_THREE(getPosXYZ, getPos().getPointer())
NODE_GET_THREE_ROT(getRotXYZ, getRot().getX(), getRot().getY(), getRot().getZ())
NODE_GET_THREE(getScaleXYZ, getScale().getPointer())

void registerNode() {
    unordered_map<string, int (*)(lua_State*)> methods;
	ADD_METHOD(methods, "getNode", getNode);
    ADD_METHOD(methods, "__gc", deleteNode);
	ADD_METHOD(methods, "setParent", nodeSetParent);
	ADD_METHOD(methods, "addChild", nodeAddChild);
	ADD_METHOD(methods, "enablePhysics", nodeEnablePhysics);
	ADD_METHOD(methods, "disablePhysics", nodeDisablePhysics);
	ADD_METHOD(methods, "setMass", nodeSetMass);
	ADD_METHOD(methods, "accelerate", nodeAccelerate);
	ADD_METHOD(methods, "turn", nodeTurn);
    ADD_METHOD(methods, "setShader", nodeSetShader);
    ADD_METHOD(methods, "getShader", nodeGetShader);
	ADD_METHOD(methods, "setTexture", nodeSetTexture);
    ADD_METHOD(methods, "setIndex", nodeSetSprite);
    ADD_METHOD(methods, "getCount", nodeGetSpriteCount);
    ADD_METHOD(methods, "getName", getName);
    ADD_METHOD(methods, "setVisibility", setVisibility);
    ADD_METHOD(methods, "getVisibility", getVisibility);
	//ADD_METHOD(methods, "getVehicle", getVehicle);
	ADD_METHOD(methods, "setFontSize", nodeGuiTextSetSize);
	ADD_METHOD(methods, "setText", nodeGuiTextSetText);
	ADD_METHOD(methods, "setColor", nodeGuiTextSetColor);
	ADD_METHOD(methods, "setBackground", nodeGuiSetBackground);
	ADD_METHOD(methods, "getBackground", nodeGuiGetBackground);
	ADD_METHOD(methods, "getWidth", nodeGetWidth);
	ADD_METHOD(methods, "getHeight", nodeGetHeight);
	ADD_METHOD(methods, "setWidth", nodeSetWidth);
	ADD_METHOD(methods, "setHeight", nodeSetHeight);
	ADD_METHOD(methods, "setMarginLeft", nodeSetMarginLeft);
	ADD_METHOD(methods, "getMarginLeft", nodeGetMarginLeft);
	ADD_METHOD(methods, "setMarginBottom", nodeSetMarginBottom);
	ADD_METHOD(methods, "getMarginBottom", nodeGetMarginBottom);
	ADD_METHOD(methods, "setAmbient", nodeSetAmbient);
	ADD_METHOD(methods, "setType", nodeSetType);
    ADD_METHOD(methods, "moveX", moveX);
    ADD_METHOD(methods, "moveY", moveY);
    ADD_METHOD(methods, "moveZ", moveZ);
    ADD_METHOD(methods, "addPosX", addPosX);
    ADD_METHOD(methods, "setPosX", setPosX);
    ADD_METHOD(methods, "getPosX", getPosX);
    ADD_METHOD(methods, "addPosY", addPosY);
    ADD_METHOD(methods, "setPosY", setPosY);
    ADD_METHOD(methods, "getPosY", getPosY);
    ADD_METHOD(methods, "addPosZ", addPosZ);
    ADD_METHOD(methods, "setPosZ", setPosZ);
    ADD_METHOD(methods, "getPosZ", getPosZ);
    ADD_METHOD(methods, "addRotX", addRotX);
    ADD_METHOD(methods, "setRotX", setRotX);
    ADD_METHOD(methods, "getRotX", getRotX);
    ADD_METHOD(methods, "addRotY", addRotY);
    ADD_METHOD(methods, "setRotY", setRotY);
    ADD_METHOD(methods, "getRotY", getRotY);
    ADD_METHOD(methods, "addRotZ", addRotZ);
    ADD_METHOD(methods, "setRotZ", setRotZ);
    ADD_METHOD(methods, "getRotZ", getRotZ);
    ADD_METHOD(methods, "addScaleX", addScaleX);
    ADD_METHOD(methods, "setScaleX", setScaleX);
    ADD_METHOD(methods, "getScaleX", getScaleX);
    ADD_METHOD(methods, "addScaleY", addScaleY);
    ADD_METHOD(methods, "setScaleY", setScaleY);
    ADD_METHOD(methods, "getScaleY", getScaleY);
    ADD_METHOD(methods, "addScaleZ", addScaleZ);
    ADD_METHOD(methods, "setScaleZ", setScaleZ);
    ADD_METHOD(methods, "getScaleZ", getScaleZ);
	ADD_METHOD(methods, "addPosXY", addPosXY);
	ADD_METHOD(methods, "addPosXZ", addPosXZ);
	ADD_METHOD(methods, "addPosYZ", addPosYZ);
	ADD_METHOD(methods, "addRotXY", addRotXY);
	ADD_METHOD(methods, "addRotXZ", addRotXZ);
	ADD_METHOD(methods, "addRotYZ", addRotYZ);
	ADD_METHOD(methods, "addScaleXY", addScaleXY);
	ADD_METHOD(methods, "addScaleXZ", addScaleXZ);
	ADD_METHOD(methods, "addScaleYZ", addScaleYZ);
	ADD_METHOD(methods, "setPosXY", setPosXY);
	ADD_METHOD(methods, "setPosXZ", setPosXZ);
	ADD_METHOD(methods, "setPosYZ", setPosYZ);
	ADD_METHOD(methods, "setRotXY", setRotXY);
	ADD_METHOD(methods, "setRotXZ", setRotXZ);
	ADD_METHOD(methods, "setRotYZ", setRotYZ);
	ADD_METHOD(methods, "setScaleXY", setScaleXY);
	ADD_METHOD(methods, "setScaleXZ", setScaleXZ);
	ADD_METHOD(methods, "setScaleYZ", setScaleYZ);
	ADD_METHOD(methods, "getPosXY", getPosXY);
	ADD_METHOD(methods, "getPosXZ", getPosXZ);
	ADD_METHOD(methods, "getPosYZ", getPosYZ);
	ADD_METHOD(methods, "getRotXY", getRotXY);
	ADD_METHOD(methods, "getRotXZ", getRotXZ);
	ADD_METHOD(methods, "getRotYZ", getRotYZ);
	ADD_METHOD(methods, "getScaleXY", getScaleXY);
	ADD_METHOD(methods, "getScaleXZ", getScaleXZ);
	ADD_METHOD(methods, "getScaleYZ", getScaleYZ);
	ADD_METHOD(methods, "addPosXYZ", addPosXYZ);
	ADD_METHOD(methods, "addRotXYZ", addRotXYZ);
	ADD_METHOD(methods, "addScaleXYZ", addScaleXYZ);
	ADD_METHOD(methods, "setPosXYZ", setPosXYZ);
	ADD_METHOD(methods, "setRotXYZ", setRotXYZ);
	ADD_METHOD(methods, "setScaleXYZ", setScaleXYZ);
	ADD_METHOD(methods, "getPosXYZ", getPosXYZ);
	ADD_METHOD(methods, "getRotXYZ", getRotXYZ);
	ADD_METHOD(methods, "getScaleXYZ", getScaleXYZ);
    ScriptManager::addFunction("getNode", getNode);
    ScriptManager::addClass("Node", methods);
}

int getInput(lua_State* L) {
	Input* input = SM_GET_SL()->getInput();
	SM_RETURN_OBJECT(L, "Input", Input, input);
    return 1;
}

int inputGetPointerX(lua_State* L) {
	Input* input = SM_GET_OBJECT(L, 0, Input);
	SM_RETURN_INT(L, input->getPointerX());
	return 1;
}

int inputGetPointerY(lua_State* L) {
	Input* input = SM_GET_OBJECT(L, 0, Input);
	SM_RETURN_INT(L, input->getPointerY());
	return 1;
}

int inputGetPointerDeltaX(lua_State* L) {
	Input* input = SM_GET_OBJECT(L, 0, Input);
	SM_RETURN_INT(L, input->getPointerDeltaX());
	return 1;
}

int inputGetPointerDeltaY(lua_State* L) {
	Input* input = SM_GET_OBJECT(L, 0, Input);
	SM_RETURN_INT(L, input->getPointerDeltaY());
	return 1;
}

int inputKeyPressed(lua_State* L) {
	Input* input = SM_GET_OBJECT(L, 0, Input);
	int key = SM_GET_INT(L, 1);
	if (input->keyPressed(key)) {
		SM_RETURN_BOOL(L, true);
	}
	else {
		SM_RETURN_BOOL(L, false);
	}
	return 1;
}

int inputKeyReleased(lua_State* L) {
	Input* input = SM_GET_OBJECT(L, 0, Input);
	int key = SM_GET_INT(L, 1);
	if (input->keyReleased(key)) {
		SM_RETURN_BOOL(L, true);
	}
	else {
		SM_RETURN_BOOL(L, false);
	}
	return 1;
}

void registerInput() {
    unordered_map<string, int (*)(lua_State*)> methods;
    ADD_METHOD(methods, "getPointerX", inputGetPointerX);
	ADD_METHOD(methods, "getPointerY", inputGetPointerY);
	ADD_METHOD(methods, "getPointerDeltaX", inputGetPointerDeltaX);
	ADD_METHOD(methods, "getPointerDeltaY", inputGetPointerDeltaY);
	ADD_METHOD(methods, "keyPressed", inputKeyPressed);
	ADD_METHOD(methods, "keyReleased", inputKeyReleased);
    ScriptManager::addFunction("getInput", getInput);
    ScriptManager::addClass("Input", methods);
}

int newModel(lua_State* L) {
	int argc = SM_GET_ARGUMENT_COUNT(L);
	ASSERT(argc == 1, "Model constructor expects name as parameter.");
	string name = SM_GET_STRING(L, 0);
	Model* model = static_cast<Model*>(SM_GET_RM()->get(
		Resource::MODEL, name));
	if (model == 0) {
		model = NEW Model(ScriptManager::getServiceLocator());
		SM_GET_RM()->add(name, model);
	}
	Node* node = NEW Node(name + "_ModelResource", model);
	SM_GET_SL()->getRootNode()->addChild(node);
	node->setParent(SM_GET_SL()->getRootNode());
	SM_RETURN_OBJECT(L, "Node", Node, node);
    return 1;
}

void registerModel() {
    unordered_map<string, int (*)(lua_State*)> methods;
    ADD_METHOD(methods, "new", newModel);
    ScriptManager::addClass("Model", methods);
}

int newSprite(lua_State* L) {
	int argc = SM_GET_ARGUMENT_COUNT(L);
	ASSERT(argc >= 1 && argc <= 4, "Argument count for Sprite constructor is invalid.");
	string val = SM_GET_STRING(L, 0);
	Sprite* sprite = static_cast<Sprite*>(SM_GET_RM()->get(
		Resource::SPRITE, val));
	if (sprite == 0) {
		sprite = NEW Sprite(ScriptManager::getServiceLocator());
		if (argc == 3 || argc == 4) {
			float width = SM_GET_FLOAT(L, 1);
			float height = SM_GET_FLOAT(L, 2);
			sprite->getAttributes().setFloat(Sprite::ATTR_WIDTH, width);
			sprite->getAttributes().setFloat(Sprite::ATTR_HEIGHT, height);
		} else if (argc == 2) {
			Texture* texture = SM_GET_OBJECT(L, 1, Texture);
			ASSERT(texture != 0, "Texture for sprite %s is null.", val.c_str());
			float width = (float) texture->getWidth();
			float height = (float) texture->getHeight();
			sprite->getAttributes().setFloat(Sprite::ATTR_WIDTH, width);
			sprite->getAttributes().setFloat(Sprite::ATTR_HEIGHT, height);
		}
		sprite->create();
		SM_GET_RM()->add(val, sprite);
	}
	if (argc == 4 || argc == 2) {
		int index = argc == 2 ? 1 : 3;
		Texture* texture = SM_GET_OBJECT(L, index, Texture);
		if (texture != 0) {
			sprite->setTexture(texture);
		}
	}
	Node* node = NEW Node(val, sprite);
	ScriptManager::getServiceLocator()->getRootNode()->addChild(node);
	node->setParent(ScriptManager::getServiceLocator()->getRootNode());
	SM_RETURN_OBJECT(L, "Node", Node, node);
    return 1;
}

void registerSprite() {
    unordered_map<string, int (*)(lua_State*)> methods;
    ADD_METHOD(methods, "new", newSprite);
	ScriptManager::addClass("Sprite", methods);
}

int newTexture(lua_State* L) {
	int argc = SM_GET_ARGUMENT_COUNT(L);
	ASSERT(SM_IS_STRING(L, 0), "First argument to Texture constructor must be name.");
	ASSERT(argc == 1 || argc == 2 || argc == 4 || argc == 8, "Wrong argument count for Texture constructor.");
	string name = SM_GET_STRING(L, 0);
	ASSERT(static_cast<Texture*>(SM_GET_RM()->get(Resource::TEXTURE_2D, name)) == 0,
		"Texture with name %s already exists.", name.c_str());
	Texture* texture = 0;
	bool alpha = false;
	if (argc == 1) {
		texture = Texture::load(SM_GET_SL(), name, false);
	} else if (argc == 2) {
		if (SM_IS_STRING(L, 1)) {
			string type = SM_GET_STRING(L, 1);
			if (type == Texture::VAL_MONO) {
				texture = Texture::createMono(SM_GET_SL());
			} else if (type == Texture::VAL_RGB) {
				texture = Texture::createRGB(SM_GET_SL());
			} else {
				texture = Texture::createRGBA(SM_GET_SL());
			}
		} else {
			HttpResponse* response = SM_GET_OBJECT(L, 1, HttpResponse);
			texture = Texture::load(SM_GET_SL(), response->getContent());
		}
	} else {
		string type = SM_GET_STRING(L, 1);
		int width = SM_GET_INT(L, 2);
		int height = SM_GET_INT(L, 3);
		if (type == Texture::VAL_MONO) {
			texture = Texture::createMono(SM_GET_SL(), width, height);
		} else if (type == Texture::VAL_RGB) {
			texture = Texture::createRGB(SM_GET_SL(), width, height);
		} else {
			texture = Texture::createRGBA(SM_GET_SL(), width, height);
		}
	}
	if (argc == 7) {
		UINT8 color[] = {
			SM_GET_BYTE(L, 4),
			SM_GET_BYTE(L, 5),
			SM_GET_BYTE(L, 6)
		};
		texture->rectangle(0, 0, texture->getWidth(), texture->getHeight(), color);
	}
	else if (argc == 8) {
		UINT8 color[] = {
			SM_GET_BYTE(L, 4),
			SM_GET_BYTE(L, 5),
			SM_GET_BYTE(L, 6),
			SM_GET_BYTE(L, 7)
		};
		texture->rectangle(0, 0, texture->getWidth(), texture->getHeight(), color);
	}
	SM_GET_RM()->add(name, texture);
	SM_RETURN_OBJECT(L, "Texture", Texture, texture);
    return 1;
}

int deleteTexture(lua_State* L) {
    return 0;
}

int textureSetPixel(lua_State* L) {
    Texture* texture = SM_GET_OBJECT(L, 0, Texture);
	UINT8 color[] = {
		SM_GET_BYTE(L, 3),
		SM_GET_BYTE(L, 4),
		SM_GET_BYTE(L, 5),
		SM_GET_BYTE(L, 6)
	};
	texture->setPixel(color, SM_GET_INT(L, 1), SM_GET_INT(L, 2));
    return 0;
}

int textureLine(lua_State* L) {
    Texture* texture = SM_GET_OBJECT(L, 0, Texture);
	UINT8 color[] = {
		SM_GET_BYTE(L, 5),
		SM_GET_BYTE(L, 6),
		SM_GET_BYTE(L, 7),
		SM_GET_BYTE(L, 8)
	};
	texture->line(SM_GET_INT(L, 1), SM_GET_INT(L, 2),
		SM_GET_INT(L, 3), SM_GET_INT(L, 4), color);
    return 0;
}

int textureRectangle(lua_State* L) {
    Texture* texture = SM_GET_OBJECT(L, 0, Texture);
	UINT8 color[] = {
		SM_GET_BYTE(L, 5),
		SM_GET_BYTE(L, 6),
		SM_GET_BYTE(L, 7),
		SM_GET_BYTE(L, 8)
	};
	texture->rectangle(SM_GET_INT(L, 1), SM_GET_INT(L, 2),
		SM_GET_INT(L, 3), SM_GET_INT(L, 4), color);
    return 0;
}

int textureCircle(lua_State* L) {
    Texture* texture = SM_GET_OBJECT(L, 0, Texture);
	UINT8 color[] = {
		SM_GET_BYTE(L, 4),
		SM_GET_BYTE(L, 5),
		SM_GET_BYTE(L, 6),
		SM_GET_BYTE(L, 7)
	};
	texture->circle(SM_GET_INT(L, 1), SM_GET_INT(L, 2),
		SM_GET_INT(L, 3), color);
    return 0;
}

int textureFilledCircle(lua_State* L) {
    Texture* texture = SM_GET_OBJECT(L, 0, Texture);
	UINT8 color[] = {
		SM_GET_BYTE(L, 4),
		SM_GET_BYTE(L, 5),
		SM_GET_BYTE(L, 6),
		SM_GET_BYTE(L, 7)
	};
	texture->filledCircle(SM_GET_INT(L, 1), SM_GET_INT(L, 2),
		SM_GET_INT(L, 3), color);
    return 0;
}

int textureCommit(lua_State* L) {
    Texture* texture = SM_GET_OBJECT(L, 0, Texture);
	texture->commit();
    return 0;
}

void registerTexture() {
	unordered_map<string, int (*)(lua_State*)> methods;
	ADD_METHOD(methods, "new", newTexture);
	ADD_METHOD(methods, "setPixel", ::textureSetPixel);
	ADD_METHOD(methods, "line", ::textureLine);
	ADD_METHOD(methods, "rectangle", ::textureRectangle);
	ADD_METHOD(methods, "circle", ::textureCircle);
	ADD_METHOD(methods, "filledCircle", ::textureFilledCircle);
	ADD_METHOD(methods, "commit", ::textureCommit);
    ADD_METHOD(methods, "__gc", ::deleteTexture);
    ScriptManager::addClass("Texture", methods);
}

int surfaceSetWidth(lua_State* L) {
    GUISurface* surface = SM_GET_OBJECT(L, 0, GUISurface);
	float val = SM_GET_FLOAT(L, 1);
	surface->setWidth(val);
    return 0;
}

int surfaceGetWidth(lua_State* L) {
	GUISurface* surface = SM_GET_OBJECT(L, 0, GUISurface);
	SM_RETURN_INT(L, (int) surface->getWidth());
	return 1;
}

void registerGUISurface() {
    unordered_map<string, int (*)(lua_State*)> methods;
	//ADD_METHOD(methods, "new", newContainer);
	//ADD_METHOD(methods, "__gc", deleteContainer);
	//ADD_METHOD(methods, "setWidth", containerSetWidth);
	//ADD_METHOD(methods, "getWidth", containerGetWidth);
	//ADD_METHOD(methods, "setHeight", containerSetHeight);
	//ADD_METHOD(methods, "getHeight", containerGetWidth);
	//ADD_METHOD(methods, "setMarginLeft", containerSetMarginLeft);
	//ADD_METHOD(methods, "getMarginLeft", containerGetMarginLeft);
	//ADD_METHOD(methods, "setMarginBottom", containerSetMarginBottom);
	//ADD_METHOD(methods, "getMarginBottom", containerGetMarginBottom);
	//ADD_METHOD(methods, "setTransparency", containerSetTransparency);
	//ADD_METHOD(methods, "getTransparency", containerGetTransparency);
    ScriptManager::addClass("Surface", methods);
}

int newGUIText(lua_State* L) {
	int argc = SM_GET_ARGUMENT_COUNT(L);
	ASSERT(argc == 1, "Wrong argument count for GUIText.new");
	string name = SM_GET_STRING(L, 0);
	GUIText* text = dynamic_cast<GUIText*>(SM_GET_RM()->get(Resource::GUI_SURFACE, name));
	if (text == 0) {
		text = NEW GUIText(SM_GET_SL());
		text->getAttributes().setString(GUIText::ATTR_TEXT, "");
        text->getAttributes().setString(GUIText::ATTR_SCREEN_LEFT, "true");
        text->getAttributes().setString(GUIText::ATTR_SCREEN_RIGHT, "true");
        text->getAttributes().setString(GUIText::ATTR_SCREEN_TOP, "true");
		text->create();
		SM_GET_RM()->add(name, text);
	}
	Node* node = NEW Node(name + "_TextResource", text);
	ScriptManager::getServiceLocator()->getRootNode()->addChild(node);
	node->setParent(ScriptManager::getServiceLocator()->getRootNode());
	SM_RETURN_OBJECT(L, "Node", Node, node);
    return 1;
}

void registerGUIText() {
    unordered_map<string, int (*)(lua_State*)> methods;
	ADD_METHOD(methods, "new", newGUIText);
    ScriptManager::addClass("GUIText", methods);
}

int newGUIButton(lua_State* L) {
	int argc = SM_GET_ARGUMENT_COUNT(L);
	ASSERT(argc == 1 || argc == 3 || argc == 5, "Wrong argument count for GUIButton constructor.");
	ASSERT(SM_IS_STRING(L, 0), "First argument for GUIButton must be string type.");
	string name = SM_GET_STRING(L, 0);
	ASSERT(dynamic_cast<GUIButton*>(SM_GET_RM()->get(Resource::GUI_SURFACE, name)) == 0,
		"GUIButton object with this name already exists.");
	GUIButton* button = NEW GUIButton(SM_GET_SL());
	if (argc == 3) {
		ASSERT(SM_IS_NUMBER(L, 1) && SM_IS_NUMBER(L, 2),
			"Second and third arguments must be numbers for GUIButton constructor.");
		button->getAttributes().setFloat(GUIText::ATTR_WIDTH, SM_GET_FLOAT(L, 1));
		button->getAttributes().setFloat(GUIText::ATTR_HEIGHT, SM_GET_FLOAT(L, 2));
	} else if (argc == 5) {
		ASSERT(SM_IS_NUMBER(L, 1) && SM_IS_NUMBER(L, 2)
			&& SM_IS_NUMBER(L, 3) && SM_IS_NUMBER(L, 4),
			"Arguments from second to fifth must be numbers for GUIButton constructor.");
		button->getAttributes().setFloat(GUIText::ATTR_MARGIN_LEFT, SM_GET_FLOAT(L, 1));
		button->getAttributes().setFloat(GUIText::ATTR_MARGIN_BOTTOM, SM_GET_FLOAT(L, 2));
		button->getAttributes().setFloat(GUIText::ATTR_WIDTH, SM_GET_FLOAT(L, 3));
		button->getAttributes().setFloat(GUIText::ATTR_HEIGHT, SM_GET_FLOAT(L, 4));
	}
	button->create();
	SM_GET_RM()->add(name, button);
	Node* node = NEW Node(name, button);
	ScriptManager::getServiceLocator()->getRootNode()->addChild(node);
	node->setParent(ScriptManager::getServiceLocator()->getRootNode());
	SM_GET_SL()->getGUIManager()->refreshNodes(SM_GET_SL()->getRootNode());
	SM_RETURN_OBJECT(L, "Node", Node, node);
    return 1;
}

void registerGUIButton() {
    unordered_map<string, int (*)(lua_State*)> methods;
	ADD_METHOD(methods, "new", newGUIButton);
    ScriptManager::addClass("Button", methods);
}

int getCamera(lua_State* L) {
    Camera* camera = ScriptManager::getServiceLocator()->getCamera();
	SM_RETURN_OBJECT(L, "Camera", Camera, camera);
    return 1;
}

int cameraFollow(lua_State* L) {
    Camera* camera = SM_GET_OBJECT(L, 0, Camera);
    Node* node = SM_GET_OBJECT(L, 1, Node);
	camera->follow(node);
	return 0;
}

int cameraLookAt(lua_State* L) {
    Camera* camera = SM_GET_OBJECT(L, 0, Camera);
    Node* node = SM_GET_OBJECT(L, 1, Node);
	camera->lookAt(node);
	return 0;
}

int cameraMoveX(lua_State* L) {
    Camera* camera = SM_GET_OBJECT(L, 0, Camera);
    float distance = SM_GET_FLOAT(L, 1);
	Vec3 move(distance, 0.0f, 0.0f);
	Mat4 inv;
	Vec3 pos = camera->getPos();
	Vec3 rot = camera->getRot();
	Matrix::transformInv(inv, pos, rot);
	Matrix::multiply(move, inv);
	camera->getPos().addXYZ(move);
	return 0;
}

int cameraMoveY(lua_State* L) {
    Camera* camera = SM_GET_OBJECT(L, 0, Camera);
    float distance = SM_GET_FLOAT(L, 1);
	Vec3 move(0.0f, distance, 0.0f);
	Mat4 inv;
	Vec3 pos = camera->getPos();
	Vec3 rot = camera->getRot();
	Matrix::transformInv(inv, pos, rot);
	Matrix::multiply(move, inv);
	camera->getPos().addXYZ(move);
	return 0;
}

int cameraMoveZ(lua_State* L) {
    Camera* camera = SM_GET_OBJECT(L, 0, Camera);
    float distance = SM_GET_FLOAT(L, 1);
	Vec3 move(0.0f, 0.0f, distance);
	Mat4 inv;
	Vec3 pos = camera->getPos();
	Vec3 rot = camera->getRot();
	Matrix::transformInv(inv, pos, rot);
	Matrix::multiply(move, inv);
	camera->getPos().addXYZ(move);
	return 0;
}

#define CAMERA_ADD_SET(method_name, method_call) \
	METHOD_ADD_SET(Camera, method_name, method_call)
#define CAMERA_GET(method_name, method_call) \
	METHOD_GET(Camera, method_name, method_call)
#define CAMERA_ADD_SET_TWO(method_name, method1, method2) \
	METHOD_ADD_SET_TWO(Camera, method_name, method1, method2)
#define CAMERA_GET_TWO(method_name, method1, method2) \
	METHOD_GET_TWO(Camera, method_name, method1, method2)
#define CAMERA_ADD_SET_THREE(method_name, method) \
	METHOD_ADD_SET_THREE(Camera, method_name, method)
#define CAMERA_GET_THREE(method_name, method) \
	METHOD_GET_THREE(Camera, method_name, method)
#define CAMERA_GET_THREE_ROT(method_name, method1, method2, method3) \
	METHOD_GET_THREE_ROT(Camera, method_name, method1, method2, method3)

CAMERA_ADD_SET(cameraAddPosX, getPos().addX)
CAMERA_ADD_SET(cameraAddPosY, getPos().addY)
CAMERA_ADD_SET(cameraAddPosZ, getPos().addZ)
CAMERA_ADD_SET(cameraAddRotX, getRot().addX)
CAMERA_ADD_SET(cameraAddRotY, getRot().addY)
CAMERA_ADD_SET(cameraAddRotZ, getRot().addZ)
CAMERA_ADD_SET(cameraSetPosX, getPos().setX)
CAMERA_ADD_SET(cameraSetPosY, getPos().setY)
CAMERA_ADD_SET(cameraSetPosZ, getPos().setZ)
CAMERA_ADD_SET(cameraSetRotX, getRot().setX)
CAMERA_ADD_SET(cameraSetRotY, getRot().setY)
CAMERA_ADD_SET(cameraSetRotZ, getRot().setZ)
CAMERA_GET(cameraGetPosX, getPos().getX())
CAMERA_GET(cameraGetPosY, getPos().getY())
CAMERA_GET(cameraGetPosZ, getPos().getZ())
CAMERA_GET(cameraGetRotX, getRot().getX())
CAMERA_GET(cameraGetRotY, getRot().getY())
CAMERA_GET(cameraGetRotZ, getRot().getZ())
CAMERA_ADD_SET_TWO(cameraAddPosXY, getPos().addX, getPos().addY)
CAMERA_ADD_SET_TWO(cameraAddPosXZ, getPos().addX, getPos().addZ)
CAMERA_ADD_SET_TWO(cameraAddPosYZ, getPos().addY, getPos().addZ)
CAMERA_ADD_SET_TWO(cameraAddRotXY, getRot().addX, getRot().addY)
CAMERA_ADD_SET_TWO(cameraAddRotXZ, getRot().addX, getRot().addZ)
CAMERA_ADD_SET_TWO(cameraAddRotYZ, getRot().addY, getRot().addZ)
CAMERA_ADD_SET_TWO(cameraSetPosXY, getPos().setX, getPos().setY)
CAMERA_ADD_SET_TWO(cameraSetPosXZ, getPos().setX, getPos().setZ)
CAMERA_ADD_SET_TWO(cameraSetPosYZ, getPos().setY, getPos().setZ)
CAMERA_ADD_SET_TWO(cameraSetRotXY, getRot().setX, getRot().setY)
CAMERA_ADD_SET_TWO(cameraSetRotXZ, getRot().setX, getRot().setZ)
CAMERA_ADD_SET_TWO(cameraSetRotYZ, getRot().setY, getRot().setZ)
CAMERA_GET_TWO(cameraGetPosXY, getPos().getX(), getPos().getY())
CAMERA_GET_TWO(cameraGetPosXZ, getPos().getX(), getPos().getZ())
CAMERA_GET_TWO(cameraGetPosYZ, getPos().getY(), getPos().getZ())
CAMERA_GET_TWO(cameraGetRotXY, getRot().getX(), getRot().getY())
CAMERA_GET_TWO(cameraGetRotXZ, getRot().getX(), getRot().getZ())
CAMERA_GET_TWO(cameraGetRotYZ, getRot().getY(), getRot().getZ())
CAMERA_ADD_SET_THREE(cameraAddPosXYZ, getPos().setXYZ)
CAMERA_ADD_SET_THREE(cameraAddRotXYZ, getRot().setXYZ)
CAMERA_ADD_SET_THREE(cameraSetPosXYZ, getPos().setXYZ)
CAMERA_ADD_SET_THREE(cameraSetRotXYZ, getRot().setXYZ)
CAMERA_GET_THREE(cameraGetPosXYZ, getPos().getPointer())
CAMERA_GET_THREE_ROT(cameraGetRotXYZ, getRot().getX(), getRot().getY(), getRot().getZ())

void registerCamera() {
    unordered_map<string, int (*)(lua_State*)> methods;
    ADD_METHOD(methods, "follow", cameraFollow);
    ADD_METHOD(methods, "lookAt", cameraLookAt);
    ADD_METHOD(methods, "moveX", cameraMoveX);
    ADD_METHOD(methods, "moveY", cameraMoveY);
    ADD_METHOD(methods, "moveZ", cameraMoveZ);
    ADD_METHOD(methods, "addPosX", cameraAddPosX);
    ADD_METHOD(methods, "setPosX", cameraSetPosX);
    ADD_METHOD(methods, "getPosX", cameraGetPosX);
    ADD_METHOD(methods, "addPosY", cameraAddPosY);
    ADD_METHOD(methods, "setPosY", cameraSetPosY);
    ADD_METHOD(methods, "getPosY", cameraGetPosY);
    ADD_METHOD(methods, "addPosZ", cameraAddPosZ);
    ADD_METHOD(methods, "setPosZ", cameraSetPosZ);
    ADD_METHOD(methods, "getPosZ", cameraGetPosZ);
    ADD_METHOD(methods, "addRotX", cameraAddRotX);
    ADD_METHOD(methods, "setRotX", cameraSetRotX);
    ADD_METHOD(methods, "getRotX", cameraGetRotX);
    ADD_METHOD(methods, "addRotY", cameraAddRotY);
    ADD_METHOD(methods, "setRotY", cameraSetRotY);
    ADD_METHOD(methods, "getRotY", cameraGetRotY);
    ADD_METHOD(methods, "addRotZ", cameraAddRotZ);
    ADD_METHOD(methods, "setRotZ", cameraSetRotZ);
    ADD_METHOD(methods, "getRotZ", cameraGetRotZ);
	ADD_METHOD(methods, "addPosXY", cameraAddPosXY);
	ADD_METHOD(methods, "addPosXZ", cameraAddPosXZ);
	ADD_METHOD(methods, "addPosYZ", cameraAddPosYZ);
	ADD_METHOD(methods, "addRotXY", cameraAddRotXY);
	ADD_METHOD(methods, "addRotXZ", cameraAddRotXZ);
	ADD_METHOD(methods, "addRotYZ", cameraAddRotYZ);
	ADD_METHOD(methods, "setPosXY", cameraSetPosXY);
	ADD_METHOD(methods, "setPosXZ", cameraSetPosXZ);
	ADD_METHOD(methods, "setPosYZ", cameraSetPosYZ);
	ADD_METHOD(methods, "setRotXY", cameraSetRotXY);
	ADD_METHOD(methods, "setRotXZ", cameraSetRotXZ);
	ADD_METHOD(methods, "setRotYZ", cameraSetRotYZ);
	ADD_METHOD(methods, "getPosXY", cameraGetPosXY);
	ADD_METHOD(methods, "getPosXZ", cameraGetPosXZ);
	ADD_METHOD(methods, "getPosYZ", cameraGetPosYZ);
	ADD_METHOD(methods, "getRotXY", cameraGetRotXY);
	ADD_METHOD(methods, "getRotXZ", cameraGetRotXZ);
	ADD_METHOD(methods, "getRotYZ", cameraGetRotYZ);
	ADD_METHOD(methods, "addPosXYZ", cameraAddPosXYZ);
	ADD_METHOD(methods, "addRotXYZ", cameraAddRotXYZ);
	ADD_METHOD(methods, "setPosXYZ", cameraSetPosXYZ);
	ADD_METHOD(methods, "setRotXYZ", cameraSetRotXYZ);
	ADD_METHOD(methods, "getPosXYZ", cameraGetPosXYZ);
	ADD_METHOD(methods, "getRotXYZ", cameraGetRotXYZ);
    ScriptManager::addFunction("getCamera", getCamera);
    ScriptManager::addClass("Camera", methods);
}

int newShader(lua_State* L) {
	string val = SM_GET_STRING(L, 0);
	Shader* shader = static_cast<Shader*>(SM_GET_RM()->get(Resource::SHADER, val));
	if (shader == 0) {
		shader = NEW Shader(SM_GET_SL());
		shader->getAttributes().setString(Resource::ATTR_FILE, val);
		shader->create();
		SM_GET_RM()->add(val, shader);
	}
	SM_RETURN_OBJECT(L, "Shader", Shader, shader);
    return 1;
}

int deleteShader(lua_State* L) {
    return 0;
}

void registerShader() {
    unordered_map<string, int (*)(lua_State*)> methods;
	ADD_METHOD(methods, "new", newShader);
	ADD_METHOD(methods, "__gc", deleteShader);
    ScriptManager::addClass("Shader", methods);
}


int getNetworkManager(lua_State* L) {
	NetworkManager* networkManager = SM_GET_SL()->getNetworkManager();
	SM_RETURN_OBJECT(L, "NetworkManager", NetworkManager, networkManager);
    return 1;
}

class LuaHttpTask : public NetworkManager::HttpTask {
public:
	void run(HttpResponse* response) {
		SM_GET_SL()->getScriptManager()->provideResponse(response);
	}
};

int networkManagerExecute(lua_State* L) {
	HttpRequest* request = SM_GET_OBJECT(L, 1, HttpRequest);
	SM_GET_SL()->getNetworkManager()->execute(request, new LuaHttpTask());
	return 0;
}

void registerNetworkManager() {
    unordered_map<string, int (*)(lua_State*)> methods;
	ADD_METHOD(methods, "execute", networkManagerExecute);
	ScriptManager::addFunction("getNetworkManager", getNetworkManager);
    ScriptManager::addClass("NetworkManager", methods);
}

int newHttpRequest(lua_State* L) {
	int argc = SM_GET_ARGUMENT_COUNT(L);
	string uri = SM_GET_STRING(L, 0);
	HttpRequest::Method requestMethod = HttpRequest::GET;
	if (argc > 1) {
		string method = SM_GET_STRING(L, 1);
		if (method == "POST") {
			requestMethod = HttpRequest::POST;
		}
	}
	HttpRequest::MimeType requestMimeType = HttpRequest::RAW;
	if (argc > 2) {
		string mimeType = SM_GET_STRING(L, 2);
		if (mimeType == "FORM") {
			requestMimeType = HttpRequest::FORM;
		}
	}
	HttpRequest* request = NEW HttpRequest(uri, requestMethod, requestMimeType);
	SM_RETURN_OBJECT(L, "Request", HttpRequest, request);
    return 1;
}

int deleteHttpRequest(lua_State* L) {
    return 0;
}

int httpRequestSetEntity(lua_State* L) {
	HttpRequest* request = SM_GET_OBJECT(L, 0, HttpRequest);
	if (SM_IS_STRING(L, 1)) {
		string entity = SM_GET_STRING(L, 1);
		INT8* bytes = NEW INT8[entity.size()];
		memcpy(bytes, entity.c_str(), entity.size());
		request->setEntity(bytes, entity.size());
	}
	return 0;
}

int httpRequestSetId(lua_State* L) {
	HttpRequest* request = SM_GET_OBJECT(L, 0, HttpRequest);
	int id = SM_GET_INT(L, 1);
	request->setId(id);
	return 0;
}

void registerHttpRequest() {
    unordered_map<string, int (*)(lua_State*)> methods;
	ADD_METHOD(methods, "new", newHttpRequest);
	ADD_METHOD(methods, "__gc", deleteHttpRequest);
	ADD_METHOD(methods, "setEntity", httpRequestSetEntity);
	ADD_METHOD(methods, "setId", httpRequestSetId);
    ScriptManager::addClass("Request", methods);
}

int deleteHttpResponse(lua_State* L) {
 //   LOGD("Deleting http response.");
	//HttpResponse* response = *(HttpResponse**) luaL_checkudata(L, 1, "Response");
 //   delete response;
	return 0;
}

int httpResponseGetHeader(lua_State* L) {
	HttpResponse* response = SM_GET_OBJECT(L, 0, HttpResponse);
	SM_RETURN_STRING(L, response->getHeader().c_str());
	return 1;
}

int httpResponseGetId(lua_State* L) {
	HttpResponse* response = SM_GET_OBJECT(L, 0, HttpResponse);
	SM_RETURN_INT(L, response != 0 ? response->getId() : -1);
	return 1;
}

int httpResponseGetContent(lua_State* L) {
	HttpResponse* response = SM_GET_OBJECT(L, 0, HttpResponse);
	if (response != 0 && response->getContentSize() > 0) {
		SM_RETURN_STRING(L, response->getContent());
	} else {
		SM_RETURN_NIL(L);
	}
	return 1;
}

void registerHttpResponse() {
    unordered_map<string, int (*)(lua_State*)> methods;
	ADD_METHOD(methods, "__gc", deleteHttpResponse);
	ADD_METHOD(methods, "getHeader", httpResponseGetHeader);
	ADD_METHOD(methods, "getId", httpResponseGetId);
	ADD_METHOD(methods, "getContent", httpResponseGetContent);
    ScriptManager::addClass("Response", methods);
}

/**int newVehicle(lua_State* L) {
	LOGW("Unimplemented vehicle creation in script side.");
	return 0;
}

int deleteVehicle(lua_State* L) {
	return 0;
}

int vehicleAccelerate(lua_State* L) {
	Vehicle* vehicle = SM_GET_OBJECT(L, 0, Vehicle);
	float acceleration = SM_GET_FLOAT(L, 1);
	btRaycastVehicle* raycastVehicle = vehicle->getRaycastVehicle();
	raycastVehicle->applyEngineForce(acceleration, 2);
	raycastVehicle->applyEngineForce(acceleration, 3);
	raycastVehicle->setBrake(0.0f, 0);
	raycastVehicle->setBrake(0.0f, 1);
	return 0;
}

int vehicleTurn(lua_State* L) {
	Vehicle* vehicle = SM_GET_OBJECT(L, 0, Vehicle);
	float rotation = SM_GET_FLOAT(L, 1);;
	btRaycastVehicle* raycastVehicle = vehicle->getRaycastVehicle();
	float maxRot = DEG_TO_RAD(vehicle->getWheelTurn());
	float rot = raycastVehicle->getSteeringValue(0) + DEG_TO_RAD(rotation);
	if (rot > maxRot) {
		rot = maxRot;
	} else if (rot < -maxRot) {
		rot = -maxRot;
	}
	raycastVehicle->setSteeringValue(rot, 0);
	raycastVehicle->setSteeringValue(rot, 1);
	return 0;
}

void registerVehicle() {
    unordered_map<string, int (*)(lua_State*)> methods;
	ADD_METHOD(methods, "new", newVehicle);
	ADD_METHOD(methods, "__gc", deleteVehicle);
	ADD_METHOD(methods, "accelerate", vehicleAccelerate);
	ADD_METHOD(methods, "turn", vehicleTurn);
    ScriptManager::addClass("Vehicle", methods);
}*/

int newScenario(lua_State* L) {
	Scenario* scenario = new Scenario(SM_GET_SL());
	scenario->create();
	SM_RETURN_OBJECT(L, "Scenario", Scenario, scenario);
	return 1;
}

int deleteScenario(lua_State* L) {
	delete SM_GET_OBJECT(L, 0, Scenario);
	return 0;
}

int scenarioMove(lua_State* L) {
	Scenario* scenario = SM_GET_OBJECT(L, 0, Scenario);
	Node* node = SM_GET_OBJECT(L, 1, Node);
	int argc = SM_GET_ARGUMENT_COUNT(L);
	Vec3 start = node->getPos();
	Vec3 end = start;
	float duration = 0.0f;
	if (argc == 5) {
		end.setXYZ(SM_GET_FLOAT(L, 2), SM_GET_FLOAT(L, 3), start.getZ());
		duration = SM_GET_FLOAT(L, 4);
	}
	else if (argc == 6) {
		end.setXYZ(SM_GET_FLOAT(L, 2), SM_GET_FLOAT(L, 3), SM_GET_FLOAT(L, 4));
		duration = SM_GET_FLOAT(L, 5);
	}
	else if (argc == 7) {
		start.setXYZ(SM_GET_FLOAT(L, 2), SM_GET_FLOAT(L, 3), start.getZ());
		end = start;
		end.setXYZ(SM_GET_FLOAT(L, 4), SM_GET_FLOAT(L, 5), start.getZ());
		duration = SM_GET_FLOAT(L, 6);
	}
	else if (argc == 9) {
		start.setXYZ(SM_GET_FLOAT(L, 2), SM_GET_FLOAT(L, 3), SM_GET_FLOAT(L, 4));
		end = start;
		end.setXYZ(SM_GET_FLOAT(L, 5), SM_GET_FLOAT(L, 6), SM_GET_FLOAT(L, 7));
		duration = SM_GET_FLOAT(L, 8);
	}
	scenario->move(node, start, end, duration);
	return 0;
}

int scenarioWait(lua_State* L) {
	Scenario* scenario = SM_GET_OBJECT(L, 0, Scenario);
	scenario->wait(SM_GET_FLOAT(L, 1));
	return 0;
}

void registerScenario() {
    unordered_map<string, int (*)(lua_State*)> methods;
	ADD_METHOD(methods, "new", newScenario);
	ADD_METHOD(methods, "__gc", deleteScenario);
	ADD_METHOD(methods, "move", scenarioMove);
	ADD_METHOD(methods, "wait", scenarioWait);
    ScriptManager::addClass("Scenario", methods);
}

int getScenarioManager(lua_State* L) {
	SM_RETURN_OBJECT(L, "ScenarioManager", ScenarioManager, SM_GET_SL()->getScenarioManager());
    return 1;
}

int scenarioManagerExecute(lua_State* L) {
	ScenarioManager* sm = SM_GET_OBJECT(L, 0, ScenarioManager);
	Scenario* scenario = SM_GET_OBJECT(L, 1, Scenario);
	sm->execute(new Scenario(*scenario));
	return 0;
}

void registerScenarioManager() {
    unordered_map<string, int (*)(lua_State*)> methods;
	ADD_METHOD(methods, "execute", scenarioManagerExecute);
	ScriptManager::addFunction("getScenarioManager", getScenarioManager);
    ScriptManager::addClass("ScenarioManager", methods);
}

int getSceneManager(lua_State* L) {
	SM_RETURN_OBJECT(L, "SceneManager", SceneManager, SM_GET_SL()->getSceneManager());
    return 1;
}

int sceneManagerSetSceneHeight(lua_State* L) {
	ASSERT(lua_gettop(L) == 2, "Wrong argument count for method setSceneHeight.");
	SceneManager* sm = SM_GET_OBJECT(L, 0, SceneManager);
	int size = SM_GET_INT(L, 1);
	sm->setSceneHeight(size);
	return 0;
}

int sceneManagerSetTarget(lua_State* L) {
	SceneManager* sm = SM_GET_OBJECT(L, 0, SceneManager);
	Node* sprite = SM_GET_OBJECT(L, 1, Node);
	sm->setTarget(sprite);
	return 0;
}

int sceneManagerSetBackground(lua_State* L) {
	SceneManager* sm = SM_GET_OBJECT(L, 0, SceneManager);
	Node* sprite = SM_IS_NULL(L, 1) ? 0 : SM_GET_OBJECT(L, 1, Node);
	int argc = SM_GET_ARGUMENT_COUNT(L);
	sm->setBackground(sprite, argc > 2 ? SM_GET_INT(L, 2) - 1 : 0, argc > 3 ? SM_GET_FLOAT(L, 3) : 1.0f);
	return 0;
}

int sceneManagerGetBackgroundWidth(lua_State* L) {
	SceneManager* sm = SM_GET_OBJECT(L, 0, SceneManager);
	SM_RETURN_FLOAT(L, sm->getBackgroundWidth());
	return 1;
}

int sceneManagerGetBackgroundHeight(lua_State* L) {
	SceneManager* sm = SM_GET_OBJECT(L, 0, SceneManager);
	SM_RETURN_FLOAT(L, sm->getBackgroundHeight());
	return 1;
}

void registerSceneManager() {
    unordered_map<string, int (*)(lua_State*)> methods;
	ADD_METHOD(methods, "setSceneHeight", sceneManagerSetSceneHeight);
	ADD_METHOD(methods, "setTarget", sceneManagerSetTarget);
	ADD_METHOD(methods, "setBackground", sceneManagerSetBackground);
	ADD_METHOD(methods, "getBackgroundWidth", sceneManagerGetBackgroundWidth);
	ADD_METHOD(methods, "getBackgroundHeight", sceneManagerGetBackgroundHeight);
	ScriptManager::addFunction("getSceneManager", getSceneManager);
    ScriptManager::addClass("SceneManager", methods);
}

int getDB(lua_State* L) {
	SM_RETURN_OBJECT(L, "Database", Database, SM_GET_SL()->getDB());
    return 1;
}

int databaseGetString(lua_State* L) {
	Database* db = SM_GET_OBJECT(L, 0, Database);
	ASSERT(SM_GET_ARGUMENT_COUNT(L) != 1,
		"Function getString() takes setting name as parameter.");
	string name = SM_GET_STRING(L, 1);
	SM_RETURN_STRING(L, db->getString(name).c_str());
	return 1;
}

int databaseSetString(lua_State* L) {
	Database* db = SM_GET_OBJECT(L, 0, Database);
	ASSERT(SM_GET_ARGUMENT_COUNT(L) != 2,
		"Function setString() takes setting name and value as parameters.");
	string name = SM_GET_STRING(L, 1);
	string value = SM_GET_STRING(L, 2);
	db->setString(name, value);
	return 0;
}

int databaseExecute(lua_State* L) {
	Database* db = SM_GET_OBJECT(L, 0, Database);
	ASSERT(SM_GET_ARGUMENT_COUNT(L) != 1,
		"Function execute() takes SQL query as parameter.");
	string query = SM_GET_STRING(L, 1);
	Database::ResultSet rs = db->execute(query);
	int size = rs.vec.size();
	lua_newtable(L);
	for (int i = 0; i < size; i++) {
		unordered_map<string, string>& map = rs.vec[i];
		unordered_map<string, string>::const_iterator it = map.begin();
		lua_pushnumber(L, i + 1);
		lua_newtable(L);
		while (it != map.end()) {
			lua_pushstring(L, it->first.c_str());
			lua_pushstring(L, it->second.c_str());
			lua_settable(L, -3);
			it++;
		}
		lua_settable(L, -3);
	}
	return 1;
}

void registerDatabase() {
    unordered_map<string, int (*)(lua_State*)> methods;
	ADD_METHOD(methods, "getString", databaseGetString);
	ADD_METHOD(methods, "setString", databaseSetString);
	ADD_METHOD(methods, "execute", databaseExecute);
	ScriptManager::addFunction("getDB", getDB);
    ScriptManager::addClass("Database", methods);
}