/*
 * Quaternion.cpp
 *
 *  Created on: 2013.03.08
 *      Author: Martynas Šustavičius
 */

#include "Quaternion.h"

Quaternion::Quaternion() {
	setXYZ(0.0f, 0.0f, 0.0f);
}

Quaternion::Quaternion(float x, float y, float z) {
	setXYZ(x, y, z);
}

Quaternion::Quaternion(float x, float y, float z, float w) {
	data_[0] = x;
	data_[1] = y;
	data_[2] = z;
	data_[3] = w;
}

/**
 * Multiplying q1 with q2 applies the rotation q2 to q1.
 */
Quaternion Quaternion::operator* (const Quaternion& q) {
	return Quaternion(
		data_[3] * q.data_[0] + data_[0] * q.data_[3]
			+ data_[1] * q.data_[2] - data_[2] * q.data_[1],
		data_[3] * q.data_[1] + data_[1] * q.data_[3]
			+ data_[2] * q.data_[0] - data_[0] * q.data_[2],
		data_[3] * q.data_[2] + data_[2] * q.data_[3]
			+ data_[0] * q.data_[1] - data_[1] * q.data_[0],
		data_[3] * q.data_[3] - data_[0] * q.data_[0]
			- data_[1] * q.data_[1] - data_[2] * q.data_[2]);
}

Quaternion Quaternion::operator* (float scale) {
	return Quaternion(
		data_[0] * scale, data_[1] * scale,
		data_[2] * scale, data_[3] * scale);
}

void Quaternion::invert() {
	data_[0] *= -1.0f;
	data_[1] *= -1.0f;
	data_[2] *= -1.0f;
	float length = getLength();
	float squaredLength = length * length;
	data_[0] /= squaredLength;
	data_[1] /= squaredLength;
	data_[2] /= squaredLength;
	data_[3] /= squaredLength;
}

float Quaternion::getLength() {
	return data_[3] * data_[3]
		+ data_[0] * data_[0]
		+ data_[1] * data_[1]
		+ data_[2] * data_[2];
}

void Quaternion::normalise() {
	float mag2 = getLength();
	// Don't normalize if we don't have to.
	if (fabs(mag2) > GHOST_DELTA && fabs(mag2 - 1.0f) > GHOST_DELTA) {
		float mag = sqrt(mag2);
		data_[0] /= mag;
		data_[1] /= mag;
		data_[2] /= mag;
		data_[3] /= mag;
	}
}

void Quaternion::setXYZ(float x, float y, float z) {
	float yaw = y, pitch = x;
	y = pitch * 0.0174532925f;
	x = yaw * 0.0174532925f;
	z *= 0.0174532925f;
    float rollOver2 = z * 0.5f;
    float sinRollOver2 = (float)sin((double)rollOver2);
    float cosRollOver2 = (float)cos((double)rollOver2);
    float pitchOver2 = x * 0.5f;
    float sinPitchOver2 = (float)sin((double)pitchOver2);
    float cosPitchOver2 = (float)cos((double)pitchOver2);
    float yawOver2 = y * 0.5f;
    float sinYawOver2 = (float)sin((double)yawOver2);
    float cosYawOver2 = (float)cos((double)yawOver2);
    data_[0] = sinYawOver2 * cosPitchOver2 * cosRollOver2 - cosYawOver2 * sinPitchOver2 * sinRollOver2;
    data_[1] = cosYawOver2 * sinPitchOver2 * cosRollOver2 + sinYawOver2 * cosPitchOver2 * sinRollOver2;
    data_[2] = cosYawOver2 * cosPitchOver2 * sinRollOver2 - sinYawOver2 * sinPitchOver2 * cosRollOver2;
	data_[3] = cosYawOver2 * cosPitchOver2 * cosRollOver2 + sinYawOver2 * sinPitchOver2 * sinRollOver2;
}

void Quaternion::setXYZW(float x, float y, float z, float w) {
	data_[0] = x;
	data_[1] = y;
	data_[2] = z;
	data_[3] = w;
}

float* Quaternion::getMatrix() {
	float x = -data_[0];
	float y = -data_[1];
	float z = -data_[2];
	float x2 = x * x;
	float y2 = y * y;
	float z2 = z * z;
	float xy = x * y;
	float xz = x * z;
	float yz = y * z;
	float wx = data_[3] * x;
	float wy = data_[3] * y;
	float wz = data_[3] * z;
	// This calculation would be a lot more
	// complicated for non-unit length quaternions
	// Note: The constructor of Matrix4 expects
	// the Matrix in column-major format like expected by OpenGL.
	matrix_[0] = 1.0f - 2.0f * (y2 + z2);
	matrix_[1] = 2.0f * (xy - wz);
	matrix_[2] = 2.0f * (xz + wy);
	matrix_[3] = 0.0f;
	matrix_[4] = 2.0f * (xy + wz);
	matrix_[5] = 1.0f - 2.0f * (x2 + z2);
	matrix_[6] = 2.0f * (yz - wx);
	matrix_[7] = 0.0f;
	matrix_[8] = 2.0f * (xz - wy);
	matrix_[9] = 2.0f * (yz + wx);
	matrix_[10] = 1.0f - 2.0f * (x2 + y2);
	matrix_[11] = 0.0f;
	matrix_[12] = 0.0f;
	matrix_[13] = 0.0f;
	matrix_[14] = 0.0f;
	matrix_[15] = 1.0f;
	return matrix_;
}

void Quaternion::addX(float val, bool global) {
	Quaternion q1(data_[0], data_[1], data_[2], data_[3]);
	Quaternion q2(val, 0.0f, 0.0f);
	Quaternion res = global == true ? q2 * q1 : q1 * q2;
	memcpy(data_, res.data_, sizeof(data_));
}

void Quaternion::addY(float val, bool global) {
	Quaternion q1(data_[0], data_[1], data_[2], data_[3]);
	Quaternion q2(0.0f, val, 0.0f);
	Quaternion res = global == true ? q2 * q1 : q1 * q2;
	memcpy(data_, res.data_, sizeof(data_));
}

void Quaternion::addZ(float val, bool global) {
	Quaternion q1(data_[0], data_[1], data_[2], data_[3]);
	Quaternion q2(0.0f, 0.0f, val);
	Quaternion res = global == true ? q2 * q1 : q1 * q2;
	memcpy(data_, res.data_, sizeof(data_));
}

void Quaternion::setX(float val) {
	setXYZ(val, getY(), getZ());
}

void Quaternion::setY(float val) {
	setXYZ(getX(), val, getZ());
}

void Quaternion::setZ(float val) {
	setXYZ(getX(), getY(), val);
}

float Quaternion::getX() {
	return (float) atan2(
		2.0f *  data_[0] * data_[3]
		+ 2.0f * data_[1] * data_[2], 1.0f - 2.0f
		* (data_[2] * data_[2]  +  data_[0] *  data_[0])) * 57.2957795f;
}

float Quaternion::getY() {
	return (float) atan2(
		2.0f * data_[3] * data_[1]
		+ 2.0f * data_[2] * data_[0], 1.0f - 2.0f
		* (data_[1] * data_[1]  + data_[2] * data_[2])) * 57.2957795f;
}

float Quaternion::getZ() {
	return (float) asin(
		2.0f *(data_[3] * data_[2] - data_[0] * data_[1])) * 57.2957795f; 
}

float* Quaternion::toArray() {
	return data_;
}

void Quaternion::print(string space) {
	LOGD("%sx=%f, y=%f, z=%f, w=%f",
		space.c_str(), data_[0], data_[1], data_[2], data_[3]);
}