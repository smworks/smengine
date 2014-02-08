/*
 * GUIText.cpp
 *
 *  Created on: 2013.07.13
 *      Author: Martynas Ðustavièius
 */

#include "GUIText.h"
#include "Vertex.h"
#include "TextureRGBA.h"
#include "Symbol.h"
#include "../Shapes.h"
#include "../ResourceManager.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Multiplatform/GraphicsManager.h"
#include "../Node.h"
#include "../Input.h"
#include "../TextManager.h"
#include "../ScriptManager.h"
#include "../Settings.h"
#include "../Camera.h"

GUIText::GUIText(ServiceLocator* services) :
	GUISurface(services),
	text_(""),
	size_(10),
	textOffsetX_(0),
	textOffsetY_(0),
	vbo_(0),
	vertexCount_(0)
{}

GUIText::~GUIText() {
	getServiceLocator()->getGraphicsManager()->unsetVertexBuffer(vbo_);
}

bool GUIText::create() {
    size_ = GUISurface::getSize(getAttribute(ATTR_SIZE));
	if (size_ == 0) {
		size_ = DEFAULT_TEXT_SIZE;
	}
    setText(getAttribute(ATTR_TEXT));
	getDiffuse().setRGBA(getAttribute(ATTR_COLOR));
    bool ret = GUISurface::create();
	return ret;
}

Resource::Type GUIText::getType() {
	return Resource::GUI_TEXT;
}

void GUIText::update() {
	UINT32 screenWidth = getServiceLocator()->getScreenWidth();
	UINT32 screenHeight = getServiceLocator()->getScreenHeight();
	float posX = 0.0f, posY = 0.0f, width = 0.0f, height = 0.0f;
	bool screenLeft = toBool(getAttribute(Resource::ATTR_SCREEN_LEFT)),
		screenRight = toBool(getAttribute(Resource::ATTR_SCREEN_RIGHT)),
		screenTop = toBool(getAttribute(Resource::ATTR_SCREEN_TOP)),
		screenBottom = toBool(getAttribute(Resource::ATTR_SCREEN_BOTTOM));
	if (screenLeft && screenRight) {
		posX = 0.0f;
		width = (float) screenWidth;
	}
	else if (screenRight) {
		posX = (float) screenWidth - width;
	}
	else if (screenLeft) {
		posX = 0.0f;
	}
	if (screenTop && screenBottom) {
		posY = 0.0f;
		height = (float) screenHeight;
	}
	else if (screenTop) {
		posY = (float) screenHeight; // TODO: MUST BE UPDATED ON RESIZE!!!
	}
	else if (screenBottom) {
		posY = 0.0;
	}
	SIZE maxHeight = size_;
	SIZE maxWidth = 0;
	SIZE symbolOffset = 0;
	float textOffsetY = size_ * 0.25f;
	getServiceLocator()->getTextManager()->setFontSize(size_);
	VertexPT tmp;
	Symbol* symbol = 0;
	float startX = 0.0f;
	float startY = 0.0f;
	vertices_.resize(0);
	for (SIZE i = 0; i < text_.length(); i++) {
		if (text_[i] == '\n' || text_[i] == '\r') {
			maxHeight += size_;
			textOffsetY -= size_;
			symbolOffset = 0;
			continue;
		}
		symbol = getServiceLocator()->getTextManager()->getSymbol(text_[i]);
		startX = posX + getTextOffsetX() + (float) symbolOffset + symbol->getOffsetX();
		startY = posY + getTextOffsetY() + symbol->getOffsetY() + textOffsetY - size_;
		float sWidth = (float) symbol->getWidth();
		float sHeight = (float) symbol->getHeight();
		symbolOffset += symbol->getAdvance();
		if (symbolOffset > maxWidth) {
			maxWidth = symbolOffset;
		}
		// Generate vbo.
		const float* uv = symbol->getUV();
		tmp.pos[0] = startX; tmp.pos[1] = startY; tmp.pos[2] = 0;
		tmp.uv[0] = uv[0]; tmp.uv[1] = uv[1];
		vertices_.push_back(tmp);
		tmp.pos[0] = startX + sWidth; tmp.pos[1] = startY; tmp.pos[2] = 0;
		tmp.uv[0] = uv[2]; tmp.uv[1] = uv[3];
		vertices_.push_back(tmp);
		tmp.pos[0] = startX + sWidth; tmp.pos[1] = startY + sHeight; tmp.pos[2] = 0;
		tmp.uv[0] = uv[4]; tmp.uv[1] = uv[5];
		vertices_.push_back(tmp);
		tmp.pos[0] = startX + sWidth; tmp.pos[1] = startY + sHeight; tmp.pos[2] = 0;
		tmp.uv[0] = uv[6]; tmp.uv[1] = uv[7];
		vertices_.push_back(tmp);
		tmp.pos[0] = startX; tmp.pos[1] = startY + sHeight; tmp.pos[2] = 0;
		tmp.uv[0] = uv[8]; tmp.uv[1] = uv[9];
		vertices_.push_back(tmp);
		tmp.pos[0] = startX; tmp.pos[1] = startY; tmp.pos[2] = 0;
		tmp.uv[0] = uv[10]; tmp.uv[1] = uv[11];
		vertices_.push_back(tmp);
	}
	vertexCount_ = vertices_.size();
	if (vertexCount_ > 0) {
		getServiceLocator()->getGraphicsManager()->setVertexBuffer(
			vbo_, &vertices_[0], vertexCount_ * sizeof(VertexPT));
	}
	setWidth(maxWidth > (SIZE) width ? maxWidth : (SIZE) width);
	setHeight(maxHeight);
}

void GUIText::hasFocus() {
}

void GUIText::setText(const string& text) {
    text_ = text;
	update();
}

string& GUIText::getText() {
	return text_;
}

void GUIText::setTextSize(SIZE size) {
    size_ = size;
    setText(text_);
}

SIZE GUIText::getTextSize() {
	return size_;
}

float GUIText::getTextOffsetX() {
	return (float) textOffsetX_;
}

float GUIText::getTextOffsetY() {
	return (float) textOffsetY_;
}

SIZE GUIText::getTextVBO() {
	return vbo_;
}

SIZE GUIText::getTextVertexCount() {
	return vertexCount_;
}