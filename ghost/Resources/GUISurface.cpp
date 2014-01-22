/*
 * GUISurface.cpp
 *
 *  Created on: 2013.07.03
 *      Author: Martynas Šustavičius
 */

#include "../Multiplatform/ServiceLocator.h"
#include "../Multiplatform/GraphicsManager.h"
#include "GUISurface.h"
#include "Vertex.h"
#include "AtlasTexture.h"
#include "TextureRGBA.h"
#include "Shader.h"
#include "../Shapes.h"
#include "../ResourceManager.h"
#include "../Node.h"

GUISurface::GUISurface(ServiceLocator* services) :
	Resource(services),
	surfaceId_(0),
	textureName_(""),
	widthSize_(DEFAULT),
	heightSize_(DEFAULT),
	marginLeft_(0),
	marginBottom_(0),
	marginRight_(0),
	marginTop_(0),
	posX_(0),
	posY_(0),
	update_(true),
	textureBackground_(0),
	width_(0),
	height_(0)
{}

GUISurface::~GUISurface() {
	release();
}

void GUISurface::release() {
	textureBackground_ = 0;
	update_ = true;
}

SIZE GUISurface::getSize() {
	return sizeof(GUISurface);
}

bool GUISurface::isValid() {
	return true;
}

void GUISurface::resize() {
	update();
}

SIZE GUISurface::getVertexCount() {
	return g_planeVerticeCount / 3;
}


SIZE GUISurface::getNormalCount() {
	return 0;
}

SIZE GUISurface::getTexture() {
	return textureBackground_ == 0 ? 0 : textureBackground_->getId();
}

SIZE GUISurface::getIBO() {
	return 0;
}

BoundingVolume* GUISurface::getBV() {
	return 0;
}

SIZE GUISurface::getCBO() {
	return textureBackground_ == 0 ? 0 : textureBackground_->getId();
}

SIZE GUISurface::getIndexCount() {
	return 0;
}

SIZE GUISurface::getRenderCount() {
	return 1;
}

void GUISurface::setRenderable(SIZE i) {
	
}

SIZE GUISurface::getVertexStride() {
	return sizeof(VertexPT);
}

int GUISurface::getPosOffset() {
	return offsetof(VertexPT, pos);
}

int GUISurface::getNormalOffset() {
	return -1;
}

int GUISurface::getUVOffset() {
	return offsetof(VertexPT, uv);
}

Shader* GUISurface::getDefaultShader() {
	return 0;
}

Texture* GUISurface::getPointerToTexture() {
	return textureBackground_;
}

void GUISurface::setMargins(int left, int bottom, int right, int top) {
	marginLeft_ = left;
	marginBottom_ = bottom;
	marginRight_ = right;
	marginTop_ = top;
	setPos(left, bottom);
}

int GUISurface::getMarginLeft() {
	return marginLeft_;
}

int GUISurface::getMarginBottom() {
	return marginBottom_;
}

int GUISurface::getMarginRight() {
	return marginRight_;
}

int GUISurface::getMarginTop() {
	return marginTop_;
}

void GUISurface::setPos(int x, int y) {
	posX_ = x;
	posY_ = y;
	update_ = true;
}

void GUISurface::setBackground(string background) {
	if (background.length() == 0 || background[0] == '#') {
        if (background.length() == 0) {
            getAmbient().setRGBA(1.0f);
        }
        else {
			getAmbient().setRGBA(background);
        }
		Shader* shader = static_cast<Shader*>(
			getServiceLocator()->getRM()->get(SHADER, SURFACE_BCKG_COLOR));
		if (shader == 0) {
			shader = NEW Shader(getServiceLocator());
			shader->getAttributes().setString(
				ATTR_FILE, SURFACE_BCKG_COLOR);
			shader->create();
			getServiceLocator()->getRM()->add(SURFACE_BCKG_COLOR, shader);
		}
		setShader(shader);
    }
    else {
        textureBackground_ = static_cast<TextureRGBA*>(
			getServiceLocator()->getRM()->get(
				TEXTURE_2D, background));
		if (textureBackground_ == 0) {
			textureBackground_ = NEW TextureRGBA(getServiceLocator());
			textureBackground_->getAttributes().setString(
				ATTR_FILE, background);
			textureBackground_->create();
			getServiceLocator()->getRM()->add(background,
				textureBackground_);
		}
		Shader* shader = static_cast<Shader*>(
			getServiceLocator()->getRM()->get(SHADER, SURFACE_BCKG_IMAGE));
		if (shader == 0) {
			shader = NEW Shader(getServiceLocator());
			shader->getAttributes().setString(
				ATTR_FILE, SURFACE_BCKG_IMAGE);
			shader->create();
			getServiceLocator()->getRM()->add(SURFACE_BCKG_IMAGE, shader);
		}
		setShader(shader);
    }
	update_ = true;
}

void GUISurface::setWidth(SIZE width) {
	width_ = width;
	update_ = true;
}

SIZE GUISurface::getWidth() {
    return width_;
}

void GUISurface::setHeight(SIZE height) {
	height_ = height;
	update_ = true;
}

SIZE GUISurface::getHeight() {
    return height_;
}

UINT32 GUISurface::convertUnitsToPixels(UINT32 units, UnitType type) {
	switch (type) {
	case DEFAULT:
		return units;
	default:
		return units;
	}
}

int GUISurface::getSize(string size, int maxSize) {
	if (VAL_FILL == size) {
		return maxSize;
	}
	int rawSize = toInt(size.c_str());
	if (string::npos != size.find("%")) {
		return (int) ((float) maxSize / 100.0f * rawSize);
	}
	return rawSize;
}

bool GUISurface::create() {
    marginLeft_ = getSize(getAttribute(ATTR_MARGIN_LEFT),
		getServiceLocator()->getScreenWidth());
	marginBottom_ = getSize(getAttribute(ATTR_MARGIN_BOTTOM),
		getServiceLocator()->getScreenHeight());
    marginRight_ = getSize(getAttribute(ATTR_MARGIN_RIGHT),
		getServiceLocator()->getScreenWidth());
	marginTop_ = getSize(getAttribute(ATTR_MARGIN_TOP),
		getServiceLocator()->getScreenHeight());
	if (getAttribute(ATTR_TRANSPARENCY).length() > 0) {
		setTransparency(toFloat(getAttribute(ATTR_TRANSPARENCY).c_str()));
	}
	posX_ = marginLeft_;
	posY_ = marginBottom_;
	//string attrWidth = getAttribute(ATTR_WIDTH);
	//width_ = getSize(
	//	attrWidth,	getServiceLocator()->getScreenWidth());
	//if (VAL_FILL == attrWidth) {
	//	widthSize_ = FILL;
	//}
	//string attrHeight = getAttribute(ATTR_HEIGHT);
	//height_ = getSize(
	//	attrHeight, getServiceLocator()->getScreenHeight());
	//if (VAL_FILL == attrHeight) {
	//	heightSize_ = FILL;
	//}
   // if (width_ == 0 ||  height_ == 0) {
   //     LOGW("Invalid width or height specified for GUI element: %s. "
			//"Width = %d, height = %d.", getName().c_str(), width_, height_);
   // }
	setBackground(getAttribute(ATTR_BACKGROUND));
	if (checkGLError("Creating view resource.")) {
		release();
		return false;
	}
	return true;
}