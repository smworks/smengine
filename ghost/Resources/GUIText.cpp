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
#include "../Node.h"
#include "../Input.h"
#include "../TextManager.h"
#include "../ScriptManager.h"
#include "../Settings.h"
#include "../Camera.h"

bool operator<(const GUIText::SymbolData& left, const GUIText::SymbolData& right) {
	return left.index < right.index;
}

GUIText::GUIText(ServiceLocator* services) :
	GUISurface(services),
	text_(""),
	size_(10),
	textOffsetX_(0),
	textOffsetY_(0)
{}

GUIText::~GUIText() {
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
	symbols_.clear();
	SymbolData sd;
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
	Mat4 matPos, matScale, matProjPos;
	for (SIZE i = 0; i < text_.length(); i++) {
		if (text_[i] == '\n' || text_[i] == '\r') {
			maxHeight += size_;
			textOffsetY -= size_;
			symbolOffset = 0;
			continue;
		}
		sd.symbol = getServiceLocator()->getTextManager()->getSymbol(text_[i]);
		sd.posX = posX + getTextOffsetX() + (float) symbolOffset + sd.symbol->getOffsetX();
		Matrix::translate(matPos,
			sd.posX,
			posY + getTextOffsetY() + sd.symbol->getOffsetY() + textOffsetY - size_, 1.0f);
		Matrix::scale(matScale,
			(float) sd.symbol->getWidth(),
			(float) sd.symbol->getHeight(), 1.0f);
		Matrix::multiply(getServiceLocator()->getCamera()->getProjection2D(), matPos, matProjPos);
		Matrix::multiply(matProjPos, matScale, sd.matProjPosScale);
		symbolOffset += sd.symbol->getAdvance();
		if (symbolOffset > maxWidth) {
			maxWidth = symbolOffset;
		}
		sd.index = (SIZE) text_[i];
		symbols_.push_back(sd);
	}
	SIZE size = symbols_.size();
	sort(symbols_.begin(), symbols_.end());
	setWidth(maxWidth);
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

UINT32 GUIText::getTextSize() {
	return size_;
}

float GUIText::getTextOffsetX() {
	return (float) textOffsetX_;
}

float GUIText::getTextOffsetY() {
	return (float) textOffsetY_;
}

GUIText::SymbolData* GUIText::getSymbolArray() {
	if (symbols_.size() == 0) {
		return 0;
	}
	return &symbols_[0];
}

SIZE GUIText::getSymbolCount() {
	return symbols_.size();
}