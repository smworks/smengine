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
	SIZE getVertexCount() override;
	SIZE getTexture() override;
	SIZE getIBO() override;
	SIZE getIndexOffset() override;
	BoundingVolume* getBV() override;
	SIZE getCBO() override;
	SIZE getIndexCount() override;
	SIZE getRenderCount() override;
	void setRenderable(SIZE i) override;
	SIZE getVertexStride() override;
	int getPosOffset() override;
	int getNormalOffset() override;
	int getUVOffset() override;
	Shader* getDefaultShader() override;

	ModelData* getData();
	void setTexture(Texture* texture, UINT32 index);
private:
	ModelData* modelData;
	UINT32 cbo;
	UINT32 ibo;
	SIZE currentPart;
};

#endif
