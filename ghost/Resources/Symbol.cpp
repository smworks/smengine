/*
 * Symbol.cpp
 *
 *  Created on: 2013.12.19
 *      Author: Martynas Šustavičius
 */

#include "Symbol.h"
#include "Texture.h"
#include "../ResourceManager.h"

Symbol::Symbol(ServiceLocator* services) :
	Resource(services),
	texture_(0),
	width_(0),
	height_(0),
	advance_(0),
	fontSize_(10),
	offsetX_(0),
	offsetY_(0)
{
}

Symbol::~Symbol() {
	release();
}

bool Symbol::create() {
	return true;
}

void Symbol::release() {
	texture_ = 0;
}

SIZE Symbol::getSize() {
	return sizeof(Symbol);
}

Resource::Type Symbol::getType() {
	return Resource::SYMBOL;
}

void Symbol::resize() {

}

bool Symbol::isValid() {
	return texture_ != 0;
}

void Symbol::setTexture(Texture* texture) {
	texture_ = texture;
}

Texture* Symbol::getTexture() {
	return texture_;
}

void Symbol::setWidth(UINT32 width) {
	width_ = width;
}

UINT32 Symbol::getWidth() {
	return width_;
}

void Symbol::setHeight(UINT32 height) {
	height_ = height;
}

UINT32 Symbol::getHeight() {
	return height_;
}

void Symbol::setOffsetX(INT32 offset) {
	offsetX_ = offset;
}

INT32 Symbol::getOffsetX() {
	return offsetX_;
}

void Symbol::setOffsetY(INT32 offset) {
	offsetY_ = offset;
}

INT32 Symbol::getOffsetY() {
	return offsetY_;
}

void Symbol::setAdvance(SIZE advance) {
	advance_ = advance;
}

SIZE Symbol::getAdvance() {
	return advance_;
}

void Symbol::setFontSize(SIZE size) {
	fontSize_ = size;
}

SIZE Symbol::getFontSize() {
	return fontSize_;
}