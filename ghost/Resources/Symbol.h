/*
 * Symbol.h
 *
 *  Created on: 2013.12.19
 *      Author: Martynas Šustavičius
 */

#ifndef SYMBOL_H_
#define SYMBOL_H_

#include "../Multiplatform/Ghost.h"
#include "Resource.h"
#include "Renderable.h"

class Texture;

class Symbol : public Resource {
public:
	Symbol(ServiceLocator* services);
	~Symbol();

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
	Type getType();

	/**
	 * @see Resource
	 */
	void resize();

	/**
	 * @see Resource
	 */
	bool isValid();

	/**
	 * @param texture - texture to use as font.
	 */
	void setTexture(Texture* texture);

	/**
	 * @return Pointer to texture that represents symbol.
	 */
	Texture* getTexture();

	/**
	 * @param width - width of the symbol in pixels.
	 */
	void setWidth(UINT32 width);

	/**
	 * @return Width of symbol in pixels.
	 */
	UINT32 getWidth();

	/**
	 * @param height - height of the symbol in pixels.
	 */
	void setHeight(UINT32 height);

	/**
	 * @return Height of symbol in pixels.
	 */
	UINT32 getHeight();

	/**
	 * @param offsetX - texture offset on X axis.
	 */
	void setOffsetX(INT32 offset);

	/**
	 * @return Texture offset on X axis.
	 */
	INT32 getOffsetX();

	/**
	 * @param offsetY - texture offset on Y axis.
	 */
	void setOffsetY(INT32 offset);

	/**
	 * @return How many pixels symbol must be
	 * moved from the top when rendering.
	 */
	INT32 getOffsetY();

	/**
	 * @param advance - how many pixels symbol takes in width.
	 */
	void setAdvance(SIZE advance);

	/**
	 * @return How many pixels symbol takes in width.
	 */
	SIZE getAdvance();

	/**
	 * @param size - symbol font size.
	 */
	void setFontSize(SIZE size);

	/**
	 * @return Symbol font size.
	 */
	SIZE getFontSize();

	/**
	 * @return Array of uv coordinates of size 12 used to access
	 * texture on texture map.
	 */
	float* getUV();
private:
	Texture* texture_;
	UINT32 width_, height_;
	SIZE advance_;
	SIZE fontSize_;
	INT32 offsetX_, offsetY_;
};

#endif /* SYMBOL_H_ */


