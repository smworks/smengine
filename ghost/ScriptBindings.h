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

/**
 * Node.
 */
void registerNode();

/**
 * Input.
 */
void registerInput();

/**
 * Model.
 */
void registerModel();

/**
 * Sprite.
 */
void registerSprite();

/**
 * Texture.
 */
void registerTexture();

/**
 * GUISurface.
 */
void registerGUISurface();

/**
 * GUIText.
 */
void registerGUIText();

/**
 * GUIButton.
 */
void registerGUIButton();

/**
 * Camera.
 */
void registerCamera();

/**
 * Shader.
 */
void registerShader();

/**
 * Network manager.
 */
void registerNetworkManager();

/**
 * HttpRequest.
 */
void registerHttpRequest();

/**
 * HttpResponse.
 */
void registerHttpResponse();

/**
 * Vehicle.
 */
void registerVehicle();

#endif
