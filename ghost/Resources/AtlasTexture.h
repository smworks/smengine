/*
 * AtlasTexture.h
 *
 *  Created on: 2012.07.24
 *      Author: Martynas Šustavičius
 */

#ifndef ATLASTEXTURE_H_
#define ATLASTEXTURE_H_

#include "../Multiplatform/Ghost.h"
#include "../Color.h"
#include "Texture.h"

class AtlasTexture : public Texture  {
public:
	AtlasTexture(ServiceLocator* services, int type);
	~AtlasTexture();

	/**
	 * @see Texture
	 */
	UINT8* createBuffer(UINT32 width, UINT32 height);

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
	 * @see Texture
	 */
	UINT32 getId();

	/**
	 * @see Texture
	 */
	UINT32 getCBO();

	/**
	 * @see Texture
	 */
	UINT32 getWidth();

	/**
	 * @see Texture
	 */
	UINT32 getHeight();

	/**
	 * @see Texture
	 */
	UINT8* getRawData();

	/**
	 * @see Texture
	 */
	void resize(UINT32 width, UINT32 height);

	/**
	 * @see Texture
	 */
	void setPixel(UINT8* color, SIZE row, SIZE col);

	/**
	 * @see Texture
	 */
	UINT8* getPixel(SIZE row, SIZE col);

	/**
	 * @see Texture
	 */
	void blend(Texture* src, bool alpha = true, UINT32 rowOffset = 0, UINT32 colOffset = 0);

	/**
	 * @see Texture
	 */
	bool commit();

	/**
	 * @return Array of uv coordinates of size 12 used to access
	 * texture on texture map.
	 */
	float* getUV();
protected:
	/**
	 * @see Resource
	 */
	bool create();
private:
    /** Id of the texture in atlas. */
    UINT32 atlasId_;
	/** Type of texture. (Texture::Type) */
	int type_;
};

#endif
