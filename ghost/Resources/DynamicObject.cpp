/*
 * DynamicObject.cpp
 *
 *  Created on: 2012.12.27
 *      Author: MS
 */

#include "DynamicObject.h"
#include "Vertex.h"
#include "TextureRGBA.h"
#include "../ResourceManager.h"
#include "../MD2Parser.h"
#include "../Settings.h"
#include "../Multiplatform/GraphicsManager.h"

DynamicObject::DynamicObject(ServiceLocator* services) :
	Resource(services),
	vertexCount_(0),
	frameCount_(0),
	animation_(0),
	frame_(0),
	cbo_(0),
	time_(0.0f),
	vertexStride_(0),
	posOffset_(0),
	uvOffset_(0),
	normalOffset_(0),
	textureName_("")
{}

DynamicObject::~DynamicObject() {
	release();
}

void DynamicObject::release() {
	if (cbo_ != 0) {
		glDeleteBuffers(1, &cbo_);
	}
	vertices_.clear();
	animations_.clear();
	vertexCount_ = 0;
	frameCount_ = 0;
	animation_ = 0;
	frame_ = 0;
	cbo_ = 0;
	time_ = 0;
	textures_.clear();
	uv_.clear();
}

SIZE DynamicObject::getSize() {
	return sizeof(DynamicObject);
}

Resource::Type DynamicObject::getType() {
	return Resource::DYNAMIC_OBJECT;
}

bool DynamicObject::isValid() {
	return vertices_.size() != 0;
}

SIZE DynamicObject::getVertexCount() {
	return vertices_.size() / frameCount_;
}

SIZE DynamicObject::getNormalCount() {
	return vertices_.size() / frameCount_;
}

SIZE DynamicObject::getTexture() {
	int textureIndex = animation_ == 0 ?
		0 : animation_->texture_;
	return textures_.size() == 0 ?
		0 : textures_[textureIndex]->getId();
}

SIZE DynamicObject::getIBO() {
	return 0;
}

BoundingVolume* DynamicObject::getBV() {
	return 0;
}

SIZE DynamicObject::getCBO() {
	return cbo_;
}

SIZE DynamicObject::getIndexCount() {
	return 0;
}

SIZE DynamicObject::getRenderCount() {
	return animations_.size();
}

void DynamicObject::setRenderable(SIZE i) {
	frame_ = i;
}

SIZE DynamicObject::getVertexStride() {
	return vertexStride_;
}

int DynamicObject::getPosOffset() {
	return posOffset_;
}

int DynamicObject::getNormalOffset() {
	return normalOffset_;
}

int DynamicObject::getUVOffset() {
	return uvOffset_;
}

Shader* DynamicObject::getDefaultShader() {
	ServiceLocator* sl = getServiceLocator();
	string& name = sl->getSettings()->getString(
		Settings::DEFAULT_MODEL_SHADER);
	Shader* shader = dynamic_cast<Shader*>(sl->getRM()->get(SHADER, name));
	if (shader == 0) {
		shader = NEW Shader(sl);
		shader->getAttributes().setString(ATTR_FILE, name);
		shader->create();
		sl->getRM()->add(name, shader);
	}
	return shader;
}

void DynamicObject::nextFrame(double time) {
	time_ += time;
	UINT32 offset = 0;
	UINT32 frames = 1;
	double frameDuration = DEFAULT_FRAME_TIME;
	UINT32 frame_ = UINT32(time_ / frameDuration);
	if (animation_ != 0) {
		offset = animation_->offset_;
		frames = animation_->frames_;
		frameDuration = animation_->frameDuration_;
	}
	if (frame_ >= frames) {
		time_ = 0;
		frame_ = 0;
	}
	if (cbo_ != 0) {
		glDeleteBuffers(1, &cbo_);
		cbo_ = 0;
	}
	if (frames > 1) {
		UINT32 nextFrame = frame_ + 1 >= frames ?
			0 : frame_ + 1;
		float* vertices = NEW float[vertexCount_];
		float* normals = NEW float[vertexCount_];
		float interp = float(
			(time_ - (frame_ * frameDuration)) / frameDuration);
		for (UINT32 i = 0; i < vertexCount_; i++) {
			float* curr = &vertices_[0] + offset + vertexCount_ * frame_;
			float* next = &vertices_[0] + offset + vertexCount_ * nextFrame;
			vertices[i] = curr[i] + interp * (next[i] - curr[i]);
		}
		for (UINT32 i = 0; i < vertexCount_ / 3; i++) {
			UINT32 index = normals_[offset / 3
				+ vertexCount_ / 3 * frame_];
			float x = g_md2Normals[index][0];
			float y = g_md2Normals[index][1];
			float z = g_md2Normals[index][2];
			UINT32 nIndex = normals_[offset / 3
				+ vertexCount_ / 3 * nextFrame];
			float nx = g_md2Normals[nIndex][0];
			float ny = g_md2Normals[nIndex][1];
			float nz = g_md2Normals[nIndex][2];
			Vec3 vec(
				x + interp * (nx - x),
				y + interp * (ny - y),
				z + interp * (nz - z));
			vec.normalize();
			normals[i * 3 + 0] = vec.getX();
			normals[i * 3 + 1] = vec.getY();
			normals[i * 3 + 2] = vec.getZ();
		}
		vector<VertexPNT> cbo;
		cbo.reserve(vertexCount_);
		VertexPNT v;
		UINT32 uv = 0;
		for (UINT32 i = 0; i < vertexCount_; i += 3) {
			v.pos[0] = vertices[i];
			v.pos[1] = vertices[i + 1];
			v.pos[2] = vertices[i + 2];
			v.normals[0] = normals[i];
			v.normals[1] = normals[i + 1];
			v.normals[2] = normals[i + 2];
			v.uv[0] = uv_[uv++];
			v.uv[1] = uv_[uv++];
			cbo.push_back(v);
		}
		glGenBuffers(1, &cbo_);
		getServiceLocator()->getGraphicsManager()->bindBuffer(cbo_);
		glBufferData(GL_ARRAY_BUFFER, cbo.size() * sizeof(VertexPNT), &cbo[0], GL_STATIC_DRAW);
		vertexStride_ = sizeof(VertexPNT);
		posOffset_ = offsetof(VertexPNT, pos);
		normalOffset_ = offsetof(VertexPNT, normals);
		uvOffset_ = offsetof(VertexPNT, uv);
		delete [] vertices;
		delete [] normals;
	}
	else {
		vector<VertexPNT> cbo;
		cbo.reserve(vertexCount_);
		VertexPNT v;
		UINT32 uv = 0;
		for (UINT32 i = 0; i < vertexCount_; i += 3) {
			v.pos[0] = vertices_[i];
			v.pos[1] = vertices_[i + 1];
			v.pos[2] = vertices_[i + 2];
			v.normals[0] = normals_[i];
			v.normals[1] = normals_[i + 1];
			v.normals[2] = normals_[i + 2];
			v.uv[0] = uv_[uv++];
			v.uv[1] = uv_[uv++];
			cbo.push_back(v);
		}
		glGenBuffers(1, &cbo_);
		getServiceLocator()->getGraphicsManager()->bindBuffer(cbo_);
		glBufferData(GL_ARRAY_BUFFER, cbo.size() * sizeof(VertexPNT), &cbo[0], GL_STATIC_DRAW);
		vertexStride_ = sizeof(VertexPNT);
		posOffset_ = offsetof(VertexPNT, pos);
		normalOffset_ = offsetof(VertexPNT, normals);
		uvOffset_ = offsetof(VertexPNT, uv);
	}

}

void DynamicObject::setAnimation(string name) {
	if (animations_.find(name) != animations_.end()) {
		animation_ = &animations_.find(name)->second;
	}
}

bool DynamicObject::create() {
	MD2Object obj;
	if (!MD2Parser::parse(
		obj, getAttribute(ATTR_FILE), getServiceLocator()))
	{
		LOGW("Failed to parse object.");
		return false;
	}

	vertexCount_ = obj.header_.numTriangles * 9;
	frameCount_ = obj.header_.numFrames;
	vector<float> tempVertices;
	tempVertices.reserve(obj.header_.numFrames * obj.header_.numVerts);
	vector<UINT16> tempNormals;
	tempNormals.reserve(obj.header_.numFrames * obj.header_.numVerts / 3);
	for (int i = 0; i < obj.header_.numFrames; i++) {
		string name = obj.frames_[i].name;
		name = name.substr(0, name.length() - 1);
		if (animations_.end() == animations_.find(name)) {
			animations_.insert(
				pair<string, Animation>(
				name, Animation(i * vertexCount_)));
		}
		else {
			animations_.find(name)->second.frames_++;
		}
		MD2Frame* frame = &obj.frames_[i];
		for (int j = 0; j < obj.header_.numVerts; j++) {
			tempVertices.push_back(
				(float) frame->vertices[j].v[0] *
				frame->scale[0] + frame->translate[0]);
			tempVertices.push_back(
				(float) frame->vertices[j].v[1] *
				frame->scale[1] + frame->translate[1]);
			tempVertices.push_back(
				(float) frame->vertices[j].v[2] *
				frame->scale[2] + frame->translate[2]);
			tempNormals.push_back(frame->vertices[j].normalIndex);
		}
	}

	for (int i = 0; i < obj.header_.numSkins; i++) {
		string text(obj.skins_[i].name);
		if (!getServiceLocator()->getRM()->has(TEXTURE_2D, text)) {
			TextureRGBA* texture = NEW TextureRGBA(getServiceLocator());
			texture->getAttributes().setString(ATTR_FILE, text);
			texture->create();
			getServiceLocator()->getRM()->add(text, texture);
			textures_.push_back(texture);
		}
		else {
			textures_.push_back(static_cast<TextureRGBA*>(
				getServiceLocator()->getRM()->get(TEXTURE_2D, text)));
		}
	}

	uv_.reserve(obj.header_.numTriangles * 6);
	for (int i = 0; i < obj.header_.numTriangles; i++) {
		MD2Triangle* triangle = &obj.triangles_[i];
		for (int j = 2; j >= 0; j--) {
			int index = triangle->uv[j];
			uv_.push_back((float)
				obj.uv_[index].u / obj.header_.skinWidth);
			uv_.push_back((float)
				obj.uv_[index].v / obj.header_.skinHeight);
		}
	}

	vertices_.reserve(obj.header_.numTriangles * 9);
	normals_.reserve(obj.header_.numTriangles * 3);
	for (int i = 0; i < obj.header_.numFrames; i++) {
		for (int j = 0; j < obj.header_.numTriangles; j++) {
			for (int k = 2; k >= 0; k--) {
				int index = obj.header_.numVerts
					* i + obj.triangles_[j].vertex[k];
				float x = tempVertices[index * 3 + 0];
				float y = tempVertices[index * 3 + 1];
				float z = tempVertices[index * 3 + 2];
				vertices_.push_back(x);
				vertices_.push_back(y);
				vertices_.push_back(z);
				UINT16 normal = tempNormals[index];
				normals_.push_back(normal);
			}
		}
	}
	if (checkGLError("Compiling dynamic object data.")) {
		release();
		return false;
	}
	setAnimation("wave_1");
	setAmbient(Color(1.0f, 1.0f, 1.0f, 1.0f));
	//setDiffuse(Color(0.6f, 0.6f, 0.6f, 1.0f));
	//setSpecular(Color(0.5f, 0.5f, 0.5f, 1.0f));
	//setSpecIntensity(50);

	return true;
}
