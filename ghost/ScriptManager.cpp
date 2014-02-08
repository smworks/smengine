/*
 * ScriptManger.cpp
 *
 *  Created on: 2013.02.25
 *      Author: Martynas Šustavičius
 */

#include "ScriptManager.h"
#include "Multiplatform/ServiceLocator.h"
#include "Input.h"
#include "Node.h"
#include "Resources/Script.h"
#include "Settings.h"
#include "ScriptBindings.h"
#include "GUIManager.h"

const string ScriptManager::FUNCTION_EVENT_RESPONSE = "eventResponse";
const string ScriptManager::FUNCTION_EVENT_GUI = "eventGUI";
const string ScriptManager::FUNCTION_START = "start";
const string ScriptManager::FUNCTION_RESIZE = "resize";
const string ScriptManager::FUNCTION_UPDATE = "update";
const string ScriptManager::FUNCTION_PAUSE = "pause";
const string ScriptManager::FUNCTION_RESUME = "resume";
const string ScriptManager::FUNCTION_QUIT = "quit";
const string ScriptManager::CONSTANTS = "constants";

ServiceLocator* ScriptManager::serviceLocator_ = 0;
lua_State* ScriptManager::state_ = 0;
unordered_map<string, int (*)(lua_State*)> ScriptManager::functions_;
Script* ScriptManager::script_ = 0;

ScriptManager::ScriptManager() {
    serviceLocator_ = 0;
	state_ = luaL_newstate();
//	// Specifie required libraries.
//	static const luaL_Reg libraries[] = {
//		{"base", luaopen_base},
//		{"io", luaopen_io},
//		{"math", luaopen_math},
//		{0, 0}
//	};
//	const luaL_Reg* library = libraries;
//	// Load required libraries.
//	for (; library->func != 0; library++) {
//		luaL_requiref(
//			ScriptManager::state_, library->name, library->func, 1);
//		lua_settop(ScriptManager::state_, 0);
//	}
    luaL_openlibs(state_);
    LOGD("Created script manager.");
}

ScriptManager::~ScriptManager() {
	lua_close(ScriptManager::state_);
	state_ = 0;
	serviceLocator_ = 0;
	script_ = 0;
    LOGD("Deleted script manager.");
}

bool ScriptManager::isReady() {
	return script_ != 0;
}

void ScriptManager::initialize(ServiceLocator* locator) {
    ScriptManager::serviceLocator_ = locator;
	// Add key support.
	#define addKeyValue(value, key) lua_pushstring(state_, value); \
		lua_pushnumber(state_, key); lua_settable(state_, -3); count++
	int count = 0;
	lua_createtable(state_, Input::KEY_COUNT + GUIManager::ACTION_COUNT, 0);
	addKeyValue("NONE", Input::NONE);
	addKeyValue("0", Input::N0);
	addKeyValue("1", Input::N1);
	addKeyValue("2", Input::N2);
	addKeyValue("3", Input::N3);
	addKeyValue("4", Input::N4);
	addKeyValue("5", Input::N5);
	addKeyValue("6", Input::N6);
	addKeyValue("7", Input::N7);
	addKeyValue("8", Input::N8);
	addKeyValue("9", Input::N9);
	addKeyValue("A", Input::A);
	addKeyValue("B", Input::B);
	addKeyValue("C", Input::C);
	addKeyValue("D", Input::D);
	addKeyValue("E", Input::E);
	addKeyValue("F", Input::F);
	addKeyValue("G", Input::G);
	addKeyValue("H", Input::H);
	addKeyValue("I", Input::I);
	addKeyValue("J", Input::J);
	addKeyValue("K", Input::K);
	addKeyValue("L", Input::L);
	addKeyValue("M", Input::M);
	addKeyValue("N", Input::N);
	addKeyValue("O", Input::O);
	addKeyValue("P", Input::P);
	addKeyValue("Q", Input::Q);
	addKeyValue("R", Input::R);
	addKeyValue("S", Input::S);
	addKeyValue("T", Input::T);
	addKeyValue("U", Input::U);
	addKeyValue("V", Input::V);
	addKeyValue("W", Input::W);
	addKeyValue("X", Input::X);
	addKeyValue("Y", Input::Y);
	addKeyValue("Z", Input::Z);
	addKeyValue("a", Input::a);
	addKeyValue("b", Input::b);
	addKeyValue("c", Input::c);
	addKeyValue("d", Input::d);
	addKeyValue("e", Input::e);
	addKeyValue("f", Input::f);
	addKeyValue("g", Input::g);
	addKeyValue("h", Input::h);
	addKeyValue("i", Input::i);
	addKeyValue("k", Input::j);
	addKeyValue("k", Input::k);
	addKeyValue("l", Input::l);
	addKeyValue("m", Input::m);
	addKeyValue("n", Input::n);
	addKeyValue("o", Input::o);
	addKeyValue("p", Input::p);
	addKeyValue("q", Input::q);
	addKeyValue("r", Input::r);
	addKeyValue("s", Input::s);
	addKeyValue("t", Input::t);
	addKeyValue("u", Input::u);
	addKeyValue("v", Input::v);
	addKeyValue("w", Input::w);
	addKeyValue("x", Input::x);
	addKeyValue("y", Input::y);
	addKeyValue("z", Input::z);
	addKeyValue("F1", Input::F1);
	addKeyValue("F2", Input::F2);
	addKeyValue("F3", Input::F3);
	addKeyValue("F4", Input::F4);
	addKeyValue("F5", Input::F5);
	addKeyValue("F6", Input::F6);
	addKeyValue("F7", Input::F7);
	addKeyValue("F8", Input::F8);
	addKeyValue("F9", Input::F9);
	addKeyValue("F10", Input::F10);
	addKeyValue("F11", Input::F11);
	addKeyValue("F12", Input::F12);
	addKeyValue("UP", Input::UP);
	addKeyValue("DOWN", Input::DOWN);
	addKeyValue("LEFT", Input::LEFT);
	addKeyValue("RIGHT", Input::RIGHT);
	addKeyValue("RETURN", Input::RETURN);
	addKeyValue("PAGE_UP", Input::PAGE_UP);
	addKeyValue("PAGE_DOWN", Input::PAGE_DOWN);
	addKeyValue("HOME", Input::HOME);
	addKeyValue("END", Input::END);
	addKeyValue("INSERT", Input::INSERT);
	addKeyValue("DELETE", Input::DELETE);
	addKeyValue("BACK", Input::BACK);
	addKeyValue("SPACE", Input::SPACE);
	addKeyValue("ALT", Input::ALT);
	addKeyValue("CTRL", Input::CTRL);
	addKeyValue("ESC", Input::ESC);
	addKeyValue("SHIFT", Input::SHIFT);
	addKeyValue("TAB", Input::TAB);
	addKeyValue("MOUSE_L", Input::MOUSE_L);
	addKeyValue("MOUSE_M", Input::MOUSE_M);
	addKeyValue("MOUSE_R", Input::MOUSE_R);
	addKeyValue("MOUSE_4", Input::MOUSE_5);
	addKeyValue("MOUSE_5", Input::MOUSE_5);
	addKeyValue("TOUCH", Input::TOUCH);
	if (Input::KEY_COUNT != count) {
		LOGW("Mismatch between actually specified keys and key count.");
		LOGW("Defined keys: %d, specified keys: %d.", Input::KEY_COUNT, count);
	}
	addKeyValue("ACTION_CLICK", GUIManager::CLICK);
	lua_setglobal(state_, CONSTANTS.c_str());
	// Add engine specific functions and classes to default lua runtime.
	registerFunctions();
	registerClasses();
	unordered_map<string, int (*)(lua_State*)>::iterator it = functions_.begin();
	while (it != functions_.end()) {
		lua_register(ScriptManager::state_, it->first.c_str(), it->second);
		it++;
	}
    LOGD("Initialized script manager.");
}

bool ScriptManager::isFunctionAvailable(const string& functionName) {
	lua_getglobal(state_, functionName.c_str());
	int res = lua_isfunction(state_, lua_gettop(state_));
	return res == 1;
}

void ScriptManager::invokeFunction(const string& functionName) {
	lua_getglobal(state_, functionName.c_str());
	int result = lua_pcall(state_, 0, 0, 0);
	if (result != LUA_OK) {
		LOGE("Unable to call function \"%s\". Error: %s.",
			functionName.c_str(),
			lua_tostring(state_, lua_gettop(state_)));
	}
}

/**
 * Loads script on top of the current Lua stack,
 * then moves it to registry and returns unique id.
 * This way script can be used more than once
 * (Lua pops out scripts after using them).
 */
void ScriptManager::add(Node* node) {
//	if (script_ != 0) {
//		LOGE("Unable to add script. Script already specified.");
//		return;
//	}
	if (node->hasResource(Resource::SCRIPT)) {
		script_ = dynamic_cast<Script*>(
			node->getResource(Resource::SCRIPT));
		if (script_ == 0) {
			LOGE("Unable to cast resource to script object.");
			return;
		}
		if (script_->getId() == -1) {
			while (true) {
				int status = luaL_loadstring(
					ScriptManager::state_, script_->getScript().c_str());
				if (status != LUA_OK) {
					LOGE("Unable to load Lua script.\nError: %s",
						lua_tostring(ScriptManager::state_,
                        lua_gettop(ScriptManager::state_)));
					script_ = 0;
				}
				else {
					script_->setId(luaL_ref(state_, LUA_REGISTRYINDEX));
					lua_rawgeti(
						state_, LUA_REGISTRYINDEX, script_->getId());
					int result = lua_pcall(state_, 0, LUA_MULTRET, 0);
                    if (result != LUA_OK) {
                        LOGE("Unable to compile Lua script.\nError: %s",
                            lua_tostring(ScriptManager::state_, lua_gettop(ScriptManager::state_)));
                    }
					if (!isFunctionAvailable(FUNCTION_EVENT_RESPONSE)) {
						LOGE("Function \"%s\" not defined.", FUNCTION_EVENT_RESPONSE.c_str());
					}
					else if (!isFunctionAvailable(FUNCTION_EVENT_GUI)) {
						LOGE("Function \"%s\" not defined.", FUNCTION_EVENT_GUI.c_str());
					}
					else if (!isFunctionAvailable(FUNCTION_START)) {
						LOGE("Function \"%s\" not defined.", FUNCTION_START.c_str());
					}
					else if (!isFunctionAvailable(FUNCTION_RESIZE)) {
						LOGE("Function \"%s\" not defined.", FUNCTION_RESIZE.c_str());
					}
					else if (!isFunctionAvailable(FUNCTION_PAUSE)) {
						LOGE("Function \"%s\" not defined.", FUNCTION_PAUSE.c_str());
					}
					else if (!isFunctionAvailable(FUNCTION_RESUME)) {
						LOGE("Function \"%s\" not defined.", FUNCTION_RESUME.c_str());
					}
					else if (!isFunctionAvailable(FUNCTION_QUIT)) {
						LOGE("Function \"%s\" not defined.", FUNCTION_QUIT.c_str());
					}
					break;
				}
			}
		}
	}
}

void ScriptManager::provideResponse(HttpResponse* response) {
	lua_getglobal(state_, FUNCTION_EVENT_RESPONSE.c_str());
	HttpResponse** udata = (HttpResponse**) lua_newuserdata(state_, sizeof(HttpResponse*));
	*udata = response;
	luaL_getmetatable(state_, "Response");
	lua_setmetatable(state_, -2);
	int result = lua_pcall(state_, 1, 0, 0);
	if (result != LUA_OK) {
		LOGE("Unable to call function \"%s\". Error: %s.",
			FUNCTION_EVENT_RESPONSE.c_str(),
			lua_tostring(state_, lua_gettop(state_)));
	}
}

void ScriptManager::provideEventGUI(Node* node, int type) {
	lua_getglobal(state_, FUNCTION_EVENT_GUI.c_str());
	Node** udata = (Node**) lua_newuserdata(state_, sizeof(Node*));
	*udata = node;
	luaL_getmetatable(state_, "Node");
	lua_setmetatable(state_, -2);
	lua_pushnumber(state_, type);
	int result = lua_pcall(state_, 2, 0, 0);
	if (result != LUA_OK) {
		LOGE("Unable to call function \"%s\". Error: %s.",
			FUNCTION_EVENT_GUI.c_str(),
			lua_tostring(state_, lua_gettop(state_)));
	}
}

void ScriptManager::start() {
	if (isReady()) {
		invokeFunction(FUNCTION_START);
	}
}

void ScriptManager::resize() {
	if (isReady()) {
		invokeFunction(FUNCTION_RESIZE);
	}
}

/**
 * If a script is found in a registry table,
 * it is pushed into current stack with lua_rawgeti()
 * and then executed with lua_pcall().
 */
void ScriptManager::update() {
	if (!isReady()) {
		return;
	}
	lua_rawgeti(state_, LUA_REGISTRYINDEX, script_->getId());
	invokeFunction(FUNCTION_UPDATE);
	// Force garbage collection.
	lua_gc(state_, LUA_GCCOLLECT, 0);
}

void ScriptManager::pause() {
	if (isReady()) {
		invokeFunction(FUNCTION_PAUSE);
	}
}

void ScriptManager::resume() {
	if (isReady()) {
		invokeFunction(FUNCTION_RESUME);
	}
}

void ScriptManager::quit() {
	if (isReady()) {
		invokeFunction(FUNCTION_QUIT);
	}
}

void ScriptManager::addFunction(string name, int (*functionPtr)(lua_State*)) {
	functions_.insert(pair<string, int (*)(lua_State*)>(
		name, functionPtr));
}

void ScriptManager::addClass(
	string name, unordered_map<string, int (*)(lua_State*)> map)
{
    luaL_Reg* regs = NEW luaL_Reg[map.size() + 1];
    unordered_map<string, int (*)(lua_State*)>::iterator it = map.begin();
    UINT32 c = 0;
    while (it != map.end()) {
		luaL_Reg reg;
		reg.name = it->first.c_str();
		reg.func = it->second;
        regs[c++] = reg;
        it++;
    }
	luaL_Reg reg;
	reg.name = 0;
	reg.func = 0;
    regs[c] = reg;
    // Create a luaL metatable. This metatable is not
    // exposed to Lua. The "luaL_Foo" label is used by luaL
    // internally to identity things.
    luaL_newmetatable(ScriptManager::state_, name.c_str());
    // Register the C functions _into_ the metatable we just created.
	luaL_setfuncs(ScriptManager::state_, regs, 0);
    //luaL_register(l, 0, sFooRegs);
    // The Lua stack at this point looks like this:
    // 1| metatable "luaL_Foo" |-1
    lua_pushvalue(ScriptManager::state_, -1);
    // The Lua stack at this point looks like this:
    // 2| metatable "luaL_Foo" |-1
    // 1| metatable "luaL_Foo" |-2
    // Set the "__index" field of the metatable to point to itself
    // This pops the stack
    lua_setfield(ScriptManager::state_, -1, "__index");
    // The Lua stack at this point looks like this:
    // 1| metatable "luaL_Foo" |-1
    // The luaL_Foo metatable now has the following fields
    // - __gc
    // - __index
    // - add
    // - new
    // Now we use setglobal to officially expose the luaL_Foo metatable
    // to Lua. And we use the name "Foo".
    // This allows Lua scripts to _override_ the metatable of Foo.
    // For high security code this may not be called for but
    // we'll do this to get greater flexibility.
    lua_setglobal(ScriptManager::state_, name.c_str());
    delete regs;
}

ServiceLocator* ScriptManager::getServiceLocator() {
    return serviceLocator_;
}

lua_State* ScriptManager::getLuaState() {
	return state_;
}