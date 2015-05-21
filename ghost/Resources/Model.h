/*
 * Model.h
 *
 *  Created on: 2012.12.23
 *      Author: Martynas Šustavičius
 */

#ifndef Model_H_
#define Model_H_

#include "Resource.h"
#include "Renderable.h"
class BoundingVolume;
class TextureRGBA;
class Vec3;
class ModelData;
class Texture;

#define GHOST_TERRAIN_HEIGHT_MAP "heightMap"

class Model :
	public Resource,
	public Renderable {
public:
	Model(ServiceLocator* services);
	~Model();

	/**
	 * @see Resource
	 */
	bool create();
	void release();
	SIZE getSize();
	Resource::Type getType();
	bool isValid();

	/**
	 * @see Renderable
	 */
	SIZE getVertexCount();
	SIZE getTexture();
	SIZE getIBO();
	SIZE getIndexOffset();
	BoundingVolume* getBV();
	SIZE getCBO();
	SIZE getIndexCount();
	SIZE getRenderCount();
	void setRenderable(SIZE i);
	SIZE getVertexStride();
	int getPosOffset();
	int getNormalOffset();
	int getUVOffset();
	Shader* getDefaultShader();

	ModelData* getData();
	void setTexture(Texture* texture, UINT32 index);
private:
	ModelData* modelData;
	UINT32 cbo;
	UINT32 ibo;
	SIZE currentPart;
};

#endif
