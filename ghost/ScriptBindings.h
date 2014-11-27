/*
 * ScriptBingings.h
 *
 *  Created on: 2013.11.03
 *      Author: Martynas Šustavičius
 */

#ifndef SCRIPTBINDINGS_H_
#define SCRIPTBINDINGS_H_

#include "Multiplatform/Ghost.h"
class ServiceLocator;
class Node;
class Script;
struct lua_State;

/**
 * Registers all function bindings.
 */
void registerFunctions();

/**
 * Registers all class bindings.
 */
void registerClasses();

void registerNode();
void registerInput();
void registerModel();
void registerSprite();
void registerTexture();
void registerGUISurface();
void registerGUIText();
void registerGUIButton();
void registerCamera();
void registerShader();
void registerNetworkManager();
void registerHttpRequest();
void registerHttpResponse();
void registerVehicle();
void registerScenario();
void registerScenarioManager();
void registerSceneManager();
void registerSettings();

#endif
