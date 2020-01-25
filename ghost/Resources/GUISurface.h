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
protected:
    UINT32 surfaceId;
	bool shouldUpdate;
	Texture* textureBackground;
	UINT32 cbo;
	Color backgroundColor;
};

#endif