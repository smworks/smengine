/*
 * Environment.cpp
 *
 *  Created on: 2012.10.22
 *      Author: MS
 */

#include "Environment.h"
#include <string.h>

Environment::Environment(void) {
	sunPos_ = NEW Vec3(50.0f, 100.0f, 50.0f);
	ambient_[0] = 0.1f;
	ambient_[1] = 0.1f;
	ambient_[2] = 0.1f;
	fogColor_[0] = 1.0f;
	fogColor_[1] = 1.0f;
	fogColor_[2] = 1.0f;
	fogDensity_ = 1.0f;
	LOGD("Created environment.");
}

Environment::~Environment(void) {
	if (sunPos_ != 0) {
		delete sunPos_;
	}
	LOGD("Deleted environment.");
}

void Environment::setSunPos(Vec3* pos) {
	if (sunPos_ != 0) {
		delete sunPos_;
	}
	sunPos_ = pos;
}

Vec3* Environment::getSunPos() {
	return sunPos_;
}

void Environment::setAmbient(float intensity[3]) {
	memcpy(ambient_, intensity, sizeof(float) * 3);
}

float* Environment::getAmbient() {
	return ambient_;
}

void Environment::setFogColor(float color[3]) {
	memcpy(fogColor_, color, sizeof(float) * 3);
}

float* Environment::getFogColor() {
	return fogColor_;
}

void Environment::setFogDensity(float density) {
	fogDensity_ = density;
}

float Environment::getFogDensity() {
	return fogDensity_;
}
