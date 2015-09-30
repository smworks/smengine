/*
 * Light.h
 *
 *  Created on: 2013 02 24
 *      Author: Martynas Šustavičius
 */

#ifndef LIGHT_H_
#define LIGHT_H_

#include "../Color.h"
#include "Resource.h"
#include "Renderable.h"

class Light : public Resource, public Renderable {
public:
	enum LightType {NONE, POINT_LIGHT, SPOT_LIGHT, GLOBAL_LIGHT}; 
public:
	Light(ServiceLocator* services);
	~Light();

	/**
	 * @see Resource
	 */
	void release();
	SIZE getSize();
	Resource::Type getType();
	bool isValid();
    
	/**
	 * @see Renderable
	 */
	SIZE getVertexCount() { return 0; }
	SIZE getTexture() { return 0; }
	SIZE getIBO() { return 0; }
	BoundingVolume* getBV() { return 0; }
	SIZE getCBO() { return 0; }
	SIZE getIndexCount() { return 0; }
	SIZE getRenderCount() { return 0; }
	void setRenderable(SIZE i) {}
	SIZE getVertexStride() { return 0; }
	int getPosOffset() { return 0; }
	int getNormalOffset() { return 0; }
	int getUVOffset() { return 0; }
	Shader* getDefaultShader() { return 0; }
    
	/**
	 * @return Reference to light color.
	 */
	Color& getColor();
protected:
	/**
	 * @see Resource
	 */
	bool create();
private:
	LightType lightType;
	Color color;
};

#endif