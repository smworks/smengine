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

	RawObject(const RawObject& rhs);

	vector<Vec3> positions;
	vector<Vec3> normals;
	vector<Vec2> uvCoordinates;
	vector<Face> faces;

	ObjProperties objProperties;
	VertexProperties vertexProperties;
	Vec3 minVertex;
	Vec3 maxVertex;
	float radius;
	vector<MaterialIndex> matIndices;
	
};

inline RawObject::RawObject(ObjProperties objProperties, VertexProperties vertexProperties):
	objProperties(objProperties), vertexProperties(vertexProperties),
	minVertex(Vec3(FLT_MAX)), maxVertex(Vec3(FLT_MIN)),
	radius(0.0f)
{
	positions.reserve(objProperties.positionCount);
	normals.reserve(objProperties.normalCount);
	uvCoordinates.reserve(objProperties.uvCount);
	faces.reserve(objProperties.faceCount);
}

inline RawObject::RawObject(const RawObject& rhs)
{
	this->positions = rhs.positions;
	this->normals = rhs.normals;
	this->uvCoordinates = rhs.uvCoordinates;
	this->objProperties = rhs.objProperties;
	this->vertexProperties = rhs.vertexProperties;
	this->minVertex = rhs.minVertex;
	this->maxVertex = rhs.maxVertex;
	this->radius = rhs.radius;
	this->matIndices = rhs.matIndices;
	this->faces = rhs.faces;
}


#endif
