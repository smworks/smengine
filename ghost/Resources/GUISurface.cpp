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