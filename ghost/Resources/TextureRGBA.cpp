/*
 * TextureRGBA.cpp
 *
 *  Created on: 2012.06.30
 *      Author: Martynas Šustavičius
 */

#include "TextureRGBA.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Multiplatform/GraphicsManager.h"
#include "../Multiplatform/FileManager.h"
#include "../Utils.h"
#include "../TextureAtlas.h"
#include "../Shapes.h"
#include "Vertex.h"

const string TextureRGBA::ATTR_WRAP_U = "wrap_u";
const string TextureRGBA::ATTR_WRAP_V = "wrap_v";
const string TextureRGBA::VAL_CLAMP = "clamp";
const string TextureRGBA::VAL_REPEAT = "repeat";

TextureRGBA::TextureRGBA(ServiceLocator* services) :
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
	type_(RGBA)
{}

TextureRGBA::~TextureRGBA() {
	release();
}

bool TextureRGBA::create() {
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
	LOGD("Loaded texture: %s.", getAttribute(ATTR_FILE).c_str());
	return true;
}


bool TextureRGBA::create(UINT32 width, UINT32 height) {
    if (getServiceLocator() == 0) {
		LOGE("Service locator not specified for TextureRGBA object.");
		return false;
	}
	width_ = width;
	height_ = height;
	UINT32 size = width_ * height_ * 4;
	buffer_ = NEW UINT8[size];
	for (UINT32 i = 0; i < size; i += 4) {
		buffer_[i + 0] = 0;
		buffer_[i + 1] = 0;
		buffer_[i + 2] = 255;
		buffer_[i + 3] = 255;
	}
    vector<VertexPT>* cbo = static_cast<vector<VertexPT>*>(
        Shapes::getShape(
		Shapes::SHAPE_SCREEN_PLANE, Shapes::VERTEX_POS_TEX));
    getServiceLocator()->getGraphicsManager()->setVertexBuffer(
        cbo_, &(*cbo)[0], (UINT32) cbo->size() * sizeof(VertexPT));
    delete cbo;
    return true;
}

void TextureRGBA::release() {
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

SIZE TextureRGBA::getSize() {
	return sizeof(TextureRGBA);
}

Resource::Type TextureRGBA::getType() {
	return Resource::TEXTURE_2D;
}

void TextureRGBA::resize() {

}

bool TextureRGBA::isValid() {
	return getId() > 0;
}

UINT32 TextureRGBA::getId() {
	if (left_ != width_) {
		commit();
	}
	return id_;
}

UINT32 TextureRGBA::getCBO() {
    return cbo_;
}

UINT32 TextureRGBA::getWidth() {
	return width_;
}

UINT32 TextureRGBA::getHeight() {
	return height_;
}

UINT8* TextureRGBA::getRawData() {
	return buffer_;
}

void TextureRGBA::resize(UINT32 width, UINT32 height) {
    INT32 horizontalOffset = 0;
    INT32 verticalOffset = 0;
	// Use something better than nearest neighbour interpolation.
    horizontalOffset = 1;
    verticalOffset = 1;
    float widthRatio = (float) width_ / width;
    float heightRatio = (float) height_ / height;
    UINT8* buffer = NEW UINT8[width * height * 4];
    for (UINT32 i = 0; i < height; i++) {
        for (UINT32 j = 0; j < width; j++) {
            INT32 r = 0, g = 0, b = 0, a = 0, pixels = 0;
            for (INT32 k = -verticalOffset; k < 1 + verticalOffset; k++) {
                for (INT32 l = -horizontalOffset; l < 1 + horizontalOffset; l++) {
					INT32 row = INT32(i * heightRatio) + k, col = INT32(j * widthRatio) + l;
					if (row < 0 || col < 0 || col >= (INT32) width_ || row >= (INT32) height_) {
						continue;
					}
					pixels++;
                    UINT8* px = getPixel(row, col);
                    r += px[0];
                    g += px[1];
                    b += px[2];
                    a += px[3];
                }
            }
            INT32 pos = i * width * 4 + j * 4;
            buffer[pos + 0] = r / pixels;
            buffer[pos + 1] = g / pixels;
            buffer[pos + 2] = b / pixels;
            buffer[pos + 3] = a / pixels;
        }
    }
    delete [] buffer_;
    buffer_ = buffer;
    width_ = width;
    height_ = height;
}

void TextureRGBA::setPixel(UINT8* color, SIZE row, SIZE column) {
	if (buffer_ == 0 || row >= height_ || column >= width_) {
		LOGW("Unable to set pixel on row %d, column %d.", row, column);
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
	SIZE pos = row * width_ * 4 + column * 4;
	buffer_[pos + 0] = color[0];
	buffer_[pos + 1] = color[1];
	buffer_[pos + 2] = color[2];
	buffer_[pos + 3] = color[3];
}

UINT8* TextureRGBA::getPixel(SIZE row, SIZE column) {
	if (buffer_ == 0 || row >= height_ || column >= width_) {
		LOGW("Unable to get pixel on row %d, column %d.", row, column);
		static UINT8 empty[] = {0, 0, 0, 0};
		return empty;
	}
	int pos = row * width_ * 4 + column * 4;
	return buffer_ + pos;
}

void TextureRGBA::blend(
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

bool TextureRGBA::commit() {
	if (buffer_ == 0) {
		LOGE("TextureRGBA contains no pixel data. Could not compile.");
		return false;
	}
	GraphicsManager* gm = getServiceLocator()->getGraphicsManager();
	if (!isPowerOfTwo(width_) && !gm->isSupported(GraphicsManager::SUPPORT_NPOT_TEXTURES)) {
		UINT32 newWidth = toPowerOfTwo(width_);
		float ratio = (float) newWidth / (float) width_;
		LOGW("TextureRGBA width (%d) is not a power of two.", width_);
		LOGW("Hardware doesn't support NPOT textures.");
		LOGW("Texture will be scaled to next power of two resolution: %ux%u.",
			newWidth, (UINT32) (height_ * ratio));
		resize(newWidth, (UINT32) (height_ * ratio));
	}
	SIZE maxSize = gm->getMax(GraphicsManager::MAX_TEXTURE_SIZE);
	if (width_ > maxSize || height_ > maxSize) {
		LOGW("Trying to load texture whose width or height is too large.");
		LOGW("Current size: %ux%upx. Maximum supported size: %u.",
			width_, height_, gm->getMax(GraphicsManager::MAX_TEXTURE_SIZE));
		SIZE width, height;
		if (width_ > maxSize) {
			width = maxSize;
			height = (UINT32) (height_ * ((float) maxSize / width_));
		}
		if (height_ > maxSize) {
			height = maxSize;
			width = (UINT32) (width_ * ((float) maxSize / height_));
		}
		LOGW("Texture will be resized to: %ux%upx.", width, height);
		resize(width, height);
	}
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
			true, RGBA);
	}
	UINT8* buffer = NEW UINT8[updateRegionHeight * updateRegionWidth * 4];
	for (UINT32 row = 0; row < updateRegionHeight; row++) {
		memcpy(buffer + row * updateRegionWidth * sizeof(UINT8) * 4,
			buffer_ + (row + top_) * width_ * sizeof(UINT8) * 4
				+ left_ * sizeof(UINT8) * 4,
			updateRegionWidth * sizeof(UINT8) * 4);
	}
	gm->updateTexture(id_, buffer, top_, left_, updateRegionWidth, updateRegionHeight, true, RGBA);
	left_ = width_;
	top_ = height_;
	right_ = bottom_ = 0;
	delete [] buffer;
	return true;
}