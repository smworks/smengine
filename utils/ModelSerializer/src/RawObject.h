#ifndef RAWOBJECT_H_
#define RAWOBJECT_H_

#include "../../../ghost/Multiplatform/Ghost.h"
#include "../../../ghost/Vec3.h"
#include "ObjProperties.h"
#include "VertexProperties.h"
#include "MaterialIndex.h"
#include "Face.h"

struct VertexProperties;

struct RawObject
{
	RawObject(ObjProperties objProperties, VertexProperties vertexProperties);

	/**
	 * Custom copy constructor. Invalidates old object.
	 */
	RawObject(RawObject& rhs);

	~RawObject();

	ObjProperties objProperties;
	VertexProperties vertexProperties;
	Vec3 minVertex;
	Vec3 maxVertex;
	float radius;
	UINT8* vertices; // vertex, normals and uv if applicable
	vector<MaterialIndex> matIndices;
	vector<Face> faces;
};

inline RawObject::RawObject(ObjProperties objProperties, VertexProperties vertexProperties):
	objProperties(objProperties), vertexProperties(vertexProperties),
	minVertex(Vec3(FLT_MAX)), maxVertex(Vec3(FLT_MIN)),
	radius(0.0f)
{
	vertices = NEW UINT8[vertexProperties.vertexSize * objProperties.vertexCount];
	faces = vector<Face>(objProperties.faceCount);
}

inline RawObject::RawObject(RawObject& rhs)
{
	this->objProperties = rhs.objProperties;
	this->vertexProperties = rhs.vertexProperties;
	this->minVertex = rhs.minVertex;
	this->maxVertex = rhs.maxVertex;
	this->radius = rhs.radius;
	this->vertices = rhs.vertices;
	this->matIndices = rhs.matIndices;
	this->faces = rhs.faces;
	rhs.vertices = nullptr;
}

inline RawObject::~RawObject()
{
	delete[] vertices;
}

#endif
