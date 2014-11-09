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
	widthSize_(DEFAULT),
	heightSize_(DEFAULT),
	marginLeft_(0.0f),
	marginBottom_(0.0f),
	marginRight_(0.0f),
	marginTop_(0.0f),
	posX_(0.0f),
	posY_(0.0f),
	width_(0.0f),
	height_(0.0f),
	update_(true),
	textureBackground_(0),
	cbo_(0)
{}

GUISurface::~GUISurface() {
	release();
}

void GUISurface::release() {
	textureBackground_ = 0;
	update_ = true;
	if (cbo_ != 0) {
		getServiceLocator()->getGraphicsManager()->unsetVertexBuffer(cbo_);
	}
}

SIZE GUISurface::getSize() {
	return sizeof(GUISurface);
}

Resource::Type GUISurface::getType() {
	return Resource::GUI_SURFACE;
}

bool GUISurface::isValid() {
	return true;
}

void GUISurface::resize() {
	update();
}

SIZE GUISurface::getVertexCount() {
	return sizeof(g_planeVertices) / 3;
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
	return cbo_;
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

void GUISurface::setMargins(float left, float bottom, float right, float top) {
	marginLeft_ = left;
	marginBottom_ = bottom;
	marginRight_ = right;
	marginTop_ = top;
	posX_ = left;
	posY_ = bottom;
}

float GUISurface::getMarginLeft() {
	return marginLeft_;
}

float GUISurface::getMarginBottom() {
	return marginBottom_;
}

float GUISurface::getMarginRight() {
	return marginRight_;
}

float GUISurface::getMarginTop() {
	return marginTop_;
}

void GUISurface::setBackground(string background) {
	setAttribute(ATTR_BACKGROUND, background);
	if (background.length() == 0 || background[0] == '#') {
        if (background.length() == 0) {
            getAmbient().setRGBA(1.0f);
        } else {
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
    } else {
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

string GUISurface::getBackground() {
	return getAttribute(ATTR_BACKGROUND);
}

void GUISurface::setWidth(float width) {
	width_ = width;
	update_ = true;
}

float GUISurface::getWidth() {
    return width_;
}

void GUISurface::setHeight(float height) {
	height_ = height;
	update_ = true;
}

float GUISurface::getHeight() {
    return height_;
}

float GUISurface::getPosX() {
    return posX_;
}

float GUISurface::getPosY() {
    return posY_;
}

UINT32 GUISurface::convertUnitsToPixels(UINT32 units, UnitType type) {
	switch (type) {
	case DEFAULT:
		return units;
	default:
		return units;
	}
}

float GUISurface::getSize(string size, float maxSize) {
	if (VAL_FILL == size) {
		return maxSize;
	}
	float rawSize = toFloat(size.c_str());
	if (string::npos != size.find("%")) {
		return maxSize / 100.0f * rawSize;
	}
	return rawSize;
}

bool GUISurface::create() {
 //   marginLeft_ = getSize(getAttribute(ATTR_MARGIN_LEFT),
	//	(float) getServiceLocator()->getScreenWidth());
	//marginBottom_ = getSize(getAttribute(ATTR_MARGIN_BOTTOM),
	//	(float) getServiceLocator()->getScreenHeight());
 //   marginRight_ = getSize(getAttribute(ATTR_MARGIN_RIGHT),
	//	(float) getServiceLocator()->getScreenWidth());
	//marginTop_ = getSize(getAttribute(ATTR_MARGIN_TOP),
	//	(float) getServiceLocator()->getScreenHeight());
	//width_ = getSize(getAttribute(ATTR_WIDTH),
	//	(float) getServiceLocator()->getScreenHeight());
	//height_ = getSize(getAttribute(ATTR_HEIGHT),
	//	(float) getServiceLocator()->getScreenHeight());
	// 	if (getAttribute(ATTR_TRANSPARENCY).length() > 0) {
	//	setTransparency(toFloat(getAttribute(ATTR_TRANSPARENCY).c_str()));
	//}
	marginLeft_ = getAttributes().getFloat(ATTR_MARGIN_LEFT);
	marginBottom_ = getAttributes().getFloat(ATTR_MARGIN_BOTTOM);
    marginRight_ = getAttributes().getFloat(ATTR_MARGIN_RIGHT);
	marginTop_ = getAttributes().getFloat(ATTR_MARGIN_TOP);
	width_ = getAttributes().getFloat(ATTR_WIDTH);
	height_ = getAttributes().getFloat(ATTR_HEIGHT);
	setTransparency(getAttributes().getFloat(ATTR_TRANSPARENCY, 1.0f));
	posX_ = marginLeft_;
	posY_ = marginBottom_;
	setBackground(getAttribute(ATTR_BACKGROUND));
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
			// Genereate separate uv coordinates.
    vector<VertexPT> vert;
    VertexPT tmp;
	float vertices[] = {
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	};
    SIZE vertexCount = sizeof(vertices) / 3;
    for (SIZE j = 0; j < vertexCount; j++) {
        tmp.uv[0] = g_planeUV[j * 2 + 0];
        tmp.uv[1] = g_planeUV[j * 2 + 1];
		tmp.pos[0] = vertices[j * 3 + 0];
        tmp.pos[1] = vertices[j * 3 + 1];
        tmp.pos[2] = vertices[j * 3 + 2];
        vert.push_back(tmp);
    }
    getServiceLocator()->getGraphicsManager()->setVertexBuffer(
		cbo_, &vert[0], (UINT32) vert.size() * sizeof(VertexPT));
	if (checkGLError("Creating GUI surface resource.")) {
		release();
		return false;
	}
	return true;
}