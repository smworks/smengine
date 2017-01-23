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
	explicit Model(ServiceLocator* services);
	~Model();

	/**
	 * @see Resource
	 */
	bool create() override;
	void release() override;
	SIZE getSize() override;
	Resource::Type getType() override;
	bool isValid() override;

	/**
	 * @see Renderable
	 */
	SIZE getVertexCount();
	SIZE getTexture();
	SIZE getIBO();
	SIZE getIndexOffset();
	BoundingVolume* getBV() override;
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
