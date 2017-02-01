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
	explicit GUISurface(ServiceLocator* services);
	~GUISurface();


	bool create() override;
	void release() override;
	SIZE getSize() override;
	Type getType() override;
	bool isValid() override;
	void resize() override;

	SIZE getVertexCount() override;
	
	SIZE getTexture() override;
	SIZE getIBO() override;
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

	/**
	* Called whenever GUI element position or size changes.
	*/
	virtual void update() = 0;

	/**
	 * @return Pointer to surface texture.
	 */
	Texture* getPointerToTexture() const;

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
	float getMarginLeft() const;

	/**
	 * @return Bottom surface margin in pixels.
	 */
	float getMarginBottom() const;

	/**
	 * @return Right surface margin in pixels.
	 */
	float getMarginRight() const;

	/**
	 * @return Top surface margin in pixels.
	 */
	float getMarginTop() const;

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
	float getWidth() const;

	/**
	 * @param height - height of surface in pixels.
	 */
	void setHeight(float height);

	/**
	 * @return Height of surface in pixels.
	 */
	float getHeight() const;
	
	/**
	 * @return Absolute position on X axis on screen.
	 */
	float getPosX() const;

	/**
	 * @return Absolute position on Y axis on screen.
	 */
	float getPosY() const;

protected:
	/**
	 * Converts units into appropriate pixel values.
	 * @param units - unit value.
	 * @param type - type of the units.
	 * @return Units converted into pixels.
	 */
	static UINT32 convertUnitsToPixels(UINT32 units, UnitType type);

	/**
	 * Converts specified string to size in pixels.
	 * Supports formats like 2%, 2px.
	 * @param size - string with specified size.
	 * @param maxSize - relative size used when
	 * original size is specified in percents.
	 * @return Size converted to pixels.
	 */
	static float getSize(string size, float maxSize = 0);

    UINT32 surfaceId;
	UnitType widthSize, heightSize;
	float marginLeft, marginBottom, marginRight, marginTop;
	float posX, posY, width, height;
	bool shouldUpdate;
	Texture* textureBackground;
	UINT32 cbo;
};

#endif