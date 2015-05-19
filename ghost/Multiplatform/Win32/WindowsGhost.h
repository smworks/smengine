/*
 * WindowsGhost.h
 *
 *  Created on: 2013.03.27
 *      Author: Martynas Šustavičius
 */

#ifndef WINDOWSGHOST_H_
#define WINDOWSGHOST_H_

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_NO_WARNINGS // Disable warnings for sscanf_s and such.
#define NOMINMAX // Disable min and max macros.

#ifdef _DEBUG
#define SMART_DEBUG
#endif

// BASIC CONSTANTS.
#define GHOST_NEWLINE '\n'
#define GHOST_DEBUG true
#define GHOST_FRUSTUM_CULLING true
#define GHOST_DELTA 0.0000000001f
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

#ifdef SMART_DEBUG
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
	#define NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#else
	#define NEW new
#endif

#include <stdio.h>
#include <unordered_map>
#include <queue>
#include <string>
#include <time.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <windows.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <chrono>
#include <thread>
#include <mutex>
// GRAPHICS.
#define GLEW_STATIC
#include <glew.h>
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

using namespace std;
using namespace stdext; // For older compilers.

#undef DELETE
#undef RELATIVE

typedef unsigned long long UINT64;
typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;
typedef long long INT64;
typedef int INT32;
typedef short INT16;
#define INT8 char
#define SIZE size_t
typedef SIZE* POINTER;

void logToFile(char* msg);

#define OUTPUT(msg) OutputDebugStringA(msg); logToFile(msg);

// LOGGER.
#define LOGI(...) { char buff[8192];\
	sprintf_s(buff, __VA_ARGS__); \
	OUTPUT("INFO: "); \
	OUTPUT(buff); \
	char buffLine[256]; \
	sprintf_s(buffLine, 256, " (%s, %d)\n", \
		__FILE__, __LINE__); \
	OUTPUT(buffLine); }
#define LOGW(...) { char buff[8192]; \
	sprintf_s(buff, __VA_ARGS__); \
	OUTPUT("WARNING: "); \
	OUTPUT(buff); \
	char buffLine[256]; \
	sprintf_s(buffLine, 256, " (%s, %d)\n", \
		__FILE__, __LINE__); \
	OUTPUT(buffLine) }
#define LOGE(...) { char buff[8192]; \
	sprintf_s(buff, __VA_ARGS__); \
	MessageBoxA(0, buff, "Error", MB_ICONERROR | MB_SYSTEMMODAL); \
	OUTPUT("ERROR: "); \
	OUTPUT(buff); \
	char buffLine[256]; \
	sprintf_s(buffLine, 256, " (%s, %d)\n", \
		__FILE__, __LINE__); \
	OUTPUT(buffLine); }
#ifdef SMART_DEBUG
	#define LOGD(...) { char buff[8192]; \
		sprintf_s(buff, __VA_ARGS__); \
		OUTPUT("DEBUG: "); \
		OUTPUT(buff); \
		char buffLine[256]; \
		sprintf_s(buffLine, 256, " (%s, %d)\n", \
			__FILE__, __LINE__); \
		OUTPUT(buffLine); }
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
    #define PROFILE(msg)
#endif

#define RETURN(val, cond, ...) \
	if (cond) { \
		LOGW(__VA_ARGS__); \
		return val; \
	}

#ifdef SMART_DEBUG
	#include <assert.h>
	#define CHECK_GL_ERROR(msg) checkGLError(msg)
	#define ASSERT(expression, ...) \
		if (!(expression)) { LOGE(__VA_ARGS__); } assert(expression)
	#define THROWEX(msg) throw runtime_error(msg)
#else
	#define CHECK_GL_ERROR(msg)
	#define ASSERT(expression, ...) if (!(expression)) { LOGE(__VA_ARGS__); }
	#define THROWEX(msg) throw runtime_error(msg)
#endif


/**
 * @return Time in microseconds since 1970 January 1.
 */
INT64 getMicroseconds();

/*
 * Prints information about OpenGL error (if it occured).
 * @param name - name of last OpenGL method, or any sort of text.
 * @return true if error was found.
 */
bool checkGLError(string name);

#endif
