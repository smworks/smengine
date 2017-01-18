#ifndef VERTEXPROPERTIES_H_
#define OBJPROPERTIES_H_

#include "../../../ghost/Multiplatform/Ghost.h"
#include "../../../ghost/ModelData.h"

struct VertexProperties
{
	VertexProperties();

	UINT32 positionOffset;
	UINT32 normalOffset;
	UINT32 uvOffset;
	UINT32 vertexSize;
	ModelData::VertexType vertexType;

	bool hasNormals() const;
	bool hasUV() const;
};

inline VertexProperties::VertexProperties(): positionOffset(0), normalOffset(0), uvOffset(0), vertexSize(0), vertexType()
{
}

inline bool VertexProperties::hasNormals() const
{
	return normalOffset > 0;
}

inline bool VertexProperties::hasUV() const
{
	return uvOffset > 0;
}

#endif
