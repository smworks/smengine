/*
 * Texture.h
 *
 *  Created on: 2012.07.24
 *      Author: Martynas Šustavičius
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "../Multiplatform/Ghost.h"
#include "../Color.h"
#include "Resource.h"

class Texture : public Resource  {
public:
	enum Type {MONO, RGB, RGBA};
	static const string ATTR_WRAP_U;
	static const string ATTR_WRAP_V;
	static const string VAL_CLAMP;
	static const string VAL_REPEAT;
	static const string ATTR_TYPE;
	static const string VAL_MONO;
	static const string VAL_RGBA;
	static const string VAL_RGB;
public:
	/**
	 * Custom constructor used to initialize
	 * texture object manually.
	 */
	Texture(ServiceLocator* services);
	~Texture();

	/**
	 * To use this method texture object
	 * must be created by constructor that
	 * accepts service locator object as
	 * parameter.
	 * Creates empty texture.
	 * @param width - width of the
	 * texture, in pixels.
	 * @param height - height of the
	 * texture, in pixels.
	 * @return True on success.
	 */
	virtual bool create(UINT32 width, UINT32 height) = 0;

	/**
	 * @see Resource
	 */
	virtual bool create() = 0;

	/**
	 * @see Resource
	 */
	virtual void release() = 0;

	/**
	 * @see Resource
	 */
	virtual SIZE getSize() = 0;

	/**
	 * @see Resource
	 */
	virtual Resource::Type getType() = 0;

	/**
	 * @see Resource
	 */
	virtual void resize() = 0;

	/**
	 * @see Resource
	 */
	virtual bool isValid() = 0;

	/**
	 * @return Id indicating texture data
	 * in OpenGL memory.
	 */
	virtual UINT32 getId() = 0;

	/**
	 * @return Id of vertex position and UV buffer
	 * that represents current texture.
	 */
	virtual UINT32 getCBO() = 0;

	/**
	 * @return Width of image in pixels.
	 */
	virtual UINT32 getWidth() = 0;

	/**
	 * @return Height of image in pixels.
	 */
	virtual UINT32 getHeight() = 0;

	/**
	 * @return Pointer to raw texture data.
	 */
	virtual UINT8* getRawData() = 0;

	/**
	 * Resize texture.
	 * @param width - new width in pixels.
	 * @param height - new height in pixels.
	 */
	virtual void resize(UINT32 width, UINT32 height) = 0;

	/**
	 * Changes pixel in original image. Requires call
	 * to commit() method to apply changes.
	 * @param color - byte array where each byte
	 * represents color component. For TextureMono it will only be
	 * one byte, for TextureRGBA it will be four and so on...
	 * @param row - row in which pixel is located.
	 * @param col - column in which pixel is located.
	 */
	virtual void setPixel(UINT8* color, SIZE row, SIZE col) = 0;

	/**
	 * Returned offset points to original image
	 * color buffer, so care must be taken not to
	 * modify it unintentionally.
	 * @param row - row in which pixel is located.
	 * @param clumn - column in which pixel is located.
	 * @return Color byte array. Will be only one for TextureMono,
	 * four for Texture RGBA and so on...
	 */
	virtual UINT8* getPixel(SIZE row, SIZE col) = 0;

	/**
	 * Draws line of specified color on the texture.
	 * Algorithm used: Bresenham's Line Algorithm.
	 * @param startRow - row where drawing starts.
	 * @param startCol - column where drawing starts.
	 * @param endRow - row where drawing ends.
	 * @param endCol - column where dreawing ends.
	 * @param color - byte array where each byte
	 * represents color component. For TextureMono it will only be
	 * one byte, for TextureRGBA it will be four and so on...
	 */
	void line(UINT32 startRow, UINT32 startCol,
		UINT32 endRow, UINT32 endCol, UINT8* color);

	/**
	 * Draws rectangle of specified color on the texture.
	 * Starts drawing from lower left corner.
	 * @param startRow - row where drawing starts.
	 * @param startCol - column where drawing starts.
	 * @param width - rectangle width.
	 * @param height - rectangle height.
	 * @param color - byte array where each byte
	 * represents color component. For TextureMono it will only be
	 * one byte, for TextureRGBA it will be four and so on...
	 */
	void rectangle(UINT32 startRow, UINT32 startCol,
		UINT32 width, UINT32 height, UINT8* color);

	/**
	 * Draws circle of specified color on the texture.
	 * Alghorithm used: Bresenham Algorithm for a full circle.
	 * @param row - row that crosses the center of the circle.
	 * @param col - column that crosses the center of the circle.
	 * @param color - byte array where each byte
	 * represents color component. For TextureMono it will only be
	 * one byte, for TextureRGBA it will be four and so on...
	 */
	void circle(UINT32 row, UINT32 col, UINT32 radius,
		UINT8* color);

	/**
	 * Draws filled circle of specified color on the texture.
	 * Alghorithm used: Bresenham Algorithm for a full circle.
	 * @param row - row that crosses the center of the circle.
	 * @param col - column that crosses the center of the circle.
	 * @param color - byte array where each byte
	 * represents color component. For TextureMono it will only be
	 * one byte, for TextureRGBA it will be four and so on...
	 */
	void filledCircle(UINT32 row, UINT32 col, UINT32 radius,
		UINT8* color);

	/**
	 * @param src - source texture that will
	 * be put over current texture.
	 * @param alpha - if set to true, blending will go according to
	 * alpha values, otherwise source texture colors will be put
	 * over current texture.
	 * @param rowOffset - row offset on original texture.
	 * @param colOffset - column offset on original texture.
	 */
	virtual void blend(
		Texture* src, bool alpha = true, UINT32 rowOffset = 0,
		UINT32 colOffset = 0) = 0;

	/**
	 * When data on texture is changed using
	 * setPixel() and getPixel() methods,
	 * texture needs to be reloaded,
	 * so it could be used in OpenGL.
	 * This method does exactly that.
	 * @return True on success.
	 */
	virtual bool commit() = 0;
};

#endif
