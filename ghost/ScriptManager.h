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

class ScriptManager {
public:
	static const string FUNCTION_EVENT_RESPONSE;
	static const string FUNCTION_EVENT_GUI;
	static const string FUNCTION_START;
	static const string FUNCTION_RESIZE;
	static const string FUNCTION_UPDATE;
	static const string FUNCTION_PAUSE;
	static const string FUNCTION_RESUME;
	static const string FUNCTION_QUIT;
	static const string CONSTANTS;
public:
	ScriptManager();
	~ScriptManager();

	/**
	 * @return True if Script manager is initialized
	 * and has a valid script.
	 */
	bool isReady();

	/**
	 * Must be called after all of the other object that
	 * use this object, has added their functions.
	 * This method will actually add these methods added later to
	 * Lua runtime.
	 * @param locator - pointer to class that has access to all engine services.
	 */
	void initialize(ServiceLocator* locator);

	/**
	 * Checks if function is defined in script file.
	 * @param functionName - name of the function.
	 * @return True if function is defined.
	 */
	bool isFunctionAvailable(const string& functionName);

	/**
	 * Calls function with specified name and.
	 * No parameters or return values are used.
	 * @param functionName - name of the function.
	 */
	inline void invokeFunction(const string& functionName);

	string executeCode(string code);

	/**
	 * @param node - node that contains script resource.
	 */
	void add(Node* node);

	/**
	 * Sends network response to be handled
	 * on lua side.
	 * @param response - pointer to network response.
	 */
	void provideResponse(HttpResponse* response);

	/**
	 * Sends event notification to script side.
	 * @param node - pointer to node, that received event.
	 * @param type - type of the event.
	 */
	void provideEventGUI(Node* node, int type);

	/**
	 * Executes start method in script side.
	 */
	void start();

	/**
	 * Executes resize methdod in script side.
	 */
	void resize();

	/**
	 * Executes update methdod in script side.
	 */
	void update();

	/**
	 * Executes pause method in script side.
	 */
	void pause();

	/**
	 * Executes resume method in script side.
	 */
	void resume();

	/**
	 * Executes quit method in script side.
	 */
	void quit();

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
private:
	/** Pointer to lua runtime. */
	static lua_State* state_;
	/** Map containing all extended Lua functionality. */
	static unordered_map<string, int (*)(lua_State*)> functions_;
	/** Pointer to service locator. */
	static ServiceLocator* serviceLocator_;
	/** Script resource. */
	static Script* script_;
};

#endif
