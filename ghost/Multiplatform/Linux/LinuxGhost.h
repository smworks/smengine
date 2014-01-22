/*
 * LinuxGhost.h
 *
 *  Created on: 2013.03.26
 *      Author: Martynas Šustavičius
 */

#ifndef LINUXGHOST_H_
#define LINUXGHOST_H_

#if defined(_DEBUG) || defined(DEBUG) || defined(NDEBUG)
#define SMART_DEBUG
#endif

// BASIC CONSTANTS.
#define GHOST_= NEWLINE '\n'
#define GHOST_DEBUG true
#define GHOST_FRUSTUM_CULLING true
#define GHOST_DELTA 0.0000000001f
#define GHOST_LUA_KEYS "keys"
#define TO_STRING(x) #x
#define GHOST_DELETE(pointer)\
	if (pointer != 0) delete pointer; pointer = 0;

#define GHOST_RESOURCES(x) "resources/" x "/"
#define GHOST_SHADERS GHOST_RESOURCES("shaders")
#define GHOST_MODELS GHOST_RESOURCES("models")
#define GHOST_SCRIPTS GHOST_RESOURCES("scripts")
#define GHOST_SPRITES GHOST_RESOURCES("textures")
#define GHOST_SOUNDS GHOST_RESOURCES("sounds")
#define GHOST_SCENES GHOST_RESOURCES("scenes")
#define GHOST_FONTS GHOST_RESOURCES("fonts")

#include <stdio.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <string.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <algorithm>
#include <float.h>
#include <limits.h>
#include <stdarg.h>
#include <sys/time.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <resolv.h>

using namespace std;

typedef uint64_t UINT64;
typedef uint32_t UINT32;
typedef uint16_t UINT16;
typedef uint8_t UINT8;
typedef int64_t INT64;
typedef int32_t INT32;
typedef int16_t INT16;
typedef int8_t INT8;
typedef size_t SIZE;
typedef SIZE* POINTER;

// LOGGER.
#define LOGI(...) {\
	std::cout << "INFO: "; printf(__VA_ARGS__); cout << endl; }
#define LOGW(...) {\
    std::cout << "WARNING: "; printf(__VA_ARGS__); cout << endl; }
#define LOGE(...) {\
    std::cout << "ERROR: "; printf(__VA_ARGS__); cout << endl; }
#ifdef SMART_DEBUG
    #define LOGD(...) {\
        std::cout << "DEBUG: "; printf(__VA_ARGS__); cout << endl; }
#else
    #define LOGD(...)
#endif

// PROFILER
#ifdef SMART_DEBUG
    void profile(string& msg);
    #define PROFILE(...) {\
        char message[1024];\
        sprintf(message, __VA_ARGS__);\
        string msg = message;\
        profile(msg);\
    }
#else
    #define PROFILE(...)
#endif

// GRAPHICS.
#define GLEW_STATIC
#include <GL/glew.h>

#ifdef _DEBUG
	#define CHECK_GL_ERROR(msg) checkGLError(msg)
#else
	#define CHECK_GL_ERROR(msg)
#endif

// GLOBAL METHODS.

/**
 * @return Time in microseconds since 1970 January 1.
 */
SIZE getMicroseconds();

/**
 * Prints information about OpenGL error (if it occurred).
 * @param name - name of last OpenGL method, or any sort of text.
 * @return true if error was found.
 */
bool checkGLError(string name);

#endif
