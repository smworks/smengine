/*
 * StaticObject.h
 *
 *  Created on: 2012.12.23
 *      Author: Martynas Šustavičius
 */

#ifndef STATICOBJECT_H_
#define STATICOBJECT_H_

#include "Resource.h"
#include "Renderable.h"
class BoundingVolume;
class TextureRGBA;
class Vec3;
class ModelData;

#define GHOST_TERRAIN_HEIGHT_MAP "heightMap"

class StaticObject :
	public Resource,
	public Renderable {
public:
	StaticObject(ServiceLocator* services);
	~StaticObject();

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

	/**
	 * @return Pointer to bounding volume.
	 */
	BoundingVolume* getBoundingVolume();

	/**
	 * @return Model object that contains geometry,
	 * bounding volume and materials.
	 */
	ModelData* getModel();
private:
	bool createModel();
	bool createTerrain();
	bool createPlane();
	bool createSphere();
	bool createShape();
	bool createWater();
private:
	ModelData* modelData_;
	UINT32 cbo_; // Combined buffer object.
	UINT32 ibo_; // Index buffer object.
	SIZE currentPart_;
};

#endif
