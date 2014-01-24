/*
 * Renderable.h
 *
 *  Created on: 2012.12.23
 *      Author: MS
 *
 * All classes that are rendered on screen must
 * implement this interface.
 */

#ifndef RENDERABLE_H_
#define RENDERABLE_H_

#include "../Multiplatform/Ghost.h"
#include "../Color.h"
#include "Shader.h"

class BoundingVolume;
class Shader;

class Renderable {
public:
	enum RenderType {
		RENDER_TYPE_POINTS,
		RENDER_TYPE_LINES,
		RENDER_TYPE_TRIANGLES,
		RENDER_TYPE_TRIANGLE_STRIP,
		RENDER_TYPE_TRIANGLE_FAN,
		RENDER_TYPE_QUADS
	};
	enum WindingType {
		WINDING_TYPE_NONE,
		WINDING_TYPE_CW,
		WINDING_TYPE_CCW
	};
	enum IndexType {
		INDEX_TYPE_USHORT,
		INDEX_TYPE_UINT,
		INDEX_TYPE_COUNT
	};
public:
	Renderable() :
		specularity_(0.0f),
		transparency_(1.0f),
		renderType_(RENDER_TYPE_TRIANGLES),
		pointSize_(1.0f),
		lineWidth_(1.0f),
		windingType_(WINDING_TYPE_CCW),
		cullFace_(true),
		blending_(false),
		indexType_(INDEX_TYPE_USHORT),
		shader_(0)
	{}
	virtual ~Renderable() {}

	/**
	 * @return Number of vertices.
	 */
	virtual SIZE getVertexCount() = 0;

	/**
	 * @return Texture buffer id.
	 * Zero indicates failure.
	 */
	virtual SIZE getTexture() = 0;

	/**
	 * @return Index buffer object id.
	 * Zero indicates failure.
	 */
	virtual SIZE getIBO() = 0;

	/**
	 * @return Offset in IBO.
	 */
	virtual SIZE getIndexOffset() { return 0; }

	/**
	 * @return Bounding box for current model part.
	 */
	virtual BoundingVolume* getBV() = 0;

	/**
	 * @return Combined buffer object.
	 */
	virtual SIZE getCBO() = 0;

	/**
	 * @return Number of indices in
	 * current index buffer.
	 */
	virtual SIZE getIndexCount() = 0;

	/**
	 * @return Number of object
	 * parts, that need to be rendered
	 * separately.
	 */
	virtual SIZE getRenderCount() = 0;

	/**
	 * @param i - index of current object
	 * part, that will be rendered.
	 * Usually this will change
	 * the return values of other
	 * Renderable interface methods.
	 */
	virtual void setRenderable(SIZE i) = 0;

	/**
	 * @return Size of vertex structure.
	 */
	virtual SIZE getVertexStride() = 0;

	/**
	 * @return Position vector offset in
	 * vertex structure.
	 */
	virtual int getPosOffset() = 0;

	/**
	 * @return Normal vector offset in
	 * vertex structure.
	 * -1 means that normals are not used for
	 * renderable.
	 */
	virtual int getNormalOffset() = 0;

	/**
	 * @return Texture coordinate offset in
	 * vertex structure.
	 * -1 means that coordinates are not used for
	 * renderable.
	 */
	virtual int getUVOffset() = 0;

	/**
	 * @return Default shader used
	 * for this specific renderable
	 * object.
	 */
	virtual Shader* getDefaultShader() = 0;

	/**
	 * @return Shader used by this
	 * renderable object.
	 */
	Shader* getShader() {
		if (shader_ == 0) {
			shader_ = getDefaultShader();
		}
		return shader_;
	}

	/**
	 * Sets shader that will be used
	 * to render this renderable object.
	 * @param shader - shader used to
	 * render renderable object.
	 */
	void setShader(Shader* shader) {
		shader_ = shader;
	}

	/**
	 * Returns color that is used when
	 * there is no real light
	 * on the object, so it
	 * would still be visible.
	 * @return Ambient color.
	 */
	virtual Color& getAmbient() {
		return ambient_;
	}

	/**
	 * @param color - specified color.
	 */
	virtual void setAmbient(Color color) {
		ambient_ = color;
	}

	/**
	 * Returns color that is used when
	 * object is visible, because
	 * real light shines on it.
	 * @return Diffuse color.
	 */
	virtual Color& getDiffuse() {
		return diffuse_;
	}

	/**
	 * @param color - specified color.
	 */
	virtual void setDiffuse(Color color) {
		diffuse_ = color;
	}

	/**
	 * Returns color that is used
	 * for shiny spot that appears
	 * on reflective objects.
	 * @return Specular color.
	 */
	virtual Color& getSpecular() {
		return specular_;
	}

	/**
	 * @param color - specified color.
	 */
	virtual void setSpecular(Color color) {
		specular_ = color;
	}

	/**
	 * @return Specular color intensity.
	 */
	virtual float getSpecularity() {
		return specularity_;
	}

	/**
	 * @param value - specular color intensity.
	 */
	virtual void setSpecularity(float value) {
		specularity_ = value;
	}

	/**
	 * @return Renderable object transparency.
	 */
	virtual float getTransparency() {
		return transparency_;
	}

	/**
	 * @param value - transparency.
	 * 1.0f - object is opaque.
	 * 0.0f - object is fully transparent.
	 */
	virtual void setTransparency(float value) {
		transparency_ = value;
	}

	/**
	 * @return Renderable objects
	 * render type.
	 */
	virtual RenderType getRenderType() {
		return renderType_;
	}

	/**
	 * @param type - object render type.
	 */
	virtual void setRenderType(RenderType type) {
		renderType_ = type;
	}

	/**
	 * @return Size of point.
	 */
	virtual float getPointSize() {
		return pointSize_;
	}

	/**
	 * Only used when render type
	 * is set to points.
	 * @param size - size of point.
	 */
	virtual void setPointSize(float size) {
		pointSize_ = size;
	}

	/**
	 * @return Line width.
	 */
	virtual float getLineWidth() {
		return lineWidth_;
	}

	/**
	 * Only used when render type
	 * is set to lines.
	 * @param width - width of the line.
	 */
	virtual void setLineWidth(float width) {
		lineWidth_ = width;
	}

	/**
	 * Specifies in which order,
	 * drawn triangles are visible.
	 * @param type - winding type.
	 */
	virtual void setWindingType(WindingType type) {
		windingType_ = type;
	}

	/**
	 * @return Type of winding to perform.
	 */
	virtual WindingType getWindingType() {
		return windingType_;
	}

	/**
	 * Specify whether to render
	 * both sides of the triangle,
	 * or only one.
	 * @param state - true will render
	 * only one side, false - both.
	 */
	virtual void setCullFace(bool state) {
		cullFace_ = state;
	}

	/**
	 * @return Cull face state.
	 */
	virtual bool getCullFace() {
		return cullFace_;
	}

	/**
	 * Method used to enable blending.
	 * Blending is used for transparency.
	 * @param state - state true will enable.
	 */
	virtual void setBlending(bool state) {
		blending_ = state;
	}

	/**
	 * @return Blending state.
	 */
	virtual bool getBlending() {
		return blending_;
	}

	/**
	 * @param type - type of variables
	 * used in index buffer.
	 */
	virtual void setIndexType(IndexType type) {
		indexType_ = type;
	}

	/**
	 * @return Type of index buffer data.
	 */
	virtual IndexType getIndexType() {
		return indexType_;
	}
private:
	Color ambient_;
	Color diffuse_;
	Color specular_;
	float specularity_;
	float transparency_;
	RenderType renderType_;
	float pointSize_;
	float lineWidth_;
	WindingType windingType_;
	bool cullFace_;
	bool blending_;
	IndexType indexType_;
	Shader* shader_;
};

#endif /* RENDERABLE_H_ */