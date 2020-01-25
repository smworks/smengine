#include "../Multiplatform/ServiceLocator.h"
#include "../Multiplatform/GraphicsManager.h"
#include "GUISurface.h"
#include "Vertex.h"
#include "AtlasTexture.h"
#include "TextureRGBA.h"
#include "../Shapes.h"
#include "../ResourceManager.h"
#include "../Node.h"

GUISurface::GUISurface(ServiceLocator* services) :
	Resource(services),
	surfaceId(0),
	shouldUpdate(true),
	textureBackground(nullptr),
	cbo(0)
{}

GUISurface::~GUISurface() {
	GUISurface::release();
}

bool GUISurface::create() {
	setTransparency(getAttributes().getFloat(ATTR_TRANSPARENCY, 1.0f));
	setBackground(getAttribute(ATTR_BACKGROUND));
	
	// Genereate separate uv coordinates.
	vector<VertexPT>* uvbo = static_cast<vector<VertexPT>*>(
		Shapes::getShape(Shapes::SHAPE_SCREEN_PLANE, Shapes::VERTEX_POS_TEX));
    getGraphicsManager()->setVertexBuffer(
		cbo, &(*uvbo)[0], (UINT32)uvbo->size() * sizeof(VertexPT));
	delete uvbo;
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
	string name = "default_model_shader";
	Shader* shader = dynamic_cast<Shader*>(getResourceManager()->get(SHADER, name));
	if (shader == nullptr)
	{
		auto pair = getGraphicsManager()->getDefaultSpriteShader();
		shader = NEW Shader(getServiceLocator());
		shader->setVertexAndFragment(pair.first, pair.second);
		shader->create();
		getResourceManager()->add(name, shader);
	}
	return shader;
}

Texture* GUISurface::getPointerToTexture() const
{
	return textureBackground;
}

void GUISurface::setBackground(string background) {
	setAttribute(ATTR_BACKGROUND, background);
	if (background.length() == 0 || background[0] == '#') {
        if (background.length() == 0) {
			backgroundColor.setRGBA(1.0f);
        } else {
			backgroundColor.setRGBA(background);
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
		setAmbient(backgroundColor);
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