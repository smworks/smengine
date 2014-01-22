/*
 * ModelData.cpp
 *
 *  Created on: 2014.01.07
 *      Author: Martynas Šustavičius
 */

#include "ModelData.h"
#include "BoundingVolume.h"
#include "Resources/Renderable.h"

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
	boundingVolume_(0)
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