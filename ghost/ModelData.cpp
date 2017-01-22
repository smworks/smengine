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
#include "Resources/Vertex.h"

ModelData::ModelData() :
	vertexStride_(0),
	posOffset_(0),
	normalOffset_(0),
	uvOffset_(0),
	vertexType_(VERTEX_TYPE_COUNT),
	vertexCount_(0),
	vertices_(nullptr),
	indexType_(Renderable::INDEX_TYPE_COUNT),
	indexCount_(0),
	indices(nullptr),
	facesCulled(true),
	boundingVolume_(nullptr)
{
}

ModelData::~ModelData()
{
	delete boundingVolume_;
	delete [] vertices_;
	delete [] indices;
}

void ModelData::setVertices(VertexType type, UINT8* vertices, SIZE vertexCount)
{
	vertexType_ = type;
	vertexCount_ = vertexCount;
	vertices_ = vertices;
	switch (vertexType_)
	{
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

SIZE ModelData::getVertexCount() const
{
	return vertexCount_;
}

ModelData::VertexType ModelData::getVertexType() const
{
	return vertexType_;
}

UINT32 ModelData::getVertexStride() const
{
	return vertexStride_;
}

UINT32 ModelData::getPosOffset() const
{
	return posOffset_;
}

UINT32 ModelData::getNormalOffset() const
{
	return normalOffset_;
}

UINT32 ModelData::getUVOffset() const
{
	return uvOffset_;
}

void* ModelData::getVertices() const
{
	return vertices_;
}

bool ModelData::hasNormals() const
{
	return vertexType_ == PNT || vertexType_ == PN;
}

bool ModelData::hasUV() const
{
	return vertexType_ == PNT || vertexType_ == PT;
}

void ModelData::setIndices(UINT8* indices, SIZE indexCount, Renderable::IndexType type)
{
	indexType_ = type;
	indexCount_ = indexCount;
	this->indices = indices;
}

SIZE ModelData::getIndexCount() const
{
	return indexCount_;
}

Renderable::IndexType ModelData::getIndexType() const
{
	return indexType_;
}

SIZE ModelData::getIndexSize() const
{
	return indexType_ == Renderable::INDEX_TYPE_USHORT ? sizeof(UINT16) : sizeof(UINT32);
}

UINT8* ModelData::getIndices() const
{
	return indices;
}

void ModelData::setBoundingVolume(BoundingVolume* bv)
{
	boundingVolume_ = bv;
}

BoundingVolume* ModelData::getBoundingVolume() const
{
	return boundingVolume_;
}

vector<ModelData::Material>& ModelData::getMaterials()
{
	return materials_;
}

void ModelData::setParts(vector<ModelData::Part> parts)
{
	this->parts_ = parts;
}

vector<ModelData::Part>& ModelData::getParts()
{
	return parts_;
}

void ModelData::setCullFace(bool state)
{
	facesCulled = state;
}

bool ModelData::areFacesCulled() const
{
	return facesCulled;
}

void ModelData::serializeToFile(string path)
{
	PROFILE("Started serialization to file: %s", path.c_str());
	ofstream of(path, ios::binary | ios::out);
	of.write(reinterpret_cast<char*>(&vertexStride_), sizeof(UINT32));
	of.write(reinterpret_cast<char*>(&vertexType_), sizeof(VertexType));
	of.write(reinterpret_cast<char*>(&posOffset_), sizeof(UINT32));
	of.write(reinterpret_cast<char*>(&normalOffset_), sizeof(UINT32));
	of.write(reinterpret_cast<char*>(&uvOffset_), sizeof(UINT32));
	of.write(reinterpret_cast<char*>(&vertexCount_), sizeof(SIZE));

	SIZE sizeOfVertices;
	if (vertexType_ == P)
	{
		sizeOfVertices = vertexCount_ * sizeof(VertexP);
	}
	else if (vertexType_ == PN)
	{
		sizeOfVertices = vertexCount_ * sizeof(VertexPN);
	}
	else if (vertexType_ == PT)
	{
		sizeOfVertices = vertexCount_ * sizeof(VertexPT);
	}
	else if (vertexType_ == PNT)
	{
		sizeOfVertices = vertexCount_ * sizeof(VertexPNT);
	}
	else
	{
		THROWEX("Unable to serialize model with unknown vertex type");
	}

	of.write(reinterpret_cast<char*>(vertices_), sizeOfVertices);
	of.write(reinterpret_cast<char*>(&indexType_), sizeof(int));
	of.write(reinterpret_cast<char*>(&indexCount_), sizeof(SIZE));
	of.write(reinterpret_cast<char*>(indices), indexCount_ * getIndexSize());
	SIZE materialCount = materials_.size();
	of.write(reinterpret_cast<char*>(&materialCount), sizeof(SIZE));
	for (auto& m : materials_)
	{
		of.write(m.name, sizeof(Material::name));
		of.write(reinterpret_cast<char*>(&m.ambient_), sizeof(Color));
		of.write(reinterpret_cast<char*>(&m.diffuse_), sizeof(Color));
		of.write(reinterpret_cast<char*>(&m.specular_), sizeof(Color));
		of.write(reinterpret_cast<char*>(&m.specIntensity_), sizeof(float));
		of.write(reinterpret_cast<char*>(&m.transparency_), sizeof(float));

		SIZE textureNameLength = m.texture_ == 0 ? 0 : m.texture_->getName().size();
		of.write(reinterpret_cast<char*>(&textureNameLength), sizeof(SIZE));

		if (textureNameLength > 0)
		{
			string textureName = m.texture_->getName();
			of.write(textureName.c_str(), textureNameLength);
		}
	}
	SIZE partCount = parts_.size();
	of.write(reinterpret_cast<char*>(&partCount), sizeof(SIZE));
	for (auto& p : parts_)
	{
		of.write(reinterpret_cast<char*>(&p.material_), sizeof(SIZE));
		of.write(reinterpret_cast<char*>(&p.offset_), sizeof(SIZE));
		of.write(reinterpret_cast<char*>(&p.indexCount_), sizeof(SIZE));

		BoundingVolume::Type type = p.bv_ == 0 ? BoundingVolume::UNKNOWN : p.bv_->getType();
		of.write(reinterpret_cast<char*>(&type), sizeof(BoundingVolume::Type));
		if (type == BoundingVolume::SPHERE)
		{
			float radius = static_cast<BoundingSphere*>(p.bv_)->getRadius(Vec3(1.0));
			of.write(reinterpret_cast<char*>(&radius), sizeof(float));
		}
		else if (type == BoundingVolume::BOX)
		{
			Vec3 sizes = static_cast<BoundingBox*>(p.bv_)->getSizes();
			of.write(reinterpret_cast<char*>(&sizes), sizeof(Vec3));
		}
	}
	BoundingVolume::Type type = boundingVolume_ == 0 ? BoundingVolume::UNKNOWN : boundingVolume_->getType();
	of.write(reinterpret_cast<char*>(&type), sizeof(BoundingVolume::Type));
	if (type == BoundingVolume::SPHERE)
	{
		float radius = static_cast<BoundingSphere*>(boundingVolume_)->getRadius(Vec3(1.0));
		of.write(reinterpret_cast<char*>(&radius), sizeof(float));
	}
	else if (type == BoundingVolume::BOX)
	{
		Vec3 sizes = static_cast<BoundingBox*>(boundingVolume_)->getSizes();
		of.write(reinterpret_cast<char*>(&sizes), sizeof(Vec3));
	}
	of.write("sm", sizeof(char) * 2);
	of.close();
	PROFILE("Serialization to file finished.");
}

void ModelData::deserialize(ServiceLocator* sl, const char* binary)
{
	PROFILE("Deserializing binary to model");
	UINT32 offset = 0;
	memcpy(&vertexStride_, binary, sizeof(UINT32));
	memcpy(&vertexType_, binary + (offset += sizeof(UINT32)), sizeof(VertexType));
	memcpy(&posOffset_, binary + (offset += sizeof(VertexType)), sizeof(UINT32));
	memcpy(&normalOffset_, binary + (offset += sizeof(UINT32)), sizeof(UINT32));
	memcpy(&uvOffset_, binary + (offset += sizeof(UINT32)), sizeof(UINT32));
	memcpy(&vertexCount_, binary + (offset += sizeof(UINT32)), sizeof(SIZE));

	SIZE sizeOfVertices;
	if (vertexType_ == P)
	{
		sizeOfVertices = vertexCount_ * sizeof(VertexP);
	}
	else if (vertexType_ == PN)
	{
		sizeOfVertices = vertexCount_ * sizeof(VertexPN);
	}
	else if (vertexType_ == PT)
	{
		sizeOfVertices = vertexCount_ * sizeof(VertexPT);
	}
	else if (vertexType_ == PNT)
	{
		sizeOfVertices = vertexCount_ * sizeof(VertexPNT);
	}
	else
	{
		THROWEX("Unable to deserialize model with unknown vertex type");
	}

	vertices_ = NEW UINT8[sizeOfVertices];
	memcpy(vertices_, binary + (offset += sizeof(SIZE)), sizeOfVertices);
	memcpy(&indexType_, binary + (offset += sizeOfVertices), sizeof(int));
	memcpy(&indexCount_, binary + (offset += sizeof(int)), sizeof(SIZE));
	indices = NEW UINT8[indexCount_ * getIndexSize()];
	memcpy(indices, binary + (offset += sizeof(SIZE)), indexCount_ * getIndexSize());
	offset += indexCount_ * getIndexSize();

	SIZE materialCount;
	memcpy(&materialCount, binary + offset, sizeof(SIZE));
	offset += sizeof(SIZE);

	for (SIZE i = 0; i < materialCount; i++)
	{
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

		if (textureNameLength > 0)
		{
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

	for (SIZE i = 0; i < partCount; i++)
	{
		Part p;
		memcpy(&p.material_, binary + offset, sizeof(SIZE));
		memcpy(&p.offset_, binary + (offset += sizeof(SIZE)), sizeof(SIZE));
		memcpy(&p.indexCount_, binary + (offset += sizeof(SIZE)), sizeof(SIZE));

		BoundingVolume::Type type;
		memcpy(&type, binary + (offset += sizeof(SIZE)), sizeof(BoundingVolume::Type));
		offset += sizeof(BoundingVolume::Type);

		if (type == BoundingVolume::SPHERE)
		{
			float radius;
			memcpy(&radius, binary + offset, sizeof(float));
			p.bv_ = NEW BoundingSphere(radius);
			offset += sizeof(float);
		}
		else if (type == BoundingVolume::BOX)
		{
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

	if (type == BoundingVolume::SPHERE)
	{
		float radius;
		memcpy(&radius, binary + offset, sizeof(float));
		boundingVolume_ = NEW BoundingSphere(radius);
		offset += sizeof(float);
	}
	else if (type == BoundingVolume::BOX)
	{
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

void ModelData::Material::setName(string name)
{
	ASSERT(name.size() < sizeof(this->name),
		"File name %s exceeds %d symbol limit", name.c_str(), sizeof(this->name));
	strcpy(this->name, name.c_str());
}
