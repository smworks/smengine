/*
 * Color.cpp
 *
 *  Created on: 2012.10.25
 *      Author: MS
 */

#include "Color.h"

Color::Color() {
	fill(color_, color_ + 4, 1.0f);
}

Color::Color(const string& color) {
	setRGBA(color);
}

Color::Color(float value) {
	CLAMP(value)
	fill(color_, color_ + 4, value);
}

Color::Color(float r, float g, float b) {
	CLAMP(r); CLAMP(g); CLAMP(b);
	color_[0] = r; color_[1] = g;
	color_[2] = b; color_[3] = 1.0f;
}

Color::Color(float r, float g, float b, float a) {
	CLAMP(r); CLAMP(g); CLAMP(b); CLAMP(a);
	color_[0] = r; color_[1] = g;
	color_[2] = b; color_[3] = a;
}

Color::~Color() {
}

void Color::setR(float val) {
	CLAMP(val);
	color_[0] = val;
}

void Color::setG(float val) {
	CLAMP(val);
	color_[1] = val;
}

void Color::setB(float val) {
	CLAMP(val);
	color_[2] = val;
}

void Color::setA(float val) {
	CLAMP(val);
	color_[3] = val;
}

void Color::setRGB(float r, float g, float b) {
	CLAMP(r); CLAMP(g); CLAMP(b);
	color_[0] = r; color_[1] = g;
	color_[2] = b; color_[3] = 1.0f;
}

void Color::setRGBA(const string& color) {
	if (color.length() != 7 && color.length() != 9) {
		Color();
		return;
	}
	static stringstream ss;
	UINT32 val;
	UINT32 componentCount = color.length() == 7 ? 3 : 4;
	for (UINT32 i = 0; i < componentCount; i++) {
		ss << std::hex << color.substr(i * 2 + 1, 2);
		ss >> val;
		color_[i] = val / 255.0f;
		ss.str("");
		ss.clear();
	}
}

void Color::setRGBA(
	float r, float g, float b, float a)
{
	CLAMP(r); CLAMP(g); CLAMP(b); CLAMP(a);
	color_[0] = r; color_[1] = g;
	color_[2] = b; color_[3] = a;
}

void Color::setRGBA(const Color& col) {
	memcpy(color_, col.color_, sizeof(float) * 4);
}

float Color::getR() {
	return color_[0];
}

float Color::getG() {
	return color_[1];
}

float Color::getB() {
	return color_[2];
}

float Color::getA() {
	return color_[3];
}

float* Color::toArray() {
	return color_;
}

void Color::print(std::string space) {
	LOGD("%sR=%f, G=%f, B=%f, A=%f.",
		space.c_str(),
		color_[0],
		color_[1],
		color_[2],
		color_[3]);
}
