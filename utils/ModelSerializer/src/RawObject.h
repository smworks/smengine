#ifndef RAWOBJECT_H_
#define RAWOBJECT_H_

#include "../../../ghost/Multiplatform/Ghost.h"
#include "../../../ghost/Vec3.h"
#include "Face.h"

struct VertexProperties;

struct MaterialRange
{
	MaterialRange() :
		materialName(""),
		faceOffset(0),
		faceCount(0)
	{
	}

	MaterialRange(string materialName, SIZE faceOffset, SIZE faceCount) :
		materialName(materialName),
		faceOffset(faceOffset),
		faceCount(faceCount)
	{
	}

	string materialName;
	SIZE faceOffset;
	SIZE faceCount;
};

struct RawObject
{
	RawObject();

	RawObject(const RawObject& rhs);

	vector<Vec3> positions;
	vector<Vec3> normals;
	vector<Vec2> uvCoordinates;
	vector<Face> faces;
	vector<string> materialFiles;
	vector<MaterialRange> materialRanges;
};

inline RawObject::RawObject()
{
}

inline RawObject::RawObject(const RawObject& rhs)
{
	this->positions = rhs.positions;
	this->normals = rhs.normals;
	this->uvCoordinates = rhs.uvCoordinates;
	this->faces = rhs.faces;
	this->materialFiles = rhs.materialFiles;
	this->materialRanges = rhs.materialRanges;
}


#endif
