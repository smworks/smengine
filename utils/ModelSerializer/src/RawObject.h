#ifndef RAWOBJECT_H_
#define RAWOBJECT_H_

#include "../../../ghost/Multiplatform/Ghost.h"
#include "../../../ghost/Vec3.h"
#include "ObjProperties.h"

struct RawObject
{
	RawObject() : minVertex(Vec3(FLT_MAX)), maxVertex(Vec3(FLT_MIN)),
	              radius(0.0f)
	{
	}

	Vec3 minVertex;
	Vec3 maxVertex;
	float radius;
};

#endif
