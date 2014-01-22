/*
 * Quaternion.h
 *
 *  Created on: 2013.03.08
 *      Author: Martynas Šustavičius
 *
 * Some formulas:
 * if |q|=1 then q is a unit quaternion.
 * if q=(0,v) then q is a pure quaternion .
 * if |q|=1 then q conjugate = q inverse.
 * if |q|=1 then q= [cos(angle), u*sin(angle)] where u is a unit vector .
 * q and -q represent the same rotation .
 * q*q.conjugate = (q.length_squared, 0).
 * ln(cos(theta),sin(theta)*v)= ln(e^(theta*v))= (0, theta*v).
 */

#ifndef QUATERNION_H_
#define QUATERNION_H_

#include "Utils.h"

class Quaternion {
public:
	Quaternion();

	/**
	 * Creates quaternion from euler angles.
	 * @param x - rotation around x axis.
	 * @param y - rotation around y axis.
	 * @param z - rotation around z axis.
	 */
	Quaternion(float x, float y, float z);

	/**
	 * Creates quaternion from quaternion components.
	 * @param x - x component.
	 * @param y - y component.
	 * @param z - z component.
	 * @param w - w component.
	 */
	Quaternion(float x, float y, float z, float w);

	Quaternion operator* (const Quaternion& q);
	Quaternion operator* (float val);

	/**
	 * @return Length of the quaternion.
	 */
	float getLength();

	/**
	 * Inverts quaternion.
	 */
	void invert();

	/**
	 * Normalises quaternion.
	 */
	void normalise();

	/**
	 * Sets up quaternion from euler angles.
	 * @param x - rotation around x axis.
	 * @param y - rotation around y axis.
	 * @param z - rotation around z axis.
	 */
	void setXYZ(float x, float y, float z);

	/**
	 * Sets quaternion components.
	 * @param x
	 * @param y
	 * @param z
	 * @param w
	 */
	void setXYZW(float x, float y, float z, float w);

	/**
	 * @return Matrix representation of quaternion.
	 */
	float* getMatrix();

	/**
	 * Rotate quaternion around X axis by
	 * by specified angle.
	 * @param val - angle in degrees.
	 * @param global - boolean indicating
	 * whether to use global rotation,
	 * or object relative rotation.
	 */
	void addX(float val, bool global = false);

	/**
	 * Rotate quaternion around Y axis by
	 * by specified angle.
	 * @param val - angle in degrees.
	 * @param global - boolean indicating
	 * whether to use global rotation,
	 * or object relative rotation.
	 */
	void addY(float val, bool global = false);

	/**
	 * Rotate quaternion around Z axis by
	 * by specified angle.
	 * @param val - angle in degrees.
	 * @param global - boolean indicating
	 * whether to use global rotation,
	 * or object relative rotation.
	 */
	void addZ(float val, bool global = false);

	void setX(float val);
	void setY(float val);
	void setZ(float val);

	/**
	 * @return Rotation around X axis in degrees.
	 */
	float getX();

	/**
	 * @return Rotation around Y axis in degrees.
	 */
	float getY();

	/**
	 * @return Rotation around Z axis in degrees.
	 */
	float getZ();

	/**
	 * @return Array containing four quaternion
	 * attributes in order of x, y, z, w.
	 */
	float* toArray();

	/**
	 * @param space - space between message.
	 */
	void print(string space = "");
private:
	float data_[4];
	float matrix_[16];
};

#endif