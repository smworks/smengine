/*
 * ScriptBindings.cpp
 *
 *  Created on: 2013.11.03
 *      Author: Martynas Šustavičius
 */

#include "ScriptBindings.h"
#include "ScriptManager.h"
#include "Multiplatform/ServiceLocator.h"
#include "SoundManager.h"
#include "Settings.h"
#include "Input.h"
#include "Node.h"
#include "Camera.h"
#include "PhysicsManager.h"
#include "ResourceManager.h"
#include "NetworkManager.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Resources/StaticObject.h"
#include "Resources/Shader.h"
#include "Resources/Sprite.h"
#include "Resources/Sound.h"
#include "Resources/Texture.h"
#include "Resources/TextureMono.h"
#include "Resources/TextureRGBA.h"
#include "Resources/AtlasTexture.h"
#include "Resources/GUIButton.h"
#include "Extensions/Vehicle.h"

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
#define SM_RETURN_STRING(state, val) lua_pushstring(state, val);
#define SM_IS_OBJECT(state, index) lua_isuserdata(state, SM_INDEX(index))
#define SM_GET_OBJECT(state, index, type) \
	*(static_cast<type**>(lua_touserdata(state, SM_INDEX(index))))
#define SM_RETURN_OBJECT(state, name, type, object) \
	type** udata = (type**) lua_newuserdata(state, sizeof(type*)); \
	*udata = object; \
	luaL_getmetatable(L, name); \
	lua_setmetatable(L, -2)
#define SM_POP_ARGS(state, argc) lua_pop(state, argc)
#define SM_GET_SL() ScriptManager::getServiceLocator()
#define SM_GET_RM() SM_GET_SL()->getRM()
#define SM_GET_PM() SM_GET_SL()->getPM()

#define ADD_METHOD(map, method_name, method) \
    map.insert(pair<string, int (*)(lua_State*)>(method_name, method))

#define METHOD_ADD_SET_STATE(class_name, method_name, method_call, state) \
	int method_name(lua_State* L) { \
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
	if (node->hasResource(Resource::SPRITE)) {
		if (x > node->getPos().getX()
			&& x < node->getPos().getX() + node->getScale().getX()
			&& y > node->getPos().getY()
			&& y < node->getPos().getY() + node->getScale().getY())
		{
			lua_pushboolean(L, true);
			return 1;
		}
	}
	else if (node->hasResource(Resource::GUI_TEXT)) {
		GUIText* text = static_cast<GUIText*>(
			node->getResource(Resource::GUI_TEXT));
		float length = (float) text->getText().length();
		if (x > node->getPos().getX()
			&& x < node->getPos().getX() + node->getScale().getX() * length
			&& y > node->getPos().getY()
			&& y < node->getPos().getY() + node->getScale().getY())
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
	SM_GET_SL()->getSM()->play(sound);
	return 0;
}

int loadScene(lua_State* L) {
	string val = SM_GET_STRING(L, 0);
	SM_GET_SL()->getSettings()->setScene(val);
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
	registerVehicle();
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

int newNode(lua_State* L) {
    string val = SM_GET_STRING(L, 0);
	Node* node = NEW Node();
	node->setName(val);
	SM_GET_SL()->getRootNode()->addChild(node);
	node->setParent(SM_GET_SL()->getRootNode());
	SM_RETURN_OBJECT(L, "Node", Node, node);
    return 1;
}

int deleteNode(lua_State* L) {
    //LOGI("Deleting node.");
	//Node* node = *(Node**) luaL_checkudata(L, 1, "node");
 //   delete node;
    return 0;
}

int nodeDisablePhysics(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	SM_GET_PM()->remove(node);
	return 0;
}

int nodeSetMass(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	Resource* model = node->getResource(Resource::STATIC_OBJECT);
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
		LOGE("Specified shader is null.");
		return 0;
	}
	Shader* shader = SM_GET_OBJECT(L, 1, Shader);
	StaticObject* obj = static_cast<StaticObject*>(
		node->getResource(Resource::STATIC_OBJECT));
	obj->setShader(shader);
	SM_POP_ARGS(L, SM_GET_ARGUMENT_COUNT(L));
	return 0;
}

int nodeGetShader(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	StaticObject* obj = static_cast<StaticObject*>(
		node->getResource(Resource::STATIC_OBJECT));
	SM_POP_ARGS(L, SM_GET_ARGUMENT_COUNT(L));
	SM_RETURN_OBJECT(L, "Shader", Shader, obj->getShader());
	return 0;
}

int nodeAddTexture(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	Texture* texture = SM_GET_OBJECT(L, 1, Texture);
	Sprite* obj = static_cast<Sprite*>(
		node->getResource(Resource::SPRITE));
	if (obj != 0) {
		obj->addTexture(texture);
	}
	SM_POP_ARGS(L, SM_GET_ARGUMENT_COUNT(L));
	return 0;
}

int nodeSetSprite(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	INT32 index = SM_GET_INT(L, 1);
	Sprite* obj = static_cast<Sprite*>(node->getResource(Resource::SPRITE));
	if (obj != 0) {
		obj->setSpriteIndex(index);
	}
	return 0;
}

int nodeGetSpriteCount(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	Sprite* obj = static_cast<Sprite*>(node->getResource(Resource::SPRITE));
    SIZE indexCount = 0;
	if (obj != 0) {
		indexCount = obj->getIndexCount();
	}
	SM_POP_ARGS(L, SM_GET_ARGUMENT_COUNT(L));
	return 1;
}

int setText(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	GUIText* text = SM_GET_OBJECT(L, 1, GUIText);
	node->addResource(text);
	return 0;
}

int getText(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	GUIText* text = static_cast<GUIText*>(
		node->getResource(Resource::GUI_TEXT));
	SM_RETURN_OBJECT(L, "Text", GUIText, text);
	return 1;
}

int setButton(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	GUIButton* button = SM_GET_OBJECT(L, 1, GUIButton);
	node->addResource(button);
	return 0;
}

int getButton(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	GUIButton* button = dynamic_cast<GUIButton*>(
		node->getResource(Resource::GUI_BUTTON));
	SM_RETURN_OBJECT(L, "Button", GUIButton, button);
	return 1;
}

int nodeSetSurface(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	GUISurface* surface = SM_GET_OBJECT(L, 1, GUISurface);
	node->addResource(surface);
	return 0;
}

int nodeGetSurface(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	GUISurface* surface = dynamic_cast<GUISurface*>(node->getResource(Resource::GUI_CONTAINER));
	SM_RETURN_OBJECT(L, "Surface", GUISurface, surface);
	return 1;
}

int setName(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	string name = SM_GET_STRING(L, 1);
	node->setName(name);
	return 0;
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

int getVehicle(lua_State* L) {
	Node* node = SM_GET_OBJECT(L, 0, Node);
	Vehicle* vehicle = SM_GET_PM()->getVehicle(node);
	SM_RETURN_OBJECT(L, "Vehicle", Vehicle, vehicle);
	return 1;
}

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
    ADD_METHOD(methods, "new", newNode);
    ADD_METHOD(methods, "__gc", deleteNode);
	ADD_METHOD(methods, "disablePhysics", nodeDisablePhysics);
	ADD_METHOD(methods, "setMass", nodeSetMass);
	ADD_METHOD(methods, "accelerate", nodeAccelerate);
	ADD_METHOD(methods, "turn", nodeTurn);
    ADD_METHOD(methods, "setShader", nodeSetShader);
    ADD_METHOD(methods, "getShader", nodeGetShader);
	ADD_METHOD(methods, "addTexture", nodeAddTexture);
    ADD_METHOD(methods, "setIndex", nodeSetSprite);
    ADD_METHOD(methods, "getCount", nodeGetSpriteCount);
    ADD_METHOD(methods, "setText", setText);
    ADD_METHOD(methods, "getText", getText);
    ADD_METHOD(methods, "setButton", setButton);
    ADD_METHOD(methods, "getButton", getButton);
    ADD_METHOD(methods, "setSurface", nodeSetSurface);
    ADD_METHOD(methods, "getSurface", nodeGetSurface);
    ADD_METHOD(methods, "setName", setName);
    ADD_METHOD(methods, "getName", getName);
    ADD_METHOD(methods, "setVisibility", setVisibility);
    ADD_METHOD(methods, "getVisibility", getVisibility);
	ADD_METHOD(methods, "getVehicle", getVehicle);
    ADD_METHOD(methods, "moveX", setName);
    ADD_METHOD(methods, "moveY", setName);
    ADD_METHOD(methods, "moveZ", setName);
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
	string name = SM_GET_STRING(L, 0);
	StaticObject* model = static_cast<StaticObject*>(SM_GET_RM()->get(
		Resource::STATIC_OBJECT, name));
	if (model == 0) {
		model = NEW StaticObject(ScriptManager::getServiceLocator());
		model->getAttributes().setString(Resource::ATTR_FILE, name);
		if (argc > 1) {
			string type = SM_GET_STRING(L, 1);
			model->getAttributes().setString(
				StaticObject::ATTR_TYPE, type);
			if (type == StaticObject::VAL_SHAPE) {
				model->getAttributes().setFloat(
					StaticObject::ATTR_HEIGHT, SM_GET_FLOAT(L, 2));
			}
			else if (type == StaticObject::VAL_SPHERE) {
				model->getAttributes().setInt(
					StaticObject::ATTR_VERTICAL_LOD, SM_GET_INT(L, 2));
				model->getAttributes().setInt(
					StaticObject::ATTR_HORIZONTAL_LOD, SM_GET_INT(L, 3));
			}
		}
		SM_GET_RM()->add(name, model);
		model->create();
	}
	Node* node = NEW Node();
	node->setName(name + "_ModelResource");
	SM_GET_SL()->getRootNode()->addChild(node);
	node->setParent(SM_GET_SL()->getRootNode());
	node->addResource(model);
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
	string val = SM_GET_STRING(L, 0);
	Sprite* sprite = static_cast<Sprite*>(SM_GET_RM()->get(
		Resource::SPRITE, val));
	if (sprite == 0) {
		sprite = NEW Sprite(ScriptManager::getServiceLocator());
		sprite->create();
		SM_GET_RM()->add(val, sprite);
	}
	if (argc > 1) {
		Texture* texture = SM_GET_OBJECT(L, 1, Texture);
		if (texture != 0) {
			sprite->addTexture(texture);
		}
	}
	Node* node = NEW Node();
	node->setName(val + "_SpriteResource");
	ScriptManager::getServiceLocator()->getRootNode()->addChild(node);
	node->setParent(ScriptManager::getServiceLocator()->getRootNode());
	node->addResource(sprite);
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
	string name = SM_GET_STRING(L, 0);
	Texture* texture = static_cast<Texture*>(
		SM_GET_RM()->get(Resource::TEXTURE_2D, name));
	if (texture == 0) {
		if (argc == 2) {
			if (SM_IS_STRING(L, 1)) {
				string val = SM_GET_STRING(L, 1);
				if (val == Texture::VAL_MONO) {
					texture = NEW TextureMono(SM_GET_SL());
					texture->getAttributes().setString(
						Resource::ATTR_FILE, name);
					texture->create();
				} else {
					texture = NEW TextureRGBA(SM_GET_SL());
					texture->getAttributes().setString(
						Resource::ATTR_FILE, name);
					texture->create();
				}
			}
			else {
				HttpResponse* response = SM_GET_OBJECT(L, 1, HttpResponse);
				UINT32 width, height;
				UINT8* raw = reinterpret_cast<UINT8*>(
					pngToRaw(response->getContent(), width, height));
				texture = NEW TextureRGBA(SM_GET_SL());
				texture->create(width, height);
				for (UINT32 row = 0; row < height; row++) {
					for (UINT32 col = 0; col < width; col++) {
						texture->setPixel(&raw[row * width * 4 + col * 4], row, col);
					}
				}
				delete[] raw;
			}
		}
		else {
			string val = SM_GET_STRING(L, 1);
			int width = SM_GET_INT(L, 2);
			int height = SM_GET_INT(L, 3);
			if (val == Texture::VAL_MONO) {
				texture = NEW TextureMono(SM_GET_SL());
				texture->create(width, height);
			} else {
				texture = NEW TextureRGBA(SM_GET_SL());
				texture->create(width, height);
			}
		}
		SM_GET_RM()->add(name, texture);
	}
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
	ADD_METHOD(methods, "commit", ::textureCommit);
    ADD_METHOD(methods, "__gc", ::deleteTexture);
    ScriptManager::addClass("Texture", methods);
}

//int newContainer(lua_State* L) {
//	string name = SM_GET_STRING(L, 0);
//	string width = SM_GET_STRING(L, 1);
//	string height = SM_GET_STRING(L, 2);
//	GUISurface* surface = static_cast<GUISurface*>(SM_GET_RM()->get(
//		Resource::GUI_CONTAINER, name));
//	if (surface == 0) {
//		surface = NEW GUISurface(SM_GET_SL());
//		surface->getAttributes().setString(
//			GUISurface::ATTR_WIDTH, width);
//		surface->getAttributes().setString(
//			GUISurface::ATTR_HEIGHT, height);
//		surface->create();
//		SM_GET_RM()->add(name, surface);
//	}
//	SM_RETURN_OBJECT(L, "Container", GUIContainer, container);
//    return 1;
//}

//int deleteContainer(lua_State* L) {
//    return 0;
//}

int surfaceSetWidth(lua_State* L) {
    GUISurface* surface = SM_GET_OBJECT(L, 0, GUISurface);
	int val = SM_GET_INT(L, 1);
	surface->setWidth(val);
    return 0;
}

int surfaceGetWidth(lua_State* L) {
	GUISurface* surface = SM_GET_OBJECT(L, 0, GUISurface);
	SM_RETURN_INT(L, (int) surface->getWidth());
	return 1;
}

//int surfaceSetHeight(lua_State* L) {
//    GUIContainer* container = SM_GET_OBJECT(L, 0, GUIContainer);
//	int val = SM_GET_INT(L, 1);
//	container->setHeight(val);
//    return 0;
//}
//
//int containerGetHeight(lua_State* L) {
//	GUIContainer* container = SM_GET_OBJECT(L, 0, GUIContainer);
//	SM_RETURN_INT(L, (int) container->getHeight());
//	return 1;
//}
//
//int containerSetMarginLeft(lua_State* L) {
//	GUIContainer* container = SM_GET_OBJECT(L, 0, GUIContainer);
//	int val = SM_GET_INT(L, 1);
//	container->setMargins(val, container->getMarginBottom(),
//		container->getMarginRight(), container->getMarginTop());
//    return 0;
//}
//
//int containerGetMarginLeft(lua_State* L) {
//	GUIContainer* container = SM_GET_OBJECT(L, 0, GUIContainer);
//	SM_RETURN_INT(L, container->getMarginLeft());
//	return 1;
//}
//
//int containerSetMarginBottom(lua_State* L) {
//    GUIContainer* container = SM_GET_OBJECT(L, 0, GUIContainer);
//	int val = SM_GET_INT(L, 1);
//	container->setMargins(container->getMarginLeft(), val,
//		container->getMarginRight(), container->getMarginTop());
//    return 0;
//}
//
//int containerGetMarginBottom(lua_State* L) {
//	GUIContainer* container = SM_GET_OBJECT(L, 0, GUIContainer);
//	SM_RETURN_INT(L, container->getMarginBottom());
//	return 1;
//}
//
//int containerSetTransparency(lua_State* L) {
//    GUIContainer* container = SM_GET_OBJECT(L, 0, GUIContainer);
//	float val = SM_GET_FLOAT(L, 1);
//	container->setTransparency(val);
//    return 0;
//}
//
//int containerGetTransparency(lua_State* L) {
//	GUIContainer* container = SM_GET_OBJECT(L, 0, GUIContainer);
//	SM_RETURN_FLOAT(L, container->getTransparency());
//	return 1;
//}

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
	string val = SM_GET_STRING(L, 0);
	GUIText* text = static_cast<GUIText*>(SM_GET_RM()->get(Resource::GUI_TEXT, val));
	if (text == 0) {
		text = NEW GUIText(SM_GET_SL());
		text->getAttributes().setString(GUIText::ATTR_TEXT, val);
        text->getAttributes().setString(GUIText::ATTR_SCREEN_LEFT, "true");
        text->getAttributes().setString(GUIText::ATTR_SCREEN_RIGHT, "true");
        text->getAttributes().setString(GUIText::ATTR_SCREEN_TOP, "true");
		text->create();
		SM_GET_RM()->add(val, text);
	}
	SM_RETURN_OBJECT(L, "GUIText", GUIText, text);
    return 1;
}

int deleteGUIText(lua_State* L) {
//    LOGI("DELETING TEXT!");
//    Text* text = 0;
//    ScriptManager::getObject("Text", text);
//    if (text == 0) {
//        return 0;
//    }
//    LOGI("Text: %s", text->getContent().c_str());
//    string name = text->getName();
//    LOGI("Name: %s", name.c_str());
//	ScriptManager::getServiceLocator()->getRM()->remove(Resource::TEXT, text->getName());
    return 0;
}

int guiTextSetSize(lua_State* L) {
    GUIText* text = SM_GET_OBJECT(L, 0, GUIText);
	INT32 size = SM_GET_INT(L, 1);
	text->setTextSize(size);
    return 0;
}

int guiTextSetText(lua_State* L) {
    GUIText* text = SM_GET_OBJECT(L, 0, GUIText);
	string val = SM_GET_STRING(L, 1);
	text->setText(val);
    return 0;
}

int guiTextSetColor(lua_State* L) {
    GUIText* text = SM_GET_OBJECT(L, 0, GUIText);
	string val = SM_GET_STRING(L, 1);
	text->getDiffuse().setRGBA(val);
	return 0;
}

int guiTextSetBackground(lua_State* L) {
    GUIText* text = SM_GET_OBJECT(L, 0, GUIText);
	string val = SM_GET_STRING(L, 1);
	text->setBackground(val);
	return 0;
}

void registerGUIText() {
    unordered_map<string, int (*)(lua_State*)> methods;
	ADD_METHOD(methods, "new", newGUIText);
	ADD_METHOD(methods, "__gc", deleteGUIText);
    ADD_METHOD(methods, "setSize", guiTextSetSize);
	ADD_METHOD(methods, "setText", guiTextSetText);
	ADD_METHOD(methods, "setColor", guiTextSetColor);
	ADD_METHOD(methods, "setBackground", guiTextSetBackground);
    ScriptManager::addClass("GUIText", methods);
}

int newButton(lua_State* L) {
	string val = SM_GET_STRING(L, 0);
	GUIButton* button = static_cast<GUIButton*>(SM_GET_RM()->get(Resource::GUI_BUTTON, val));
	if (button == 0) {
		button = NEW GUIButton(SM_GET_SL());
		button->getAttributes().setString(GUIButton::ATTR_TEXT, val);
		button->create();
		SM_GET_RM()->add(val, button);
	}
	SM_RETURN_OBJECT(L, "Button", GUIButton, button);
    return 1;
}

int deleteButton(lua_State* L) {
    return 0;
}

int buttonSetButtonText(lua_State* L) {
    GUIButton* button = SM_GET_OBJECT(L, 0, GUIButton);
	string val = SM_GET_STRING(L, 1);
	button->setText(val);
    return 0;
}

int buttonGetButtonText(lua_State* L) {
	GUIButton* button = SM_GET_OBJECT(L, 0, GUIButton);
	SM_RETURN_STRING(L, button->getText().c_str());
	return 1;
}

int buttonSetButtonWidth(lua_State* L) {
    GUIButton* button = SM_GET_OBJECT(L, 0, GUIButton);
	int val = SM_GET_INT(L, 1);
	button->setWidth(val);
    return 0;
}

int buttonGetButtonWidth(lua_State* L) {
	GUIButton* button = SM_GET_OBJECT(L, 0, GUIButton);
	SM_RETURN_INT(L, (int) button->getWidth());
	return 1;
}

int buttonSetButtonHeight(lua_State* L) {
    GUIButton* button = SM_GET_OBJECT(L, 0, GUIButton);
	int val = SM_GET_INT(L, 1);
	button->setHeight(val);
    return 0;
}

int buttonGetButtonHeight(lua_State* L) {
	GUIButton* button = SM_GET_OBJECT(L, 0, GUIButton);
	SM_RETURN_INT(L, (int) button->getHeight());
	return 1;
}

int buttonSetButtonTransparency(lua_State* L) {
    GUIButton* button = SM_GET_OBJECT(L, 0, GUIButton);
	float val = SM_GET_FLOAT(L, 1);
	button->setTransparency(val);
    return 0;
}

int buttonGetButtonTransparency(lua_State* L) {
	GUIButton* button = SM_GET_OBJECT(L, 0, GUIButton);
	SM_RETURN_FLOAT(L, button->getTransparency());
	return 1;
}

void registerGUIButton() {
    unordered_map<string, int (*)(lua_State*)> methods;
	ADD_METHOD(methods, "new", newButton);
	ADD_METHOD(methods, "__gc", deleteButton);
	ADD_METHOD(methods, "setText", buttonSetButtonText);
	ADD_METHOD(methods, "getText", buttonGetButtonText);
	ADD_METHOD(methods, "setWidth", buttonSetButtonWidth);
	ADD_METHOD(methods, "getwidth", buttonGetButtonText);
	ADD_METHOD(methods, "setHeight", buttonSetButtonHeight);
	ADD_METHOD(methods, "getHeight", buttonGetButtonHeight);
	ADD_METHOD(methods, "setTransparency", buttonSetButtonTransparency);
	ADD_METHOD(methods, "getTransparency", buttonGetButtonTransparency);
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
	NetworkManager* networkManager = SM_GET_SL()->getNM();
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
	SM_GET_SL()->getNM()->execute(request, new LuaHttpTask());
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

void registerHttpResponse() {
    unordered_map<string, int (*)(lua_State*)> methods;
	ADD_METHOD(methods, "__gc", deleteHttpResponse);
	ADD_METHOD(methods, "getHeader", httpResponseGetHeader);
	ADD_METHOD(methods, "getId", httpResponseGetId);
    ScriptManager::addClass("Response", methods);
}

int newVehicle(lua_State* state) {
	LOGW("Unimplemented vehicle creation in script side.");
	return 0;
}

int deleteVehicle(lua_State* state) {
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
}