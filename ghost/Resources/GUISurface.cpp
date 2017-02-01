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
	surfaceId(0),
	widthSize(DEFAULT),
	heightSize(DEFAULT),
	marginLeft(0.0f),
	marginBottom(0.0f),
	marginRight(0.0f),
	marginTop(0.0f),
	posX(0.0f),
	posY(0.0f),
	width(0.0f),
	height(0.0f),
	shouldUpdate(true),
	textureBackground(nullptr),
	cbo(0)
{}

GUISurface::~GUISurface() {
	GUISurface::release();
}

bool GUISurface::create() {
	marginLeft = getAttributes().getFloat(ATTR_MARGIN_LEFT, marginLeft);
	marginBottom = getAttributes().getFloat(ATTR_MARGIN_BOTTOM, marginBottom);
    marginRight = getAttributes().getFloat(ATTR_MARGIN_RIGHT, marginRight);
	marginTop = getAttributes().getFloat(ATTR_MARGIN_TOP, marginTop);
	width = getAttributes().getFloat(ATTR_WIDTH, width);
	height = getAttributes().getFloat(ATTR_HEIGHT, height);
	setTransparency(getAttributes().getFloat(ATTR_TRANSPARENCY, 1.0f));
	posX = marginLeft;
	posY = marginTop;
	setBackground(getAttribute(ATTR_BACKGROUND));
	
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
    getGraphicsManager()->setVertexBuffer(
		cbo, &vert[0], vert.size() * sizeof(VertexPT));
	if (checkGLError("Creating GUI surface resource.")) {
		release();
		return false;
	}
	return true;
}


void GUISurface::release() {
	textureBackground = 0;
	shouldUpdate = true;
	if (cbo != 0) {
		getGraphicsManager()->unsetVertexBuffer(cbo);
	}
}

SIZE GUISurface::getSize() {
	return sizeof(GUISurface);
}

Resource::Type GUISurface::getType() {
	return GUI_SURFACE;
}

bool GUISurface::isValid() {
	return true;
}

void GUISurface::resize() {
	update();
}

SIZE GUISurface::getVertexCount() {
	return 6;
}

SIZE GUISurface::getTexture() {
	return textureBackground == nullptr ? 0 : textureBackground->getId();
}

SIZE GUISurface::getIBO() {
	return 0;
}

BoundingVolume* GUISurface::getBV() {
	return nullptr;
}

SIZE GUISurface::getCBO() {
	return cbo;
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

Texture* GUISurface::getPointerToTexture() const
{
	return textureBackground;
}

void GUISurface::setMargins(float left, float top, float right, float bottom) {
	marginLeft = left;
	marginBottom = bottom;
	marginRight = right;
	marginTop = top;
	posX = left;
	posY = top;
}

float GUISurface::getMarginLeft() const
{
	return marginLeft;
}

float GUISurface::getMarginBottom() const
{
	return marginBottom;
}

float GUISurface::getMarginRight() const
{
	return marginRight;
}

float GUISurface::getMarginTop() const
{
	return marginTop;
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
			getResourceManager()->get(SHADER, SURFACE_BCKG_COLOR));
		if (shader == 0) {
			shader = NEW Shader(getServiceLocator());
			shader->getAttributes().setString(
				ATTR_FILE, SURFACE_BCKG_COLOR);
			shader->create();
			getResourceManager()->add(SURFACE_BCKG_COLOR, shader);
		}
		setShader(shader);
    } else {
        textureBackground = static_cast<TextureRGBA*>(
			getResourceManager()->get(
				TEXTURE_2D, background));
		if (textureBackground == 0) {
			textureBackground = Texture::load(getServiceLocator(), background);
			getResourceManager()->add(background,
				textureBackground);
		}
		Shader* shader = static_cast<Shader*>(
			getResourceManager()->get(SHADER, SURFACE_BCKG_IMAGE));
		if (shader == 0) {
			shader = NEW Shader(getServiceLocator());
			shader->getAttributes().setString(
				ATTR_FILE, SURFACE_BCKG_IMAGE);
			shader->create();
			getResourceManager()->add(SURFACE_BCKG_IMAGE, shader);
		}
		setShader(shader);
    }
	shouldUpdate = true;
}

string GUISurface::getBackground() {
	return getAttribute(ATTR_BACKGROUND);
}

void GUISurface::setWidth(float width) {
	this->width = width;
	shouldUpdate = true;
}

float GUISurface::getWidth() const
{
    return width;
}

void GUISurface::setHeight(float height) {
	this->height = height;
	shouldUpdate = true;
}

float GUISurface::getHeight() const
{
    return height;
}

float GUISurface::getPosX() const
{
    return posX;
}

float GUISurface::getPosY() const
{
    return posY;
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