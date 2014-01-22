/*
 * ITexture.h
 *
 *  Created on: 2012.06.30
 *      Author: MS
 */

#ifndef CUBEMAP_H_
#define CUBEMAP_H_


#include "../Multiplatform/Ghost.h"
#include "Resource.h"

class CubeMap : public Resource {
public:
	CubeMap(ServiceLocator* services);
	~CubeMap();

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
	 * @return Id indicating texture data
	 * in OpenGL memory.
	 */
	UINT32 getId();
protected:
	/**
	 * @see Resource
	 */
	bool create();
private:
	UINT32 id_; // Handle to OpenGL resource.
};

#endif /* CUBEMAP_H_ */
