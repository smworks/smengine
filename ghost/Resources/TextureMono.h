/*
 * Texture.h
 *
 *  Created on: 2012.06.30
 *      Author: Martynas Šustavičius
 */

#ifndef TextureMono_H_
#define TextureMono_H_

#include "../Multiplatform/Ghost.h"
#include "../Color.h"
#include "Texture.h"

class TextureMono : public Texture  {
public:
	TextureMono(ServiceLocator* services);
	~TextureMono();

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
	UINT8* createBuffer(UINT32 width, UINT32 height);

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
	void blend(
		Texture* src, bool alpha = true, UINT32 rowOffset = 0,
		UINT32 colOffset = 0);

	/**
	 * @see Texture
	 */
	bool commit();
private:
	/** Dirty region dimensions. */
	SIZE left_, top_, right_, bottom_;
	/** Handle to OpenGL resource. */
	UINT32 id_;
	/** Type of the texture. */
	Type type_;
};

#endif /* TextureMono_H_ */
