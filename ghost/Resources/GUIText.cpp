#include "GUIText.h"
#include "Vertex.h"
#include "TextureRGBA.h"
#include "Symbol.h"
#include "../ResourceManager.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Multiplatform/GraphicsManager.h"
#include "../Node.h"
#include "../TextManager.h"

GUIText::GUIText(ServiceLocator* services) :
	GUISurface(services),
	text(""),
	size(10),
	vbo(0),
	vertexCount(0),
	textWidth(0.0f),
	textHeight(0.0f)
{
	setDiffuse(Color(0.0, 0.0, 0.0));
}

GUIText::~GUIText() {
	getGraphicsManager()->unsetVertexBuffer(vbo);
}

bool GUIText::create() {
    size = toInt(getAttribute(ATTR_SIZE).c_str());
	if (size <= 0) {
		size = DEFAULT_TEXT_SIZE;
	}
    setText(getAttribute(ATTR_TEXT));
	getDiffuse().setRGBA(getAttribute(ATTR_COLOR));
    bool ret = GUISurface::create();
	return ret;
}

void GUIText::update() {
	UINT32 screenWidth = getServiceLocator()->getScreenWidth();
	UINT32 screenHeight = getServiceLocator()->getScreenHeight();
	float maxHeight = static_cast<float>(size);
	float maxWidth = 0.0f;
	SIZE symbolOffset = 0;
	float textOffsetY = size * 0.25f;
	getTextManager()->setFontSize(size);
	VertexPT tmp;
	Symbol* symbol = 0;
	float startX = 0.0f;
	float startY = 0.0f;
	vertices.resize(0);
	for (SIZE i = 0; i < text.length(); i++) {
		if (text[i] == '\n' || text[i] == '\r') {
			maxHeight += size;
			textOffsetY -= size;
			symbolOffset = 0;
			continue;
		}
		symbol = getTextManager()->getSymbol(text[i]);
		if (symbol == 0) {
			continue;
		}
		startX = (float) symbolOffset + symbol->getOffsetX();
		startY = symbol->getOffsetY() + textOffsetY - size;
		float sWidth = (float) symbol->getWidth();
		float sHeight = (float) symbol->getHeight();
		symbolOffset += symbol->getAdvance();
		if (symbolOffset > maxWidth) {
			maxWidth = (float) symbolOffset;
		}
		if (text[i] == ' ') {
			continue;
		}
		// Generate vbo.
		const float* uv = symbol->getUV();
		tmp.pos[0] = startX; tmp.pos[1] = startY; tmp.pos[2] = 0;
		tmp.uv[0] = uv[0]; tmp.uv[1] = uv[1];
		vertices.push_back(tmp);
		tmp.pos[0] = startX + sWidth; tmp.pos[1] = startY; tmp.pos[2] = 0;
		tmp.uv[0] = uv[2]; tmp.uv[1] = uv[3];
		vertices.push_back(tmp);
		tmp.pos[0] = startX + sWidth; tmp.pos[1] = startY + sHeight; tmp.pos[2] = 0;
		tmp.uv[0] = uv[4]; tmp.uv[1] = uv[5];
		vertices.push_back(tmp);
		tmp.pos[0] = startX + sWidth; tmp.pos[1] = startY + sHeight; tmp.pos[2] = 0;
		tmp.uv[0] = uv[6]; tmp.uv[1] = uv[7];
		vertices.push_back(tmp);
		tmp.pos[0] = startX; tmp.pos[1] = startY + sHeight; tmp.pos[2] = 0;
		tmp.uv[0] = uv[8]; tmp.uv[1] = uv[9];
		vertices.push_back(tmp);
		tmp.pos[0] = startX; tmp.pos[1] = startY; tmp.pos[2] = 0;
		tmp.uv[0] = uv[10]; tmp.uv[1] = uv[11];
		vertices.push_back(tmp);
	}
	vertexCount = vertices.size();
	if (vertexCount > 0) {
		getGraphicsManager()->setVertexBuffer(
			vbo, &vertices[0], vertexCount * sizeof(VertexPT));
	}
	textWidth = maxWidth;
	textHeight = maxHeight;
	if (node_->getScale().getX() < textWidth) {
		node_->getScale().setX(textWidth);
	}
	if (node_->getScale().getY() < textHeight) {
		node_->getScale().setY(textHeight);
	}
}

void GUIText::hasFocus() {
}

void GUIText::setText(const string& text) {
    this->text = text;
	update();
}

string& GUIText::getText() {
	return text;
}

void GUIText::setFontSize(SIZE size) {
    this->size = size;
    setText(text);
}

SIZE GUIText::getFontSize() const
{
	return size;
}

SIZE GUIText::getTextWidth() const {
	return (SIZE) textWidth;
}

SIZE GUIText::getTextHeight() const {
    return (SIZE) textHeight;
}

SIZE GUIText::getTextVBO() const
{
	return vbo;
}

SIZE GUIText::getTextVertexCount() const
{
	return vertexCount;
}
