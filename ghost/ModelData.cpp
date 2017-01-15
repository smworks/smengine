/*
 * ModelData.cpp
 *
 *  Created on: 2014.01.07
 *      Author: Martynas Šustavičius
 */

#include "ModelData.h"
#include "BoundingSphere.h"
#include "BoundingBox.h"
#include "Resources/Renderable.h"
#include "Resources/Texture.h"

ModelData::ModelData() :
	vertexType_(VERTEX_TYPE_COUNT),
	vertexStride_(0),
	posOffset_(0),
	normalOffset_(0),
	uvOffset_(0),
	vertexCount_(0),
	vertices_(0),
	indexType_(Renderable::INDEX_TYPE_COUNT),
	indexCount_(0),
	boundingVolume_(0),
	facesCulled(true)
{}

ModelData::~ModelData() {
	delete boundingVolume_;
	delete [] vertices_;
	switch (indexType_) {
	case Renderable::INDEX_TYPE_USHORT:
		delete [] indices_.indicesShort;
		break;
	case Renderable::INDEX_TYPE_UINT:
		delete [] indices_.indicesInt;
		break;
	}
}

void ModelData::setVertices(VertexType type, UINT8* vertices, SIZE vertexCount) {
	vertexType_ = type;
	vertexCount_ = vertexCount;
	vertices_ = vertices;
	switch (vertexType_) {
	case PNT:
		vertexStride_ = sizeof(VertexPNT);
		posOffset_ = offsetof(VertexPNT, pos);
		normalOffset_ = offsetof(VertexPNT, normals);
		uvOffset_ = offsetof(VertexPNT, uv);
		break;
	case PN:
		vertexStride_ = sizeof(VertexPN);
		posOffset_ = offsetof(VertexPN, pos);
		normalOffset_ = offsetof(VertexPN, normals);
		break;
	case PT:
		vertexStride_ = sizeof(VertexPT);
		posOffset_ = offsetof(VertexPT, pos);
		uvOffset_ = offsetof(VertexPT, uv);
		break;
	case P:
		vertexStride_ = sizeof(VertexP);
		posOffset_ = offsetof(VertexP, pos);
		break;
	default:
		LOGW("Unknown model vertex type specified.");
		break;
	}
}

SIZE ModelData::getVertexCount() {
	return vertexCount_;
}

ModelData::VertexType ModelData::getVertexType() {
	return vertexType_;
}

UINT32 ModelData::getVertexStride() {
	return vertexStride_;
}

UINT32 ModelData::getPosOffset() {
	return posOffset_;
}

UINT32 ModelData::getNormalOffset() {
	return normalOffset_;
}

UINT32 ModelData::getUVOffset() {
	return uvOffset_;
}

void* ModelData::getVertices() {
	return vertices_;
}

bool ModelData::hasNormals() {
	return vertexType_ == PNT || vertexType_ == PN;
}

bool ModelData::hasUV() {
	return vertexType_ == PNT || vertexType_ == PT;
}

void ModelData::setIndices(int type, void* indices, SIZE indexCount) {
	indexType_ = type;
	indexCount_ = indexCount;
	switch (indexType_) {
	case Renderable::INDEX_TYPE_USHORT:
		indices_.indicesShort = reinterpret_cast<UINT16*>(indices);
		break;
	case Renderable::INDEX_TYPE_UINT:
		indices_.indicesInt = reinterpret_cast<UINT32*>(indices);
		break;
	default:
		LOGW("Unknown model index type specified.");
		break;
	}
}

SIZE ModelData::getIndexCount() {
	return indexCount_;
}

int ModelData::getIndexType() {
	return indexType_;
}

UINT32 ModelData::getIndexStride() {
	return indexType_ == Renderable::INDEX_TYPE_USHORT ? sizeof(UINT16) : sizeof(UINT32);
}

UINT16* ModelData::getIndicesShort() {
	return indexType_ == Renderable::INDEX_TYPE_USHORT ? indices_.indicesShort : 0;
}

UINT32* ModelData::getIndicesInt() {
	return indexType_ == Renderable::INDEX_TYPE_UINT ? indices_.indicesInt : 0;
}

void ModelData::setBoundingVolume(BoundingVolume* bv) {
	boundingVolume_ = bv;
}

BoundingVolume* ModelData::getBoundingVolume() {
	return boundingVolume_;
}

vector<ModelData::Material>& ModelData::getMaterials() {
	return materials_;
}
vector<ModelData::Part>& ModelData::getParts() {
	return parts_;
}

void ModelData::setCullFace(bool state) {
	facesCulled = state;
}

bool ModelData::areFacesCulled() {
	return facesCulled;
}

void ModelData::serializeToFile(string path) {
	PROFILE("Started serialization to file: %s", path.c_str());
	ofstream of(path, ios::binary | ios::out);
	of.write((char*)&vertexStride_, sizeof(UINT32));
	of.write((char*)&vertexType_, sizeof(VertexType));
	of.write((char*)&posOffset_, sizeof(UINT32));
	of.write((char*)&normalOffset_, sizeof(UINT32));
	of.write((char*)&uvOffset_, sizeof(UINT32));
	of.write((char*)&vertexCount_, sizeof(SIZE));
	of.write((char*)vertices_, vertexCount_ * sizeof(float));
	of.write((char*)&indexType_, sizeof(int));
	of.write((char*)&indexCount_, sizeof(SIZE));
	if (indexType_ == Renderable::INDEX_TYPE_USHORT) {
		of.write((char*)indices_.indicesShort, indexCount_ * sizeof(UINT16));
	} else {
		of.write((char*)indices_.indicesInt, indexCount_ * sizeof(UINT32));
	}
	SIZE materialCount = materials_.size();
	of.write((char*)&materialCount, sizeof(SIZE));
	for (Material& m : materials_) {
		of.write((char*)m.name, sizeof(Material::name));
		of.write((char*)&m.ambient_, sizeof(Color));
		of.write((char*)&m.diffuse_, sizeof(Color));
		of.write((char*)&m.specular_, sizeof(Color));
		of.write((char*)&m.specIntensity_, sizeof(float));
		of.write((char*)&m.transparency_, sizeof(float));
		
		SIZE textureNameLength = m.texture_ == 0 ? 0 : m.texture_->getName().size();
		of.write((char*)&textureNameLength, sizeof(SIZE));

		if (textureNameLength > 0) {
			string textureName = m.texture_->getName();
			of.write(textureName.c_str(), textureNameLength);
		}
	}
	SIZE partCount = parts_.size();
	of.write((char*)&partCount, sizeof(SIZE));
	for (Part& p : parts_) {
		of.write((char*)&p.material_, sizeof(SIZE));
		of.write((char*)&p.offset_, sizeof(SIZE));
		of.write((char*)&p.indexCount_, sizeof(SIZE));

		BoundingVolume::Type type = p.bv_ == 0 ? BoundingVolume::UNKNOWN : p.bv_->getType();
		of.write((char*)&type, sizeof(BoundingVolume::Type));
		if (type == BoundingVolume::SPHERE) {
			float radius = ((BoundingSphere*)p.bv_)->getRadius(Vec3(1.0));
			of.write((char*)&radius, sizeof(float));
		}
		else if (type == BoundingVolume::BOX) {
			Vec3 sizes = ((BoundingBox*)p.bv_)->getSizes();
			of.write((char*)&sizes, sizeof(Vec3));
		}
	}
	BoundingVolume::Type type = boundingVolume_ == 0 ? BoundingVolume::UNKNOWN : boundingVolume_->getType();
	of.write((char*)&type, sizeof(BoundingVolume::Type));
	if (type == BoundingVolume::SPHERE) {
		float radius = ((BoundingSphere*)boundingVolume_)->getRadius(Vec3(1.0));
		of.write((char*)&radius, sizeof(float));
	}
	else if (type == BoundingVolume::BOX) {
		Vec3 sizes = ((BoundingBox*)boundingVolume_)->getSizes();
		of.write((char*)&sizes, sizeof(Vec3));
	}
	of.write("sm", sizeof(char) * 2);
	of.close();
	PROFILE("Serialization to file finished.");
}

void ModelData::deserialize(ServiceLocator* sl, const char* binary) {
	PROFILE("Deserializing binary to model");
	UINT32 offset = 0;
	memcpy(&vertexStride_, binary, sizeof(UINT32));
	memcpy(&vertexType_, binary + (offset += sizeof(UINT32)), sizeof(VertexType));
	memcpy(&posOffset_, binary + (offset += sizeof(VertexType)), sizeof(UINT32));
	memcpy(&normalOffset_, binary + (offset += sizeof(UINT32)), sizeof(UINT32));
	memcpy(&uvOffset_, binary + (offset += sizeof(UINT32)), sizeof(UINT32));
	memcpy(&vertexCount_, binary + (offset += sizeof(UINT32)), sizeof(SIZE));
	SIZE sizeOfVertices = vertexCount_ * sizeof(float);
	vertices_ = NEW UINT8[sizeOfVertices];
	memcpy(vertices_, binary + (offset += sizeof(SIZE)), sizeOfVertices);
	memcpy(&indexType_, binary + (offset += sizeOfVertices), sizeof(int));
	memcpy(&indexCount_, binary + (offset += sizeof(int)), sizeof(SIZE));
	if (indexType_ == Renderable::INDEX_TYPE_USHORT) {
		indices_.indicesShort = NEW UINT16[indexCount_];
		memcpy(indices_.indicesShort, binary + (offset += sizeof(SIZE)), indexCount_ * sizeof(UINT16));
		offset += indexCount_ * sizeof(UINT16);
	} else {
		indices_.indicesInt = NEW UINT32[indexCount_];
		memcpy(indices_.indicesInt, binary + (offset += sizeof(SIZE)), indexCount_ * sizeof(UINT32));
		offset += indexCount_ * sizeof(UINT32);
	}
	SIZE materialCount;
	memcpy(&materialCount, binary + offset, sizeof(SIZE));
	offset += sizeof(SIZE);

	for (SIZE i = 0; i < materialCount; i++) {
		Material m;
		memcpy(m.name, binary + offset, sizeof(Material::name));
		memcpy(&m.ambient_, binary + (offset += sizeof(Material::name)), sizeof(Color));
		memcpy(&m.diffuse_, binary + (offset += sizeof(Color)), sizeof(Color));
		memcpy(&m.specular_, binary + (offset += sizeof(Color)), sizeof(Color));
		memcpy(&m.specIntensity_, binary + (offset += sizeof(Color)), sizeof(float));
		memcpy(&m.transparency_, binary + (offset += sizeof(float)), sizeof(float));

		SIZE textureNameLength;
		memcpy(&textureNameLength, binary + (offset += sizeof(float)), sizeof(SIZE));
		offset += sizeof(SIZE);

		if (textureNameLength > 0) {
			char* name = NEW char[textureNameLength];
			memcpy(name, binary + offset, textureNameLength);
			offset += textureNameLength;
			m.texture_ = Texture::load(sl, string(name, textureNameLength));
			delete[] name;
		}

		materials_.push_back(m);
	}

	SIZE partCount;
	memcpy(&partCount, binary + offset, sizeof(SIZE));
	offset += sizeof(SIZE);

	for (SIZE i = 0; i < partCount; i++) {
		Part p;
		memcpy(&p.material_, binary + offset, sizeof(SIZE));
		memcpy(&p.offset_, binary + (offset += sizeof(SIZE)), sizeof(SIZE));
		memcpy(&p.indexCount_, binary + (offset += sizeof(SIZE)), sizeof(SIZE));
		
		BoundingVolume::Type type;
		memcpy(&type, binary + (offset += sizeof(SIZE)), sizeof(BoundingVolume::Type));
		offset += sizeof(BoundingVolume::Type);

		if (type == BoundingVolume::SPHERE) {
			float radius;
			memcpy(&radius, binary + offset, sizeof(float));
			p.bv_ = NEW BoundingSphere(radius);
			offset += sizeof(float);
		}
		else if (type == BoundingVolume::BOX) {
			Vec3 sizes;
			memcpy(&sizes, binary + offset, sizeof(Vec3));
			p.bv_ = new BoundingBox(sizes);
			offset += sizeof(Vec3);
		}
		
		parts_.push_back(p);
	}

	BoundingVolume::Type type;
	memcpy(&type, binary + offset, sizeof(BoundingVolume::Type));
	offset += sizeof(BoundingVolume::Type);

	if (type == BoundingVolume::SPHERE) {
		float radius;
		memcpy(&radius, binary + offset, sizeof(float));
		boundingVolume_ = NEW BoundingSphere(radius);
		offset += sizeof(float);
	}
	else if (type == BoundingVolume::BOX) {
		Vec3 sizes;
		memcpy(&sizes, binary + offset, sizeof(Vec3));
		boundingVolume_ = new BoundingBox(sizes);
		offset += sizeof(Vec3);
	}

	char sm[2];
	memcpy(sm, binary + offset, sizeof(char) * 2);
	PROFILE("Deserialization is finished");
	ASSERT(sm[0] == 's' && sm[1] == 'm', "Unable to verify deserialized model");
}

void ModelData::Material::setName(string name) {
	ASSERT(name.size() < sizeof(this->name),
		"File name %s exceeds %d symbol limit", name.c_str(), sizeof(this->name));
	strcpy(this->name, name.c_str());
}