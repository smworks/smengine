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

	/**
	 * @see Resource
	 */
	void release();

	/**
	 * @see Resource
	 */
	SIZE getSize();

	/**
	 * @see Resource
	 */
	Resource::Type getType();

	/**
	 * @see Resource
	 */
	bool isValid();

	/**
	 * @see Renderable
	 */
	SIZE getVertexCount();

	/**
	 * @see Renderable
	 */
	SIZE getTexture();

	/**
	 * @see Renderable
	 */
	SIZE getIBO();

	/**
	 * @see Renderable.
	 */
	BoundingVolume* getBV();

	/**
	 * @see Renderable
	 */
	SIZE getCBO();

	/**
	 * @see Renderable
	 */
	SIZE getIndexCount();

	/**
	 * @see Renderable
	 */
	SIZE getRenderCount();

	/**
	 * @see Renderable
	 */
	void setRenderable(SIZE i);

	/**
	 * @see Renderable
	 */
	SIZE getVertexStride();

	/**
	 * @see Renderable
	 */
	int getPosOffset();

	/**
	 * @see Renderable
	 */
	int getNormalOffset();

	/**
	 * @see Renderable
	 */
	int getUVOffset();

	/**
	 * @see Renderable
	 */
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
	SIZE currentPart_;
};

#endif
