/*
 * TextureMono.cpp
 *
 *  Created on: 2012.06.30
 *      Author: Martynas Šustavičius
 */

#include "TextureMono.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Multiplatform/GraphicsManager.h"
#include "../Multiplatform/FileManager.h"
#include "../Utils.h"
#include "../TextureAtlas.h"
#include "../Shapes.h"
#include "Vertex.h"

TextureMono::TextureMono(ServiceLocator* services) :
	Texture(services),
	left_(0),
	top_(0),
	right_(0),
	bottom_(0),
	width_(0),
	height_(0),
	id_(0),
	buffer_(0),
	cbo_(0),
	type_(MONO)
{}

TextureMono::~TextureMono() {
	release();
}

bool TextureMono::create() {
	if (getAttribute(ATTR_FILE).length() == 0) {
		LOGW("No file name specified for texture.");
		return false;
	}
	buffer_ = reinterpret_cast<UINT8*>(loadPng(getServiceLocator(),
		(GHOST_SPRITES + getAttribute(ATTR_FILE)).c_str(), width_, height_));
	if (buffer_ == 0) {
		LOGW("Unable to load file \"%s\".", getAttribute(ATTR_FILE).c_str());
		return false;
	}
    vector<VertexPT>* cbo = static_cast<vector<VertexPT>*>(
        Shapes::getShape(Shapes::SHAPE_SCREEN_PLANE,
			Shapes::VERTEX_POS_TEX));
    getServiceLocator()->getGraphicsManager()->setVertexBuffer(
        cbo_, &(*cbo)[0], (UINT32) cbo->size() * sizeof(VertexPT));
    delete cbo;
	return true;
}


bool TextureMono::create(UINT32 width, UINT32 height) {
    if (getServiceLocator() == 0) {
		LOGE("Service locator not specified for TextureMono object.");
		return false;
	}
	width_ = width;
	height_ = height;
	UINT32 size = width_ * height_;
	buffer_ = NEW UINT8[size];
	for (UINT32 i = 0; i < size; i ++) {
		buffer_[i + 0] = 255;
	}
    vector<VertexPT>* cbo = static_cast<vector<VertexPT>*>(
        Shapes::getShape(
		Shapes::SHAPE_SCREEN_PLANE, Shapes::VERTEX_POS_TEX));
    getServiceLocator()->getGraphicsManager()->setVertexBuffer(
        cbo_, &(*cbo)[0], (UINT32) cbo->size() * sizeof(VertexPT));
    delete cbo;
    return true;
}

void TextureMono::release() {
    if (cbo_ != 0) {
        getServiceLocator()->
			getGraphicsManager()->unsetVertexBuffer(cbo_);
    }
	if (buffer_ != 0) {
		delete [] buffer_;
		buffer_ = 0;
	}
	if (id_ != 0) {
		getServiceLocator()->
			getGraphicsManager()->unsetTexture(id_);
		id_ = 0;
	}
}

SIZE TextureMono::getSize() {
	return sizeof(TextureMono);
}

Resource::Type TextureMono::getType() {
	return Resource::TEXTURE_2D;
}

void TextureMono::resize() {

}

bool TextureMono::isValid() {
	return id_ > 0;
}

UINT32 TextureMono::getId() {
	if (left_ != width_) {
		commit();
	}
	return id_;
}

UINT32 TextureMono::getCBO() {
    return cbo_;
}

UINT32 TextureMono::getWidth() {
	return width_;
}

UINT32 TextureMono::getHeight() {
	return height_;
}

UINT8* TextureMono::getRawData() {
	return buffer_;
}

void TextureMono::resize(UINT32 width, UINT32 height) {
    int pixels = 1;
    int horizontalOffset = 0;
    int verticalOffset = 0;

//    if (rand() % 2 == 0) { // Use something better than nearest neighbour interpolation.
//        pixels = 9;
//        horizontalOffset = 1;
//        verticalOffset = 1;
//    }

    float widthRatio = (float) width_ / width;
    float heightRatio = (float) height_ / height;
    UINT8* buffer = NEW UINT8[width * height];

    for (UINT32 i = 0; i < height; i++) {
        for (UINT32 j = 0; j < width; j++) {
            int a = 0;
            for (int k = -verticalOffset; k < 1 + verticalOffset; k++) {
                for (int l = -horizontalOffset; l < 1 + horizontalOffset;
					l++)
				{
                    UINT8* px = getPixel(
						int(i * heightRatio) + k, int(j * widthRatio) + l);
                    a += px[0];
                }
            }
            int pos = i * width + j;
            buffer[pos] = a / pixels;
        }
    }
    delete [] buffer_;
    buffer_ = buffer;
    width_ = width;
    height_ = height;
}

void TextureMono::setPixel(UINT8* color, SIZE row, SIZE column) {
	if (buffer_ == 0 || row >= height_
		|| column >= width_) {
		LOGW("Unable to set pixel.");
		return;
	}
	if (column < left_) {
		left_ = column;
	}
	if (column > right_) {
		right_ = column;
	}
	if (row < top_) {
		top_ = row;
	}
	if (row > bottom_) {
		bottom_ = row;
	}
	SIZE pos = row * width_ + column;
	buffer_[pos] = color[0];
}

UINT8* TextureMono::getPixel(SIZE row, SIZE column) {
	if (buffer_ == 0 || row >= height_
		|| column >= width_) {
		LOGW("Unable to get pixel.");
		static UINT8 empty[] = {0};
		return empty;
	}
	SIZE pos = row * width_ + column;
	return buffer_ + pos;
}

void TextureMono::blend(
	Texture* src, bool alpha, UINT32 rowOffset, UINT32 colOffset)
{
    if (src == 0 || rowOffset > getHeight() || colOffset > getWidth()) {
        LOGW("Unable to blend. Specified source texture is null "
             "or offsets are bigger than original image.");
    }
    UINT32 rowBound = rowOffset + src->getHeight();
    if (rowBound > getHeight()) {
        rowBound = getHeight();
    }
    UINT32 colBound = colOffset + src->getWidth();
    if (colBound > getWidth()) {
        colBound = getWidth();
    }
    if (alpha) {
        for (UINT32 i = rowOffset; i < rowBound; i++) {
            for (UINT32 j = colOffset; j < colBound; j++) {
                UINT8* spx = src->getPixel(i - rowOffset, j - colOffset);
                UINT8* opx = getPixel(i, j);
                UINT8 r1 = opx[0], r2 = spx[0],
                    g1 = opx[1], g2 = spx[1],
                    b1 = opx[2], b2 = spx[2];
                UINT8 av = spx[3]; // Alpha value.
                int alpha = 0x10000 * av / 255;
                int alphaInverse = 0x10000 - alpha; // Remaining fraction
                opx[0] = (r1 * alphaInverse + r2 * alpha) >> 16;
                opx[1] = (g1 * alphaInverse + g2 * alpha) >> 16;
                opx[2] = (b1 * alphaInverse + b2 * alpha) >> 16;
            }
        }
    }
    else {
        for (UINT32 i = rowOffset; i < rowBound; i++) {
            for (UINT32 j = colOffset; j < colBound; j++) {
                setPixel(src->getPixel(i - rowOffset, j - colOffset), i, j);
            }
        }
    }
}

bool TextureMono::commit() {
	if (buffer_ == 0) {
		LOGE("Monochrome texture contains no pixel data. Could not compile.");
		return false;
	}
	GraphicsManager* gm = getServiceLocator()->getGraphicsManager();
	if (left_ == width_) {
		LOGD("Commiting with zero changes.");
		return false;
	}
	SIZE updateRegionWidth = right_ - left_ + 1;
	SIZE updateRegionHeight = bottom_ - top_ + 1;
	if (id_ == 0 || (updateRegionWidth == width_ && updateRegionHeight == height_)) {
		if (id_ != 0) {
			gm->unsetTexture(id_);
			id_ = 0;
		}
		left_ = width_;
		top_ = height_;
		right_ = bottom_ = 0;
		return gm->setTexture(
			id_, buffer_, width_, height_,
			getAttribute(ATTR_WRAP_U) == VAL_REPEAT ? true : false,
			getAttribute(ATTR_WRAP_V) == VAL_REPEAT ? true : false,
			true, MONO);
	}
	UINT8* buffer = NEW UINT8[updateRegionHeight * updateRegionWidth];
	for (UINT32 row = 0; row < updateRegionHeight; row++) {
		memcpy(buffer + row * updateRegionWidth * sizeof(UINT8),
			buffer_ + (row + top_) * width_ * sizeof(UINT8)
				+ left_ * sizeof(UINT8),
			updateRegionWidth * sizeof(UINT8));
	}
	gm->updateTexture(
		id_, buffer, top_, left_, updateRegionWidth, updateRegionHeight, true, MONO);
	left_ = width_;
	top_ = height_;
	right_ = bottom_ = 0;
	delete [] buffer;
	return true;
}