/*
 * Sprite.h
 *
 *  Created on: 2012.10.27
 *      Author: Martynas Šustavičius
 */

#ifndef SPRITE_H_
#define SPRITE_H_

#include "../Multiplatform/Ghost.h"
#include "Resource.h"
#include "Renderable.h"

class Texture;

class Sprite :
	public Resource,
	public Renderable {
public:
	Sprite(ServiceLocator* services);
	~Sprite();

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
	void resize();

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
	SIZE getNormalCount();

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
	 * @see Renderable.
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

	void addTexture(Texture* texture);

	Texture* getPointerToTexture();

	///**
	// * Sets index of sprite to render.
	// * If sprite contains no different animations,
	// * whole sprite image is used.
	// * @param index - index of animation.
	// */
	//void setAnimationIndex(UINT32 index);

	///**
	// * Sets number of separte images that compose one sprite.
	// * @param count - number of images.
	// * @note This method also generates new UV Buffer objects.
	// */
	//void setAnimationCount(UINT32 count);
	//void setTexture(TextureRGBA* image) { image_ = image; }
	//TextureRGBA* getTexture() { return image_; }

	///**
	// * Gets number of separe images that compose one sprite.
	// * @return Number of images.
	// */
	//UINT32 getAnimationCount();
	///**
	// * Returns sprite UV coordinate buffer index
	// * for specified sprite animation.
	// * @return UV coordinate buffer index.
	// */
	//GLuint getTCBO();
	//int getRedColor(float x, float y);
	//int getBlueColor(float x, float y);
	//int getGreenColor(float x, float y);
private:
	///** Current animation index. */
	//UINT32 animationIndex_;
	///** Animation count. */
	//UINT32 animationCount_;
	///** Animation texture coordinate index buffers. */
	//GLuint* uvBO_;
	/** Pointer to image. */
	Texture* image_;
	UINT32 cbo_;
};

#endif /* SPRITE_H_ */


