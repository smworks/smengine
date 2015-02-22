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
	virtual bool create();

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
	void setMargins(float left, float top, float right = 0.0f, float bottom = 0.0f);

	/**
	 * @return Left surface margin in pixels.
	 */
	float getMarginLeft();

	/**
	 * @return Bottom surface margin in pixels.
	 */
	float getMarginBottom();

	/**
	 * @return Right surface margin in pixels.
	 */
	float getMarginRight();

	/**
	 * @return Top surface margin in pixels.
	 */
	float getMarginTop();

	/**
	 * Specify what to use for background.
	 * @param background - string containing either color
	 * value or texture name.
	 */
	void setBackground(string background);

	/**
	 * Return background image name or color.
	 */
	string getBackground();

	/**
	 * Method is called to handle all actions
	 * when mouse or touch is over this specific surface.
	 */
	virtual void hasFocus() = 0;

	/**
	 * @param width - width of surface in pixels.
	 */
	void setWidth(float width);

	/**
	 * @return Width of surface in pixels.
	 */
	float getWidth();

	/**
	 * @param height - height of surface in pixels.
	 */
	void setHeight(float height);

	/**
	 * @return Height of surface in pixels.
	 */
	float getHeight();
	
	/**
	 * @return Absolute position on X axis on screen.
	 */
	float getPosX();

	/**
	 * @return Absolute position on Y axis on screen.
	 */
	float getPosY();

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
	float getSize(string size, float maxSize = 0);
protected:
    UINT32 surfaceId_;
	UnitType widthSize_, heightSize_;
	float marginLeft_, marginBottom_, marginRight_, marginTop_;
	float posX_, posY_, width_, height_;
	bool update_;
	Texture* textureBackground_;
	UINT32 cbo_;
};

#endif