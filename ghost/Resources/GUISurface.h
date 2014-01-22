/*
 * GUISurface.h
 *
 *  Created on: 2013.07.03
 *      Author: Martynas Šustavičius
 */

#ifndef GUISURFACE_H_
#define GUISURFACE_H_

#include "../Multiplatform/Ghost.h"
#include "Resource.h"
#include "Renderable.h"
class Texture;

#define SURFACE_BCKG_COLOR "surface_bckg_color"
#define SURFACE_BCKG_IMAGE "surface_bckg_image"

class GUISurface :
	public Resource,
	public Renderable {
public:
	enum UnitType {
		DEFAULT, PERCENT, FILL, WRAP
	};
public:
	GUISurface(ServiceLocator* services);
	~GUISurface();

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
	virtual Resource::Type getType() = 0;

	/**
	 * @see Resource
	 */
	bool isValid();

	/**
	 * @see Resource
	 */
	virtual void resize();

	/**
	 * Called whenever GUI element position or size changes.
	 */
	virtual void update() = 0;

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
	virtual Shader* getDefaultShader();

	/**
	 * @return Pointer to surface texture.
	 */
	Texture* getPointerToTexture();

	/**
	 * Specify surface margins.
	 * @param left - offset to the left.
	 * @param bottom - offset to the bottom.
	 * @param right - offset to the right.
	 * @param top - offset to the top.
	 */
	void setMargins(int left, int bottom, int right = 0, int top = 0);

	/**
	 * @return Left surface margin in pixels.
	 */
	int getMarginLeft();

	/**
	 * @return Bottom surface margin in pixels.
	 */
	int getMarginBottom();

	/**
	 * @return Right surface margin in pixels.
	 */
	int getMarginRight();

	/**
	 * @return Top surface margin in pixels.
	 */
	int getMarginTop();

	/**
	 * Set relative view position.
	 * @param x - x coordinate.
	 * @param y - y coordinate.
	 */
	void setPos(int x, int y);

	/**
	 * Specify what to use for background.
	 * @param background - string containing either color
	 * value or texture name.
	 */
	void setBackground(string background);

	/**
	 * Method is called to handle all actions
	 * when mouse or touch is over this specific surface.
	 */
	virtual void hasFocus() = 0;

	/**
	 * @param width - width of surface in pixels.
	 */
	void setWidth(SIZE width);

	/**
	 * @return Width of surface in pixels.
	 */
	SIZE getWidth();

	/**
	 * @param height - height of surface in pixels.
	 */
	void setHeight(SIZE height);

	/**
	 * @return Height of surface in pixels.
	 */
	SIZE getHeight();
protected:
	/**
	 * Converts units into appropriate pixel values.
	 * @param units - unit value.
	 * @param type - type of the units.
	 * @return Units converted into pixels.
	 */
	UINT32 convertUnitsToPixels(UINT32 units, UnitType type);

	/**
	 * Converts specified string to size in pixels.
	 * Supports formats like 2%, 2px.
	 * @param size - string with specified size.
	 * @param maxSize - relative size used when
	 * original size is specified in percents.
	 * @return Size converted to pixels.
	 */
	int getSize(string size, int maxSize = 0);

	/**
	 * @see Resource
	 */
	virtual bool create();
protected:
    UINT32 surfaceId_;
	string textureName_;
	UnitType widthSize_, heightSize_;
	int marginLeft_, marginBottom_, marginRight_, marginTop_;
	int posX_, posY_;
	bool update_;
	SIZE width_, height_;
	Texture* textureBackground_;
};

#endif