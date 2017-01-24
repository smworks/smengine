/*
 * ScriptManger.h
 *
 *  Created on: 2013.02.25
 *      Author: Martynas Šustavičius
 */

#ifndef SCRIPTMANAGER_H_
#define SCRIPTMANAGER_H_

#include "Multiplatform/Ghost.h"
class ServiceLocator;
class Node;
class Script;
class HttpResponse;
#include "../dependencies/includes/lua/lua.hpp"

class ScriptManager
{
public:
	const string FUNCTION_EVENT_RESPONSE = "eventResponse";
	const string FUNCTION_EVENT_GUI = "eventGUI";
	const string FUNCTION_START = "start";
	const string FUNCTION_RESIZE = "resize";
	const string FUNCTION_UPDATE = "update";
	const string FUNCTION_PAUSE = "pause";
	const string FUNCTION_RESUME = "resume";
	const string FUNCTION_QUIT = "quit";
	const string CONSTANTS = "constants";

	ScriptManager();
	~ScriptManager();

	/**
	 * @return True if Script manager is initialized
	 * and has a valid script.
	 */
	bool isReady() const;

	/**
	 * Must be called after all of the other object that
	 * use this object, has added their functions.
	 * This method will actually add these methods added later to
	 * Lua runtime.
	 * @param locator - pointer to class that has access to all engine services.
	 */
	void initialize(ServiceLocator* locator) const;

	static void addKeyMapping(lua_State * state, string luaKey, int key);

	/**
	 * Checks if function is defined in script file.
	 * @param functionName - name of the function.
	 * @return True if function is defined.
	 */
	static bool isFunctionAvailable(const string& functionName);

	/**
	 * Calls function with specified name and.
	 * No parameters or return values are used.
	 * @param functionName - name of the function.
	 */
	inline void invokeFunction(const string& functionName) const;

	static string executeCode(string code);

	/**
	 * @param node - node that contains script resource.
	 */
	void add(Node* node);

	/**
	 * Sends network response to be handled
	 * on lua side.
	 * @param response - pointer to network response.
	 */
	void provideResponse(HttpResponse* response) const;

	/**
	 * Sends event notification to script side.
	 * @param node - pointer to node, that received event.
	 * @param type - type of the event.
	 */
	void provideEventGUI(Node* node, int type) const;

	/**
	 * Executes start method in script side.
	 */
	void start() const;

	/**
	 * Executes resize methdod in script side.
	 */
	void resize() const;

	/**
	 * Executes update methdod in script side.
	 */
	void update() const;

	/**
	 * Executes pause method in script side.
	 */
	void pause() const;

	/**
	 * Executes resume method in script side.
	 */
	void resume() const;

	/**
	 * Executes quit method in script side.
	 */
	void quit() const;

	static void dumpStack(lua_State* state);

	/**
	 * Adds function to script side.
	 * @param name - function name as it would be used in script side.
	 * @param functionPtr - function that will be called.
	 */
	static void addFunction(string name, int (*functionPtr)(lua_State*));

	/**
	 * Adds class to script side.
	 * @param name - name of the class as well as how it will be
	 * referenced in code.
	 * @param map - map containing method names and pointers to methods.
	 */
	static void addClass(
		string name, unordered_map<string, int (*)(lua_State*)> map);

	/**
	 * Provides static access for all
	 * lua functions added or overriden by game engine.
	 * @return ServiceLocator object.
	 */
	static ServiceLocator* getServiceLocator();

	/**
	 * @return Pointer to lua state.
	 */
	static lua_State* getLuaState();

	static lua_State* state;
	static unordered_map<string, int(*)(lua_State*)> functions;
	static ServiceLocator* serviceLocator;
private:
	Script* script;
};

#endif
