#ifndef OBJPROPERTIES_H_
#define OBJPROPERTIES_H_

#include "../../../ghost/Multiplatform/Ghost.h"

struct ObjProperties
{
	ObjProperties() : vertexCount(0), normalCount(0),
	                  uvCount(0), faceCount(0), maxVertexPos(0.0f)
	{
	}

	UINT64 vertexCount;
	UINT64 normalCount;
	UINT64 uvCount;
	UINT64 faceCount;
	float maxVertexPos;
};

#endif
