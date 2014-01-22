/*
 * Vec3.cpp
 *
 *  Created on: 2012.07.02
 *      Author: MS
 */

#include "Vec3.h"

Vec3::Vec3() {
	vec_[0] = vec_[1] = vec_[2] = 0.0f;
}

Vec3::Vec3(float val) {
	vec_[0] = vec_[1] = vec_[2] = val;
}

Vec3::Vec3(float x, float y, float z) {
	vec_[0] = x; vec_[1] = y; vec_[2] = z;
}

Vec3::Vec3(float vec[3]) {
	memcpy(vec_, vec, sizeof(float) * 3); 
}

Vec3::Vec3(const Vec3& vec) {
	memcpy(vec_, vec.vec_, sizeof(float) * 3);
}

Vec3 Vec3::operator+(const Vec3& vec) {
	Vec3 temp;
	temp.setXYZ(
		vec_[0] + vec.vec_[0],
		vec_[1] + vec.vec_[1],
		vec_[2] + vec.vec_[2]);
	return temp;
}

Vec3 Vec3::operator-(const Vec3& vec) {
	Vec3 temp;
	temp.setXYZ(
		vec_[0] - vec.vec_[0],
		vec_[1] - vec.vec_[1],
		vec_[2] - vec.vec_[2]);
	return temp;
}

Vec3 Vec3::operator*(const Vec3& vec) {
	Vec3 temp;
	temp.setXYZ(
		vec_[0] * vec.vec_[0],
		vec_[1] * vec.vec_[1],
		vec_[2] * vec.vec_[2]);
	return temp;
}

Vec3 Vec3::operator/(const Vec3& vec) {
	Vec3 temp;
	temp.setXYZ(
		vec_[0] / vec.vec_[0],
		vec_[1] / vec.vec_[1],
		vec_[2] / vec.vec_[2]);
	return temp;
}

Vec3& Vec3::operator+=(const Vec3& vec) {
	setXYZ(
		vec_[0] + vec.vec_[0],
		vec_[1] + vec.vec_[1],
		vec_[2] + vec.vec_[2]);
	return *this;
}

Vec3& Vec3::operator-=(const Vec3& vec) {
	setXYZ(
		vec_[0] - vec.vec_[0],
		vec_[1] - vec.vec_[1],
		vec_[2] - vec.vec_[2]);
	return *this;
}

Vec3& Vec3::operator*=(const Vec3& vec) {
	setXYZ(
		vec_[0] * vec.vec_[0],
		vec_[1] * vec.vec_[1],
		vec_[2] * vec.vec_[2]);
	return *this;
}

Vec3& Vec3::operator/=(const Vec3& vec) {
	setXYZ(
		vec_[0] / vec.vec_[0],
		vec_[1] / vec.vec_[1],
		vec_[2] / vec.vec_[2]);
	return *this;
}

Vec3 Vec3::operator+(float val) {
	Vec3 temp;
	temp.setXYZ(
		vec_[0] + val,
		vec_[1] + val,
		vec_[2] + val);
	return temp;
}

Vec3 Vec3::operator-(float val) {
	Vec3 temp;
	temp.setXYZ(
		vec_[0] - val,
		vec_[1] - val,
		vec_[2] - val);
	return temp;
}

Vec3 Vec3::operator*(float val) {
	Vec3 temp;
	temp.setXYZ(
		vec_[0] * val,
		vec_[1] * val,
		vec_[2] * val);
	return temp;
}

Vec3 Vec3::operator/(float val) {
	Vec3 temp;
	temp.setXYZ(
		vec_[0] / val,
		vec_[1] / val,
		vec_[2] / val);
	return temp;
}

Vec3& Vec3::operator+=(float val) {
	setXYZ(
		vec_[0] + val,
		vec_[1] + val,
		vec_[2] + val);
	return *this;
}

Vec3& Vec3::operator-=(float val) {
	setXYZ(
		vec_[0] - val,
		vec_[1] - val,
		vec_[2] - val);
	return *this;
}

Vec3& Vec3::operator*=(float val) {
	setXYZ(
		vec_[0] * val,
		vec_[1] * val,
		vec_[2] * val);
	return *this;
}

Vec3& Vec3::operator/=(float val) {
	setXYZ(
		vec_[0] / val,
		vec_[1] / val,
		vec_[2] / val);
	return *this;
}

void Vec3::crossProduct(const Vec3& vec) {
	setXYZ(
		vec_[1] * vec.vec_[2] - vec_[2] * vec.vec_[1],
		vec_[2] * vec.vec_[0] - vec_[0] * vec.vec_[2],
		vec_[0] * vec.vec_[1] - vec_[1] * vec.vec_[0]);
}

float Vec3::length() {
	return sqrt(
		vec_[0] * vec_[0] +
		vec_[1] * vec_[1] +
		vec_[2] * vec_[2]);
}

void Vec3::normalize() {
	float length = sqrt(
		vec_[0] * vec_[0] +
		vec_[1] * vec_[1] +
		vec_[2] * vec_[2]);
	vec_[0] /= length;
	vec_[1] /= length;
	vec_[2] /= length;
}

void Vec3::invert() {
	vec_[0] *= -1;
	vec_[1] *= -1;
	vec_[2] *= -1;
}

void Vec3::mix(const Vec3& vec, float ratio) {
	if (ratio < 0.0f) {
		ratio = 0.0f;
	}
	if (ratio > 1.0f) {
		ratio = 1.0f;
	}
	setXYZ(
		vec_[0] * (1.0f - ratio) + vec.vec_[0] * ratio,
		vec_[1] * (1.0f - ratio) + vec.vec_[1] * ratio,
		vec_[2] * (1.0f - ratio) + vec.vec_[2] * ratio);
}

void Vec3::addX(float value) {
	vec_[0] += value;
}

void Vec3::addY(float value) {
	vec_[1] += value;
}

void Vec3::addZ(float value) {
	vec_[2] += value;
}

void Vec3::addXYZ(float x, float y, float z) {
	vec_[0] += x;
	vec_[1] += y;
	vec_[2] += z;
}

void Vec3::addXYZ(const Vec3& vec) {
	vec_[0] += vec.vec_[0];
	vec_[1] += vec.vec_[1];
	vec_[2] += vec.vec_[2];
}

void Vec3::setX(float value) {
	vec_[0] = value;
}

void Vec3::setY(float value) {
	vec_[1] = value;
}

void Vec3::setZ(float value) {
	vec_[2] = value;
}

void Vec3::setXYZ(float x, float y, float z) {
	vec_[0] = x;
	vec_[1] = y;
	vec_[2] = z;
}

void Vec3::setXYZ(const Vec3& vec) {
	vec_[0] = vec.vec_[0];
	vec_[1] = vec.vec_[1];
	vec_[2] = vec.vec_[2];
}

float Vec3::getX() {
	return vec_[0];
}

float Vec3::getY() {
	return vec_[1];
}

float Vec3::getZ() {
	return vec_[2];
}

float* Vec3::toArray() {
	return vec_;
}

void Vec3::getXYZ(float vec[3]) {
	vec[0] = vec_[0];
	vec[1] = vec_[1];
	vec[2] = vec_[2];
}

float* Vec3::getPointer() {
	return vec_;
}

float Vec3::getMinVal() {
	return vec_[0] < vec_[1] ?
		(vec_[0] < vec_[2] ? vec_[0] : vec_[2]) :
		(vec_[1] < vec_[2] ? vec_[1] : vec_[2]);
}

float Vec3::getMaxVal() {
	return vec_[0] > vec_[1] ?
		(vec_[0] > vec_[2] ? vec_[0] : vec_[2]) :
		(vec_[1] > vec_[2] ? vec_[1] : vec_[2]);
}

void Vec3::print(string space) {
	LOGD("%sx=%f, y=%f, z=%f",
		space.c_str(), vec_[0], vec_[1], vec_[2]);
}