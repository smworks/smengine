/*
 * AtlasTexture.cpp
 *
 *  Created on: 2012.07.24
 *      Author: Martynas Šustavičius
 */

#include "AtlasTexture.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Utils.h"
#include "../TextureAtlas.h"
#include "../Shapes.h"
#include "Vertex.h"

AtlasTexture::AtlasTexture(ServiceLocator* services, int type) :
	Texture(services),
	atlasId_(0), type_(type)
{}

AtlasTexture::~AtlasTexture() {
	release();
}

bool AtlasTexture::create() {
 	width_ = getAttributes().getInt(Texture::ATTR_WIDTH, 0);
	height_ = getAttributes().getInt(Texture::ATTR_HEIGHT, 0);
	return getServiceLocator()->getTextureAtlas()->create(
		atlasId_, width_, height_, type_);
}

UINT8* AtlasTexture::createBuffer(UINT32 width, UINT32 height) {
	return 0;
}

void AtlasTexture::release() {
	getServiceLocator()->getTextureAtlas()->clear(atlasId_);
    atlasId_ = 0;
}

SIZE AtlasTexture::getSize() {
	return sizeof(TextureAtlas);
}

Resource::Type AtlasTexture::getType() {
	return Resource::TEXTURE_2D;
}

void AtlasTexture::resize() {

}

bool AtlasTexture::isValid() {
	return true;
}

UINT32 AtlasTexture::getId() {
    return getServiceLocator()->getTextureAtlas()->getId(type_);
}

UINT32 AtlasTexture::getCBO() {
	return getServiceLocator()->getTextureAtlas()->
		getTextureHeader(atlasId_).cbo;
}

UINT32 AtlasTexture::getWidth() {
	return width_;
}

UINT32 AtlasTexture::getHeight() {
	return height_;
}

UINT8* AtlasTexture::getRawData() {
	return getServiceLocator()->getTextureAtlas()->getTexture(type_)->getPixel(0, 0);
}

void AtlasTexture::resize(UINT32 width, UINT32 height) {
    if (width == width_ && height == height_) {
		return;
	}
	width_ = width;
    height_ = height;
    getServiceLocator()->getTextureAtlas()->clear(atlasId_);
	getServiceLocator()->getTextureAtlas()->create(
		atlasId_, width_, height_, type_);
}

void AtlasTexture::setPixel(UINT8* color, SIZE row, SIZE column) {
	TextureAtlas::TextureHeader& header = getServiceLocator()->
		getTextureAtlas()->getTextureHeader(atlasId_);
	getServiceLocator()->getTextureAtlas()->getTexture(type_)->setPixel(
		color, header.offsetRow + row, header.offsetCol + column);
	return;
}

UINT8* AtlasTexture::getPixel(SIZE row, SIZE column) {
    TextureAtlas::TextureHeader& header = getServiceLocator()->
		getTextureAtlas()->getTextureHeader(atlasId_);
    return getServiceLocator()->getTextureAtlas()->getTexture(type_)->getPixel(
		header.offsetRow + row, header.offsetCol + column);
	return 0;
}

void AtlasTexture::blend(
	Texture* src, bool alpha, UINT32 rowOffset, UINT32 colOffset)
{
    TextureAtlas::TextureHeader& header = getServiceLocator()->
		getTextureAtlas()->getTextureHeader(atlasId_);
    getServiceLocator()->getTextureAtlas()->getTexture(type_)->blend(
		src, alpha, header.offsetRow + rowOffset,
		header.offsetCol + colOffset);
}

bool AtlasTexture::commit() {
	getServiceLocator()->getTextureAtlas()->getTexture(type_)->commit();
	return true;
}

float* AtlasTexture::getUV() {
	return getServiceLocator()->getTextureAtlas()->
		getTextureHeader(atlasId_).uv;
}