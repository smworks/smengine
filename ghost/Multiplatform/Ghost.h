#ifndef GHOST_H_
#define GHOST_H_

#include <stdio.h>
#include <vector>
#include <string>
#include <string.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <unistd.h>
#include <algorithm>
#include <float.h>
#include <limits.h>
#include <stdarg.h>
#include <sys/time.h>
#include <stdint.h>
#include <unordered_map>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <resolv.h>
#include <errno.h>
#include <queue>
#include <assert.h>
#include <thread>
#include <mutex>

using namespace std;

#define SMART_PI 3.14159265358979323846

// ENGINE SUBSYSTEMS
#ifndef DISABLE_ALL
#define ENABLE_FONTS
//#define ENABLE_PHYSICS
#define ENABLE_DATABASE
//#define ENABLE_SOUND
#define ENABLE_GRAPHICS
#endif

#define NEW new

typedef unsigned long UINT64;
typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;
typedef long long INT64;
typedef int INT32;
typedef short INT16;
typedef char INT8;
typedef size_t SIZE;
typedef SIZE* POINTER;



#if _WIN32
#include "Win32/WindowsGhost.h"
#elif __ANDROID__

//typedef uint64_t UINT64;
//typedef uint32_t UINT32;
//typedef uint16_t UINT16;
//typedef uint8_t UINT8;
//typedef int64_t INT64;
//typedef int32_t INT32;
//typedef int16_t INT16;
//typedef int8_t INT8;
//typedef size_t SIZE;
//typedef SIZE* POINTER;

    #include <android/asset_manager.h>
    #include <android/log.h>
    #include <jni.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>

    #ifndef NDEBUG
    #define SMART_DEBUG
    #endif

    #define GHOST_NEWLINE '\n'
    #define GHOST_DELTA 0.0000000001f

    #define GHOST_RESOURCES(x) x "/"

    // LOGGER.
    #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "SMart Engine", __VA_ARGS__))
    #define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "SMart Engine", __VA_ARGS__))
    #define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "SMart Engine", __VA_ARGS__))
    #ifdef SMART_DEBUG
    #define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "SMart Engine", __VA_ARGS__))
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

    #define RETURN(val, cond, ...) \
    if (cond) { \
        LOGW(__VA_ARGS__); \
        return val; \
    }

    #ifdef SMART_DEBUG
    #define CHECK_GL_ERROR(msg) checkGLError(msg)
    #define ASSERT(expression, ...) \
        if (!(expression)) { LOGE(__VA_ARGS__); } assert(expression)
    #else
    #define CHECK_GL_ERROR(msg)
        #define ASSERT(expression, ...) if (!(expression)) { LOGE(__VA_ARGS__); }
    #endif

    #define THROWEX(...)
    #define THROWONASSERT(...)

#elif __linux
    #include "Linux/LinuxGhost.h"
#elif __MACH__
    #include <numeric>

    #define GHOST_NEWLINE '\n'
    #define GHOST_DEBUG true
    #define GHOST_FRUSTUM_CULLING true
    #define GHOST_DELTA 0.0000000001f
    #define GHOST_LUA_KEYS "keys"
    #define TO_STRING(x) #x
    #define GHOST_DELETE(pointer) if (pointer != 0) delete pointer; pointer = 0;
    #define GHOST_RESOURCES(x) "resources/" x "/"

    #ifdef DEBUG
        #define SMART_DEBUG
    #endif

    #ifdef ENABLE_GRAPHICS
        #include <OpenGL/gl3.h>
    #endif

    // LOGGER.
    #define LOGI(...) {\
    cout << "INFO: "; printf(__VA_ARGS__); cout << endl; }
    #define LOGW(...) {\
    cout << "WARNING: "; printf(__VA_ARGS__); cout << endl; }
    #define LOGE(...) {\
    cout << "ERROR: "; printf(__VA_ARGS__); cout << endl; }

    #ifdef SMART_DEBUG
        #define LOGD(...) {\
        cout << "DEBUG: "; printf(__VA_ARGS__); cout << endl; }
        #define LOGDEXT LOGD
    #else
        #define LOGD(...)
        #define LOGDEXT LOGD
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
if (!(expression)) { LOGE(__VA_ARGS__); } //assert(expression)
#else
#define CHECK_GL_ERROR(msg)
#define ASSERT(expression, ...) if (!(expression)) { LOGE(__VA_ARGS__); }
#endif

#define THROWEX(...)
#define THROWONASSERT(...)

#endif

#define GHOST_SHADERS GHOST_RESOURCES("shaders")
#define GHOST_MODELS GHOST_RESOURCES("models")
#define GHOST_SCRIPTS GHOST_RESOURCES("scripts")
#define GHOST_SPRITES GHOST_RESOURCES("textures")
#define GHOST_SOUNDS GHOST_RESOURCES("sounds")
#define GHOST_FONTS GHOST_RESOURCES("fonts")

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

/**
 * Prints information about specified OpenGL component.
 * @param name - name of the component.
 * @param id - OpenGL id of the component.
 */
inline void printGLString(string name, int id);

#endif /* GHOST_H_ */
