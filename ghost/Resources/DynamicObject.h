/*
 * DynamicObject.h
 *
 *  Created on: 2012.12.27
 *      Author: MS
 */

#ifndef DYNAMICOBJECT_H_
#define DYNAMICOBJECT_H_

#include "../Multiplatform/Ghost.h"

#include "Resource.h"
#include "Renderable.h"
#include "Animatable.h"

class TextureRGBA;

#define DEFAULT_FRAME_TIME 200.0f // 0.2 second.

class DynamicObject :
	public Resource,
	public Renderable,
	public Animatable
{
public:
	DynamicObject(ServiceLocator* services);
	~DynamicObject();

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

	/**
	 * @see Animatable
	 */
	void nextFrame(double time);

	/**
	 * @see Animatable
	 */
	void setAnimation(string name);
protected:
	/**
	 * @see Resource
	 */
	bool create();
private:
	struct Animation {
		Animation(UINT32 offset) :
			offset_(offset),
			frames_(1),
			frameDuration_(DEFAULT_FRAME_TIME),
			texture_(0)
		{}
		UINT32 offset_;
		UINT32 frames_;
		double frameDuration_;
		UINT32 texture_;
	};
private:
	vector<float> vertices_;
	vector<UINT16> normals_;
	unordered_map<string, Animation> animations_;
	UINT32 vertexCount_; // Number of floats (vertices * 3) in frame.
	UINT32 frameCount_;

	Animation* animation_;
	SIZE frame_; // Rendered frame.
	UINT32 cbo_;
	double time_;

	string textureName_;
	vector<TextureRGBA*> textures_;
	vector<float> uv_;
	
	UINT32 vertexStride_;
	UINT32 posOffset_;
	UINT32 normalOffset_;
	UINT32 uvOffset_;
};

#endif