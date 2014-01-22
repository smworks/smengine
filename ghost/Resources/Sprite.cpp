/*
 * Sprite.cpp
 *
 *  Created on: 2012.10.27
 *      Author: Martynas Šustavičius
 */

#include "Sprite.h"
#include "Vertex.h"
#include "Shader.h"
#include "../Settings.h"
#include "../ScriptManager.h"
#include "../Shapes.h"
#include "../ResourceManager.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Multiplatform/GraphicsManager.h"
#include "Texture.h"
#include "../Node.h"
#include "../TextureAtlas.h"
#include "TextureRGBA.h"

Sprite::Sprite(ServiceLocator* services) :
	Resource(services),
	image_(0),
	cbo_(0)
	//animationCount_(0),
	//uvBO_(0),
	//animationIndex_(0)
{
}

Sprite::~Sprite() {
	release();
}

bool Sprite::create() {
	vector<VertexPT>* cbo = static_cast<vector<VertexPT>*>(
        Shapes::getShape(
			Shapes::SHAPE_SCREEN_PLANE, Shapes::VERTEX_POS_TEX));
	getServiceLocator()->getGraphicsManager()->setVertexBuffer(
        cbo_, &(*cbo)[0], (UINT32) cbo->size() * sizeof(VertexPT));
    delete cbo;
	if (checkGLError("Compiling sprite data.")) {
		release();
		return false;
	}
	return true;
}

void Sprite::release() {
	image_ = 0;
	if (cbo_ != 0) {
		glDeleteBuffers(1, &cbo_);
		cbo_ = 0;
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
	return cbo_ != 0;
}


SIZE Sprite::getVertexCount() {
	return g_planeVerticeCount / 3;
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
	return cbo_;
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
	ServiceLocator* sl = getServiceLocator();
	string& name = sl->getSettings()->getString(
		Settings::DEFAULT_SPRITE_SHADER);
	Shader* shader = dynamic_cast<Shader*>(sl->getRM()->get(SHADER, name));
	if (shader == 0) {
		shader = NEW Shader(sl);
		shader->getAttributes().setString(ATTR_FILE, name);
		shader->create();
		sl->getRM()->add(name, shader);
	}
	return shader;
}

void Sprite::addTexture(Texture* texture) {
	image_ = texture;
}

Texture* Sprite::getPointerToTexture() {
	return image_;
}

//void Sprite::setAnimationIndex(UINT32 index) {
//	if (index < animationCount_) {
//		animationIndex_ = index;
//	}
//}
//
//void Sprite::setAnimationCount(UINT32 count) {
//	if (uvBO_ != 0 && animationCount_ > 0) {
//		glDeleteBuffers(animationCount_, uvBO_);
//		delete [] uvBO_;
//		uvBO_ = 0;
//	}
//	animationCount_ = count;
//	uvBO_ = NEW GLuint[animationCount_];
//	glGenBuffers(animationCount_, uvBO_);
//	for (UINT32 i = 0; i < animationCount_; i++) {
//		glBindBuffer(GL_ARRAY_BUFFER, uvBO_[i]);
//		// Genereate separate uv coordinates.
//		float fr = (float) i / animationCount_;
//		float to = (float) (i + 1) / animationCount_;
//		float uv[] = {
//			fr, 0.0f,
//			to, 0.0f,
//			to, 1.0f,
//			to, 1.0f,
//			fr, 1.0f,
//			fr, 0.0f
//		};
//		// Bind buffer data.
//		glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//	}
//	CHECK_GL_ERROR("Adding sprite animation.");
//}
//
//UINT32 Sprite::getAnimationCount() {
//	return animationCount_;
//}
//
//GLuint Sprite::getTCBO() {
//	if (uvBO_ != 0 && animationIndex_ < animationCount_) {
//		return uvBO_[animationIndex_];
//	}
//	return tbo_;
//}
//
//int Sprite::getRedColor(float x, float y) {
//	UINT32 col = (int) x;
//	UINT32 row = (int) y;
//	if (row < 0 || col < 0 || row >= image_->getHeight() || col >= image_->getWidth()) {
//		return 0;
//	}
//	return (int) image_->getPixel(row, col).r_;
//}
//
//int Sprite::getGreenColor(float x, float y) {
//	UINT32 col = (int) x;
//	UINT32 row = (int) y;
//	if (row < 0 || col < 0 || row >= image_->getHeight() || col >= image_->getWidth()) {
//		return 0;
//	}
//	return (int) image_->getPixel(row, col).g_;
//}
//
//int Sprite::getBlueColor(float x, float y) {
//	UINT32 col = (int) x;
//	UINT32 row = (int) y;
//	if (row < 0 || col < 0 || row >= image_->getHeight() || col >= image_->getWidth()) {
//		return 0;
//	}
//	return (int) image_->getPixel(row, col).b_;
//}
