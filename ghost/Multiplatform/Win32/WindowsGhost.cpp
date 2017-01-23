/*
 * WindowsGhost.cpp
 *
 *  Created on: 2013.03.27
 *      Author: Martynas Šustavičius
 */

#include "WindowsGhost.h"

void logToFile(char* msg) {
	static ofstream ofs("log.txt");
	ofs << msg;
}

void profile(string msg) {
    static UINT64 lastCheck = getMicroseconds();
    UINT64 now = getMicroseconds();
    LOGI("%s Time: %llu (ms).", msg.c_str(), (now - lastCheck) / 1000);
    lastCheck = now;
}

INT64 getMicroseconds() {
	auto time = chrono::system_clock::now();
	auto since_epoch = time.time_since_epoch();
	auto microseconds = chrono::duration_cast<chrono::microseconds>(
		since_epoch);
	return microseconds.count();
}

bool checkGLError(string name) {
	//bool hasErrors = false;
 //   for (GLint error = glGetError(); error != 0; error = glGetError()) {
	//	hasErrors = true;
 //   	if (error == GL_INVALID_ENUM) {
	//		LOGE("%s: glError (GL_INVALID_ENUM)", name.c_str());
 //   	}
 //   	else if (error == GL_INVALID_VALUE) {
 //   		LOGE("%s: glError (GL_INVALID_VALUE)", name.c_str());
 //   	}
 //   	else if (error == GL_INVALID_OPERATION) {
 //   		LOGE("%s: glError (GL_INVALID_OPERATION)", name.c_str());
 //   	}
 //   	else if (error == GL_OUT_OF_MEMORY) {
 //   		LOGE("%s: glError (GL_OUT_OF_MEMORY)", name.c_str());
 //   	}
 //   	else {
 //   		LOGE("%s: glError (0x%x).", name.c_str(), error);
 //   	}
 //   }
	//return hasErrors;
	return false;
}