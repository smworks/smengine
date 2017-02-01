/*
 * Sprite.cpp
 *
 *  Created on: 2012.10.27
 *      Author: Martynas Šustavičius
 */

#include "Sprite.h"
#include "Vertex.h"
#include "Shader.h"
#include "../Multiplatform/Database.h"
#include "../ScriptManager.h"
#include "../Shapes.h"
#include "../ResourceManager.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Multiplatform/GraphicsManager.h"
#include "Texture.h"
#include "../Node.h"
#include "Texture.h"

Sprite::Sprite(ServiceLocator* services) :
	Resource(services),
	image_(0),
	uvBO_(0),
    spriteCount_(0),
	spriteIndex_(0),
	width_(1.0f),
	height_(1.0f)
	//animationCount_(0),
	//uvBO_(0),
	//animationIndex_(0)
{
    setCullFace(false);
}

Sprite::~Sprite() {
	release();
}

bool Sprite::create() {
    string spriteCount = getAttribute(Resource::ATTR_SPRITE_COUNT);
    if (spriteCount.length() > 0) {
        spriteCount_ = toUint(spriteCount.c_str());
    }
    string file = getAttribute(Resource::ATTR_FILE);
    if (file.length() > 0) {
		Texture* texture = Texture::load(getServiceLocator(), file);
        image_ = texture;
        if (spriteCount_ == 0) {
            spriteCount_ = 1;
        }
    }
	width_ = getAttributes().getFloat(ATTR_WIDTH, width_);
	height_ = getAttributes().getFloat(ATTR_HEIGHT, height_);
    setSpriteCount(spriteCount_);
	if (checkGLError("Compiling sprite data.")) {
		release();
		return false;
	}
	return true;
}

void Sprite::release() {
	image_ = 0;
	if (uvBO_ != 0) {
		getServiceLocator()->getGraphicsManager()->unsetVertexBuffers(spriteCount_, uvBO_);
	}
}

SIZE Sprite::getSize() {
	return sizeof(Sprite);
}

Resource::Type Sprite::getType() {
	return Resource::SPRITE;
}

void Sprite::resize() {

}

bool Sprite::isValid() {
	return uvBO_ != 0;
}


SIZE Sprite::getVertexCount() {
	return 6;
}


SIZE Sprite::getNormalCount() {
	return 0;
}

SIZE Sprite::getTexture() {
	return image_ == 0 ? 0 : image_->getId();
}

SIZE Sprite::getIBO() {
	return 0;
}

BoundingVolume* Sprite::getBV() {
	return 0;
}

SIZE Sprite::getCBO() {
	return spriteCount_ == 0 ? 0 : uvBO_[spriteIndex_];
}

SIZE Sprite::getIndexCount() {
	return 0;
}

SIZE Sprite::getRenderCount() {
	return 1;
}

void Sprite::setRenderable(SIZE i) {

}

SIZE Sprite::getVertexStride() {
	return sizeof(VertexPT);
}

int Sprite::getPosOffset() {
	return offsetof(VertexPT, pos);
}

int Sprite::getNormalOffset() {
	return -1;
}

int Sprite::getUVOffset() {
	return offsetof(VertexPT, uv);
}

Shader* Sprite::getDefaultShader() {
	string name = "default_sprite_shader";
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

void Sprite::setTexture(Texture* texture) {
	image_ = texture;
	setSpriteCount(1);
}

Texture* Sprite::getPointerToTexture() {
	return image_;
}

void Sprite::setSpriteIndex(SIZE index) {
	if (index < spriteCount_) {
		spriteIndex_ = index;
	}
}

void Sprite::setSpriteCount(SIZE count) {
	if (uvBO_ != 0) {
		getServiceLocator()->getGraphicsManager()->unsetVertexBuffers(spriteCount_, uvBO_);
	}
	spriteCount_ = count == 0 ? 1 : count;
	uvBO_ = NEW UINT32[spriteCount_];
	fill(uvBO_, uvBO_ + spriteCount_, 0);
	for (SIZE i = 0; i < spriteCount_; i++) {
		// Genereate separate uv coordinates.
		float fr = (float) i /  spriteCount_;
		float to = (float) (i + 1) / spriteCount_;
        float uv[] = {
			fr, 0.0f,
			to, 0.0f,
			to, 1.0f,
			to, 1.0f,
			fr, 1.0f,
			fr, 0.0f
		};
        vector<VertexPT> vert;
        VertexPT tmp;
        SIZE vertexCount = sizeof(g_planeVertices) / 3;
        for (SIZE j = 0; j < vertexCount; j++) {
            tmp.uv[0] = uv[j * 2 + 0];
            tmp.uv[1] = uv[j * 2 + 1];
			tmp.pos[0] = g_planeVertices[j * 3 + 0] * width_;
            tmp.pos[1] = g_planeVertices[j * 3 + 1] * height_;
            tmp.pos[2] = g_planeVertices[j * 3 + 2];
            vert.push_back(tmp);
        }
        getServiceLocator()->getGraphicsManager()->setVertexBuffer(
			uvBO_[i], &vert[0], (UINT32) vert.size() * sizeof(VertexPT));
	}
	CHECK_GL_ERROR("Adding sprite animation.");
}

SIZE Sprite::getSpriteCount() {
	return spriteCount_;
}
