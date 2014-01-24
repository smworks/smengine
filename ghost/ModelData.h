/*
 * ModelData.h
 *
 *  Created on: 2014.01.07
 *      Author: Martynas Šustavičius
 */

#ifndef MODELDATA_H_
#define MODELDATA_H_

#include "Multiplatform/Ghost.h"
#include "Resources/Vertex.h"
#include "Color.h"

class BoundingVolume;
class Texture;

class ModelData {
public:
	enum VertexType {PNT, PN, PT, P, VERTEX_TYPE_COUNT};
	struct Material {
		Material() :
			texture_(0),
			specIntensity_(0.0f),
			transparency_(0.0f)
		{}
		string name_;
		Texture* texture_;
		Color ambient_;
		Color diffuse_;
		Color specular_;
		float specIntensity_;
		float transparency_;
	};
	struct Part {
		Part() : material_(0), offset_(0), indexCount_(0), bv_(0) {}
		SIZE material_;
		SIZE offset_;
		SIZE indexCount_;
		BoundingVolume* bv_;
	};
public:
	ModelData();
	~ModelData();
	void setVertices(VertexType type, UINT8* vertices, SIZE vertexCount);
	SIZE getVertexCount();
	VertexType getVertexType();
	UINT32 getVertexStride();
	UINT32 getPosOffset();
	UINT32 getNormalOffset();
	UINT32 getUVOffset();
	void* getVertices();
	bool hasNormals();
	bool hasUV();
	void setIndices(int type, void* indices, SIZE indexCount);
	SIZE getIndexCount();
	int getIndexType();
	UINT32 getIndexStride();
	UINT16* getIndicesShort();
	UINT32* getIndicesInt();
	void setBoundingVolume(BoundingVolume* bv);
	BoundingVolume* getBoundingVolume();
	vector<Material>& getMaterials();
	vector<Part>& getParts();
private:
	VertexType vertexType_;
	UINT32 vertexStride_, posOffset_, normalOffset_, uvOffset_;
	SIZE vertexCount_;
	UINT8* vertices_;
	int indexType_;
	union Indices {
		UINT16* indicesShort;
		UINT32* indicesInt;
	} indices_;
	SIZE indexCount_;
	BoundingVolume* boundingVolume_;
	vector<Material> materials_;
	vector<Part> parts_;
};

#endif