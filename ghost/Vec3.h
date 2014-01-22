/*
 * Vec3.h
 *
 *  Created on: 2012.07.02
 *      Author: MS
 */

#ifndef Vec3_H_
#define Vec3_H_

#include "Multiplatform/Ghost.h"

class Vec3 {
public:
	Vec3();
	Vec3(float val);
	Vec3(float x, float y, float z);
	Vec3(float vec[3]);
	Vec3(const Vec3& vec);

	Vec3 operator+(const Vec3& vec);
	Vec3 operator-(const Vec3& vec);
	Vec3 operator*(const Vec3& vec);
	Vec3 operator/(const Vec3& vec);
	Vec3& operator+=(const Vec3& vec);
	Vec3& operator-=(const Vec3& vec);
	Vec3& operator*=(const Vec3& vec);
	Vec3& operator/=(const Vec3& vec);
	Vec3 operator+(float val);
	Vec3 operator-(float val);
	Vec3 operator*(float val);
	Vec3 operator/(float val);
	Vec3& operator+=(float val);
	Vec3& operator-=(float val);
	Vec3& operator*=(float val);
	Vec3& operator/=(float val);

	/**
	 * Calculates cross product of two vectors.
	 * @param vec - second vector that
	 * needs to be multiplied with current one.
	 */
	void crossProduct(const Vec3& vec);

	/**
	 * @return Length of the vector.
	 */
	float length();

	/**
	 * Normalizes vector to have
	 * overall magnitude of 1.
	 */
	void normalize();

	/**
	 * Multiplies each attribute by -1.
	 */
	void invert();

	/**
	 * Mixes current with sepcified
	 * vector, by specified amount.
	 * @param vec - vector that will be mixed
	 * with current one.
	 * @param ratio - mix ratio between 0.0f and 1.0f,
	 * where 0.0f leaves only current vector data
	 * and 1.0f leaves specified vector data.
	 */
	void mix(const Vec3& vec, float ratio);

	void addX(float);
	void addY(float);
	void addZ(float);
	void addXYZ(float, float, float);
	void addXYZ(const Vec3&);
	void setX(float);
	void setY(float);
	void setZ(float);
	void setXYZ(float, float, float);
	void setXYZ(const Vec3&);
	float getX();
	float getY();
	float getZ();
	float* toArray();
	void getXYZ(float vec[3]);
	float* getPointer();

	/**
	 * @return Vector component with lowest value.
	 */
	float getMinVal();

	/**
	 * @return Vector component with highest value.
	 */
	float getMaxVal();

	/**
	 * @param space - space between message.
	 */
	void print(string space = "");
private:
	float vec_[3]; // Vector components.
};

#endif /* Vec3_H_ */
