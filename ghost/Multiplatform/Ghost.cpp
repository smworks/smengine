#include "Ghost.h"

#ifdef _WIN32
	#ifdef SMART_DEBUG
	void profile(string msg) {
		static UINT64 lastCheck = getMicroseconds();
		UINT64 now = getMicroseconds();
		LOGI("%s Time: %llu (ms).", msg.c_str(), (now - lastCheck) / 1000);
		lastCheck = now;
	}
    #else
    void profile(string msg) {}
	#endif

	void logToFile(char* msg) {
		static ofstream ofs("log.txt");
		ofs << msg;
	}

	INT64 getMicroseconds() {
		auto time = chrono::system_clock::now();
		auto since_epoch = time.time_since_epoch();
		auto microseconds = chrono::duration_cast<chrono::microseconds>(
			since_epoch);
		return microseconds.count();
	}
#else
#ifdef SMART_DEBUG
void profile(string& msg) {
	static UINT64 lastCheck = getMicroseconds();
	UINT64 now = getMicroseconds();
	LOGD("%s Time: %lu (ms).", msg.c_str(), (now - lastCheck) / 1000);
	lastCheck = now;
}
#endif

INT64 getMicroseconds() {
    timeval time;
    gettimeofday(&time, 0);
    return time.tv_sec * 1000000 + time.tv_usec;
}
#endif



bool checkGLError(string name) {
    bool hasErrors = false;
    for (GLint error = glGetError(); error != 0; error = glGetError()) {
        hasErrors = true;
        if (error == GL_INVALID_ENUM) {
            LOGE("%s: glError (GL_INVALID_ENUM)", name.c_str());
        }
        else if (error == GL_INVALID_VALUE) {
            LOGE("%s: glError (GL_INVALID_VALUE)", name.c_str());
        }
        else if (error == GL_INVALID_OPERATION) {
            LOGE("%s: glError (GL_INVALID_OPERATION)", name.c_str());
        }
        else if (error == GL_OUT_OF_MEMORY) {
            LOGE("%s: glError (GL_OUT_OF_MEMORY)", name.c_str());
        }
        else {
            LOGE("%s: glError (0x%x).", name.c_str(), error);
        }
    }
    return hasErrors;
}

void printGLString(string name, int id) {
#ifdef ENABLE_PHYSICS
    const char *info = (const char *) glGetString(id);
    LOGI("GL %s = %s.", name.c_str(), info);
#endif
}