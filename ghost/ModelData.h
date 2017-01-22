/*
 * ModelData.h
 *
 *  Created on: 2014.01.07
 *      Author: Martynas Šustavičius
 */

#ifndef MODELDATA_H_
#define MODELDATA_H_

#include "Multiplatform/Ghost.h"
#include "Color.h"
#include "Resources/Renderable.h"

class BoundingVolume;
class Texture;
class ServiceLocator;

class ModelData {
public:
	enum VertexType {PNT, PN, PT, P, VERTEX_TYPE_COUNT};
	struct Material {
		Material() :
			specIntensity_(0.0f),
			transparency_(0.0f),
			texture_(nullptr)
		{}
		char name[256];
		Color ambient_;
		Color diffuse_;
		Color specular_;
		float specIntensity_;
		float transparency_;
		Texture* texture_;
		void setName(string name);
	};

	struct Part {
		Part() : material_(0), offset_(0), indexCount_(0), bv(nullptr) {}
		SIZE material_;
		SIZE offset_;
		SIZE indexCount_;
		BoundingVolume* bv;
	};

	ModelData();
	ModelData(const ModelData& rhs);
	~ModelData();
	void setVertices(VertexType type, UINT8* vertices, SIZE vertexCount);
	SIZE getVertexCount() const;
	VertexType getVertexType() const;
	UINT32 getVertexStride() const;
	UINT32 getPosOffset() const;
	UINT32 getNormalOffset() const;
	UINT32 getUVOffset() const;
	void* getVertices() const;
	bool hasNormals() const;
	bool hasUV() const;
	void setIndices(UINT8* indices, SIZE indexCount, Renderable::IndexType type);
	SIZE getIndexCount() const;
	Renderable::IndexType getIndexType() const;
	SIZE getIndexSize() const;
	UINT8* getIndices() const;
	void setBoundingVolume(BoundingVolume* bv);
	BoundingVolume* getBoundingVolume() const;
	vector<Material>& getMaterials();
	void setParts(vector<Part> parts);
	vector<Part>& getParts();
	void setCullFace(bool state);
	bool areFacesCulled() const;
	void serializeToFile(string path);
	void deserialize(ServiceLocator* sl, const char* binary);
	
private:
	UINT32 vertexStride;
	UINT32 posOffset;
	UINT32 normalOffset;
	UINT32 uvOffset;
	VertexType vertexType;
	SIZE vertexCount;
	UINT8* vertices;
	Renderable::IndexType indexType;
	SIZE indexCount;
	UINT8* indices;
	vector<Material> materials;
	vector<Part> parts;
	bool facesCulled;
	BoundingVolume* boundingVolume;
};

#endif