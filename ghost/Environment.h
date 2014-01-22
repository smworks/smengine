/*
 * Environment.h
 *
 *  Created on: 2012.10.22
 *      Author: MS
 */
#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include "Multiplatform/Ghost.h"
#include "Vec3.h"

class Environment
{
public:
	Environment(void);
	~Environment(void);
	/**
	 * Sets sun position.
	 * @param pos - pointer to position vector.
	 * @NOTE: Environment class handles object deletion itself.
	 */
	void setSunPos(Vec3* pos);
	/**
	 * Returns sun position vector.
	 * @return sun position vector.
	 */
	Vec3* getSunPos();
	/**
	 * Sets ambient scene light intensity.
	 * @param intensity - ambient light intensity made of three floats.
	 */
	void setAmbient(float intensity[3]);
	/**
	 * Returns ambient light intensity.
	 * @return ambient light intensity.
	 */
	float* getAmbient();
	/**
	 * Fog variables.
	 */
	void setFogColor(float color[3]);
	float* getFogColor();
	void setFogDensity(float density);
	float getFogDensity();
private:
	/** Sun position. */
	Vec3* sunPos_;
	/** Ambient light intensity. */
	float ambient_[3];

	/** Fog color. */
	float fogColor_[3];
	/** Fog intensity. */
	float fogDensity_;

};

#endif /* ENVIRONMENT_H_ */