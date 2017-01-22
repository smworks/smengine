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
	vertexStride(0),
	posOffset(0),
	normalOffset(0),
	uvOffset(0),
	vertexType(VERTEX_TYPE_COUNT),
	vertexCount(0),
	vertices(nullptr),
	indexType(Renderable::INDEX_TYPE_COUNT),
	indexCount(0),
	indices(nullptr),
	facesCulled(true),
	boundingVolume(nullptr)
{
}

ModelData::ModelData(const ModelData& rhs)
{
	vertexStride = rhs.vertexStride;
	posOffset = rhs.posOffset;
	normalOffset = rhs.normalOffset;
	uvOffset = rhs.uvOffset;
	vertexType = rhs.vertexType;
	vertexCount = rhs.vertexCount;
	vertices = new UINT8[rhs.vertexStride * rhs.vertexCount];
	memcpy(vertices, rhs.vertices, rhs.vertexStride * rhs.vertexCount);
	indexType = rhs.indexType;
	indexCount = rhs.indexCount;
	indices = new UINT8[rhs.indexCount * rhs.getIndexSize()];
	memcpy(indices, rhs.indices, rhs.indexCount * rhs.getIndexSize());
	materials = rhs.materials;
	parts = rhs.parts;
	facesCulled = rhs.facesCulled;
	boundingVolume = rhs.boundingVolume->copy();
}

ModelData::~ModelData()
{
	delete boundingVolume;
	delete [] vertices;
	delete [] indices;
}

void ModelData::setVertices(VertexType type, UINT8* vertices, SIZE vertexCount)
{
	vertexType = type;
	this->vertexCount = vertexCount;
	this->vertices = vertices;
	switch (vertexType)
	{
	case PNT:
		vertexStride = sizeof(VertexPNT);
		posOffset = offsetof(VertexPNT, pos);
		normalOffset = offsetof(VertexPNT, normals);
		uvOffset = offsetof(VertexPNT, uv);
		break;
	case PN:
		vertexStride = sizeof(VertexPN);
		posOffset = offsetof(VertexPN, pos);
		normalOffset = offsetof(VertexPN, normals);
		break;
	case PT:
		vertexStride = sizeof(VertexPT);
		posOffset = offsetof(VertexPT, pos);
		uvOffset = offsetof(VertexPT, uv);
		break;
	case P:
		vertexStride = sizeof(VertexP);
		posOffset = offsetof(VertexP, pos);
		break;
	default:
		LOGW("Unknown model vertex type specified.");
		break;
	}
}

SIZE ModelData::getVertexCount() const
{
	return vertexCount;
}

ModelData::VertexType ModelData::getVertexType() const
{
	return vertexType;
}

UINT32 ModelData::getVertexStride() const
{
	return vertexStride;
}

UINT32 ModelData::getPosOffset() const
{
	return posOffset;
}

UINT32 ModelData::getNormalOffset() const
{
	return normalOffset;
}

UINT32 ModelData::getUVOffset() const
{
	return uvOffset;
}

void* ModelData::getVertices() const
{
	return vertices;
}

bool ModelData::hasNormals() const
{
	return vertexType == PNT || vertexType == PN;
}

bool ModelData::hasUV() const
{
	return vertexType == PNT || vertexType == PT;
}

void ModelData::setIndices(UINT8* indices, SIZE indexCount, Renderable::IndexType type)
{
	indexType = type;
	this->indexCount = indexCount;
	this->indices = indices;
}

SIZE ModelData::getIndexCount() const
{
	return indexCount;
}

Renderable::IndexType ModelData::getIndexType() const
{
	return indexType;
}

SIZE ModelData::getIndexSize() const
{
	return indexType == Renderable::INDEX_TYPE_USHORT ? sizeof(UINT16) : sizeof(UINT32);
}

UINT8* ModelData::getIndices() const
{
	return indices;
}

void ModelData::setBoundingVolume(BoundingVolume* bv)
{
	boundingVolume = bv;
}

BoundingVolume* ModelData::getBoundingVolume() const
{
	return boundingVolume;
}

vector<ModelData::Material>& ModelData::getMaterials()
{
	return materials;
}

void ModelData::setParts(vector<ModelData::Part> parts)
{
	this->parts = parts;
}

vector<ModelData::Part>& ModelData::getParts()
{
	return parts;
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
	of.write(reinterpret_cast<char*>(&vertexStride), sizeof(UINT32));
	of.write(reinterpret_cast<char*>(&vertexType), sizeof(VertexType));
	of.write(reinterpret_cast<char*>(&posOffset), sizeof(UINT32));
	of.write(reinterpret_cast<char*>(&normalOffset), sizeof(UINT32));
	of.write(reinterpret_cast<char*>(&uvOffset), sizeof(UINT32));
	of.write(reinterpret_cast<char*>(&vertexCount), sizeof(SIZE));

	SIZE sizeOfVertices;
	if (vertexType == P)
	{
		sizeOfVertices = vertexCount * sizeof(VertexP);
	}
	else if (vertexType == PN)
	{
		sizeOfVertices = vertexCount * sizeof(VertexPN);
	}
	else if (vertexType == PT)
	{
		sizeOfVertices = vertexCount * sizeof(VertexPT);
	}
	else if (vertexType == PNT)
	{
		sizeOfVertices = vertexCount * sizeof(VertexPNT);
	}
	else
	{
		THROWEX("Unable to serialize model with unknown vertex type");
	}

	of.write(reinterpret_cast<char*>(vertices), sizeOfVertices);
	of.write(reinterpret_cast<char*>(&indexType), sizeof(int));
	of.write(reinterpret_cast<char*>(&indexCount), sizeof(SIZE));
	of.write(reinterpret_cast<char*>(indices), indexCount * getIndexSize());
	SIZE materialCount = materials.size();
	of.write(reinterpret_cast<char*>(&materialCount), sizeof(SIZE));
	for (auto& m : materials)
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
	SIZE partCount = parts.size();
	of.write(reinterpret_cast<char*>(&partCount), sizeof(SIZE));
	for (auto& p : parts)
	{
		of.write(reinterpret_cast<char*>(&p.material_), sizeof(SIZE));
		of.write(reinterpret_cast<char*>(&p.offset_), sizeof(SIZE));
		of.write(reinterpret_cast<char*>(&p.indexCount_), sizeof(SIZE));

		BoundingVolume::Type type = p.bv == 0 ? BoundingVolume::UNKNOWN : p.bv->getType();
		of.write(reinterpret_cast<char*>(&type), sizeof(BoundingVolume::Type));
		if (type == BoundingVolume::SPHERE)
		{
			float radius = static_cast<BoundingSphere*>(p.bv)->getRadius(Vec3(1.0));
			of.write(reinterpret_cast<char*>(&radius), sizeof(float));
		}
		else if (type == BoundingVolume::BOX)
		{
			Vec3 sizes = static_cast<BoundingBox*>(p.bv)->getSizes();
			of.write(reinterpret_cast<char*>(&sizes), sizeof(Vec3));
		}
	}
	BoundingVolume::Type type = boundingVolume == 0 ? BoundingVolume::UNKNOWN : boundingVolume->getType();
	of.write(reinterpret_cast<char*>(&type), sizeof(BoundingVolume::Type));
	if (type == BoundingVolume::SPHERE)
	{
		float radius = static_cast<BoundingSphere*>(boundingVolume)->getRadius(Vec3(1.0));
		of.write(reinterpret_cast<char*>(&radius), sizeof(float));
	}
	else if (type == BoundingVolume::BOX)
	{
		Vec3 sizes = static_cast<BoundingBox*>(boundingVolume)->getSizes();
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
	memcpy(&vertexStride, binary, sizeof(UINT32));
	memcpy(&vertexType, binary + (offset += sizeof(UINT32)), sizeof(VertexType));
	memcpy(&posOffset, binary + (offset += sizeof(VertexType)), sizeof(UINT32));
	memcpy(&normalOffset, binary + (offset += sizeof(UINT32)), sizeof(UINT32));
	memcpy(&uvOffset, binary + (offset += sizeof(UINT32)), sizeof(UINT32));
	memcpy(&vertexCount, binary + (offset += sizeof(UINT32)), sizeof(SIZE));

	SIZE sizeOfVertices;
	if (vertexType == P)
	{
		sizeOfVertices = vertexCount * sizeof(VertexP);
	}
	else if (vertexType == PN)
	{
		sizeOfVertices = vertexCount * sizeof(VertexPN);
	}
	else if (vertexType == PT)
	{
		sizeOfVertices = vertexCount * sizeof(VertexPT);
	}
	else if (vertexType == PNT)
	{
		sizeOfVertices = vertexCount * sizeof(VertexPNT);
	}
	else
	{
		THROWEX("Unable to deserialize model with unknown vertex type");
	}

	vertices = NEW UINT8[sizeOfVertices];
	memcpy(vertices, binary + (offset += sizeof(SIZE)), sizeOfVertices);
	memcpy(&indexType, binary + (offset += sizeOfVertices), sizeof(int));
	memcpy(&indexCount, binary + (offset += sizeof(int)), sizeof(SIZE));
	indices = NEW UINT8[indexCount * getIndexSize()];
	memcpy(indices, binary + (offset += sizeof(SIZE)), indexCount * getIndexSize());
	offset += indexCount * getIndexSize();

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

		materials.push_back(m);
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
			p.bv = NEW BoundingSphere(radius);
			offset += sizeof(float);
		}
		else if (type == BoundingVolume::BOX)
		{
			Vec3 sizes;
			memcpy(&sizes, binary + offset, sizeof(Vec3));
			p.bv = new BoundingBox(sizes);
			offset += sizeof(Vec3);
		}

		parts.push_back(p);
	}

	BoundingVolume::Type type;
	memcpy(&type, binary + offset, sizeof(BoundingVolume::Type));
	offset += sizeof(BoundingVolume::Type);

	if (type == BoundingVolume::SPHERE)
	{
		float radius;
		memcpy(&radius, binary + offset, sizeof(float));
		boundingVolume = NEW BoundingSphere(radius);
		offset += sizeof(float);
	}
	else if (type == BoundingVolume::BOX)
	{
		Vec3 sizes;
		memcpy(&sizes, binary + offset, sizeof(Vec3));
		boundingVolume = new BoundingBox(sizes);
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
