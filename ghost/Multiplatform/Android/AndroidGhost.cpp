/*
 * AndroidGhost.cpp
 *
 *  Created on: 2013.03.27
 *      Author: Martynas Šustavičius
 */

#include "AndroidGhost.h"

#ifdef SMART_DEBUG
    void profile(string& msg) {
        static UINT64 lastCheck = getMicroseconds();
        UINT64 now = getMicroseconds();
        LOGD("%s Time: %llu (ms).", msg.c_str(), (now - lastCheck) / 1000);
        lastCheck = now;
    }
#endif

SIZE getMicroseconds() {
    timeval time;
    gettimeofday(&time, 0);
    return time.tv_sec * 1000000 + time.tv_usec;
}

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
