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
#include "ScriptBindings.h"
#include "GUIManager.h"

lua_State* ScriptManager::state = nullptr;
unordered_map<string, int(*)(lua_State*)> ScriptManager::functions;
ServiceLocator* ScriptManager::serviceLocator = nullptr;


ScriptManager::ScriptManager(): script(nullptr)
{
	state = luaL_newstate();
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
	//			ScriptManager::state, library->name, library->func, 1);
	//		lua_settop(ScriptManager::state, 0);
	//	}
	luaL_openlibs(state);
	LOGD("Created script manager.");
}

ScriptManager::~ScriptManager()
{
	if (state != nullptr)
	{
		lua_close(state);
		state = nullptr;
		script = nullptr;
	}
	LOGD("Deleted script manager.");
}

bool ScriptManager::isReady() const
{
	return script != nullptr;
}

void ScriptManager::initialize(ServiceLocator* locator) const
{
	this->serviceLocator = locator;
	lua_createtable(state, Input::KEY_COUNT + GUIManager::ACTION_COUNT, 0);

	string arr[] = {"NONE", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
		"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N",
		"O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
		"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
		"F11", "F12", "UP", "DOWN", "LEFT", "RIGHT", "RETURN", "PAGE_UP", "PAGE_DOWN",
		"HOME", "END", "INSERT", "DELETE", "BACK", "SPACE", "ALT", "CTRL",
		"ESC", "SHIFT", "TAB", "SEMICOLON", "PLUS", "COMMA", "MINUS",
		"PERIOD", "SLASH", "TILDE", "MOUSE_L", "MOUSE_M", "MOUSE_R",
		"MOUSE_4", "MOUSE_5", "TOUCH"
	};

	for (int i = Input::NONE; i < Input::KEY_COUNT; i++)
	{
		addKeyMapping(state, arr[i], i);
	}
	addKeyMapping(state, "ACTION_CLICK", GUIManager::CLICK);

	lua_setglobal(state, CONSTANTS.c_str());
	// Add engine specific functions and classes to default lua runtime.
	registerFunctions();
	registerClasses();
	for (auto it = functions.begin(); it != functions.end(); ++it)
	{
		lua_register(state, it->first.c_str(), it->second);
	}
	LOGD("Initialized script manager.");
}

void ScriptManager::addKeyMapping(lua_State* state, string luaKey, int key)
{
	lua_pushstring(state, luaKey.c_str());
	lua_pushnumber(state, key);
	lua_settable(state, -3);
}

bool ScriptManager::isFunctionAvailable(const string& functionName)
{
	lua_getglobal(state, functionName.c_str());
	return lua_isfunction(state, lua_gettop(state)) == 1;
}

void ScriptManager::invokeFunction(const string& functionName) const
{
	lua_getglobal(state, functionName.c_str());
	int result = lua_pcall(state, 0, 0, 0);
	if (result != LUA_OK)
	{
		dumpStack(state);
		LOGE("Unable to call function \"%s\". Error: %s.",
			functionName.c_str(),
			lua_tostring(state, -1));
	}
	lua_settop(state, 0);
}

string ScriptManager::executeCode(string code)
{
	return luaL_dostring(state, code.c_str()) != LUA_OK
		? lua_tostring(state, lua_gettop(state)) : "";
}

/**
 * Loads script on top of the current Lua stack,
 * then moves it to registry and returns unique id.
 * This way script can be used more than once
 * (Lua pops out scripts after using them).
 */
void ScriptManager::add(Node* node)
{
	ASSERT(node->getResource()->getType() == Resource::SCRIPT,
		"Node with name %s does not contain script resource.", node->getName().c_str());

	ASSERT((script = dynamic_cast<Script*>(node->getResource())) != nullptr,
		"Unable to cast resource to script object.");

	ASSERT(script->getId() == -1, "Script id is not -1");

	ASSERT(LUA_OK == luaL_loadstring(state, script->getScript().c_str()),
		"Unable to load Lua script.\nError: %s", lua_tostring(state, lua_gettop(state)));

	script->setId(luaL_ref(state, LUA_REGISTRYINDEX));
	lua_rawgeti(state, LUA_REGISTRYINDEX, script->getId());

	ASSERT(LUA_OK == lua_pcall(state, 0, LUA_MULTRET, 0),
		"Unable to compile Lua script.\nError: %s", lua_tostring(state, lua_gettop(state)));

	vector<string> functions = {FUNCTION_EVENT_RESPONSE,
		FUNCTION_EVENT_RESPONSE, FUNCTION_EVENT_GUI,
		FUNCTION_START, FUNCTION_RESIZE, FUNCTION_PAUSE,
		FUNCTION_RESUME, FUNCTION_QUIT};

	for (string function : functions)
	{
		ASSERT(isFunctionAvailable(function), "Function \"%s\" not defined.", function.c_str());
	}
}

void ScriptManager::provideResponse(HttpResponse* response) const
{
	lua_getglobal(state, FUNCTION_EVENT_RESPONSE.c_str());
	HttpResponse** udata = static_cast<HttpResponse**>(lua_newuserdata(state, sizeof(HttpResponse*)));
	*udata = response;
	luaL_getmetatable(state, "Response");
	lua_setmetatable(state, -2);
	int result = lua_pcall(state, 1, 0, 0);
	if (result != LUA_OK)
	{
		dumpStack(state);
		LOGE("Unable to call function \"%s\". Error: %s.",
			FUNCTION_EVENT_RESPONSE.c_str(),
			lua_tostring(state, lua_gettop(state)));
	}
	lua_settop(state, 0);
}

void ScriptManager::provideEventGUI(Node* node, int type) const
{
	lua_getglobal(state, FUNCTION_EVENT_GUI.c_str());
	Node** udata = static_cast<Node**>(lua_newuserdata(state, sizeof(Node*)));
	*udata = node;
	luaL_getmetatable(state, "Node");
	lua_setmetatable(state, -2);
	lua_pushnumber(state, type);
	if (lua_pcall(state, 2, 0, 0) != LUA_OK)
	{
		dumpStack(state);
		LOGE("Unable to call function \"%s\". Error: %s.",
			FUNCTION_EVENT_GUI.c_str(),
			lua_tostring(state, lua_gettop(state)));
	}
}

void ScriptManager::start() const
{
	if (isReady())
	{
		invokeFunction(FUNCTION_START);
	}
}

void ScriptManager::resize() const
{
	if (isReady())
	{
		invokeFunction(FUNCTION_RESIZE);
	}
}

/**
 * If a script is found in a registry table,
 * it is pushed into current stack with lua_rawgeti()
 * and then executed with lua_pcall().
 */
void ScriptManager::update() const
{
	if (!isReady())
	{
		return;
	}
	//lua_rawgeti(state, LUA_REGISTRYINDEX, script->getId());
	invokeFunction(FUNCTION_UPDATE);
	//	// Force garbage collection.
	//	lua_gc(state, LUA_GCCOLLECT, 0);
}

void ScriptManager::pause() const
{
	if (isReady())
	{
		invokeFunction(FUNCTION_PAUSE);
	}
}

void ScriptManager::resume() const
{
	if (isReady())
	{
		invokeFunction(FUNCTION_RESUME);
	}
}

void ScriptManager::quit() const
{
	if (isReady())
	{
		invokeFunction(FUNCTION_QUIT);
	}
}

void ScriptManager::dumpStack(lua_State* state)
{
	int i = lua_gettop(state);
	LOGI("---Lua stack dump begin--------------------------------" );
	while (i)
	{
		int t = lua_type(state, i);
		switch (t)
		{
		case LUA_TSTRING:
			LOGI("%d:`%s'", i, lua_tostring(state, i));
			break;
		case LUA_TBOOLEAN:
			LOGI("%d: %s", i, lua_toboolean(state, i) ? "true" : "false");
			break;
		case LUA_TNUMBER:
			LOGI("%d: %g", i, lua_tonumber(state, i));
			break;
		default:
			LOGI("%d: %s", i, lua_typename(state, t));
			break;
		}
		i--;
	}
	LOGI("---Lua stack dump complete-----------------------------" );
}

void ScriptManager::addFunction(string name, int (*functionPtr)(lua_State*))
{
	functions.insert(pair<string, int (*)(lua_State*)>(
		name, functionPtr));
}

void ScriptManager::addClass(
	string name, unordered_map<string, int (*)(lua_State*)> map)
{
	luaL_Reg* regs = NEW luaL_Reg[map.size() + 1];

	UINT32 c = 0;
	auto it = map.begin();
	while (it != map.end())
	{
		luaL_Reg reg;
		reg.name = it->first.c_str();
		reg.func = it->second;
		regs[c++] = reg;
		++it;
	}
	luaL_Reg reg;
	reg.name = nullptr;
	reg.func = nullptr;
	regs[c] = reg;
	// Create a luaL metatable. This metatable is not
	// exposed to Lua. The "luaL_Foo" label is used by luaL
	// internally to identity things.
	luaL_newmetatable(state, name.c_str());
	// Register the C functions _into_ the metatable we just created.
	luaL_setfuncs(state, regs, 0);
	//luaL_register(l, 0, sFooRegs);
	// The Lua stack at this point looks like this:
	// 1| metatable "luaL_Foo" |-1
	lua_pushvalue(state, -1);
	// The Lua stack at this point looks like this:
	// 2| metatable "luaL_Foo" |-1
	// 1| metatable "luaL_Foo" |-2
	// Set the "__index" field of the metatable to point to itself
	// This pops the stack
	lua_setfield(state, -1, "__index");
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
	lua_setglobal(state, name.c_str());
	delete [] regs;
}

ServiceLocator* ScriptManager::getServiceLocator()
{
	return serviceLocator;
}

lua_State* ScriptManager::getLuaState()
{
	return state;
}
