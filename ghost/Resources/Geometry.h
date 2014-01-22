/*
 * Geometry.h
 *
 *  Created on: 2013.08.14
 *      Author: Martynas Šustavičius
 */

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "../Multiplatform/Ghost.h"
#include "Resource.h"

class Geometry : public Resource {
public:
	Geometry(ServiceLocator* services);
	~Geometry();

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
	 * Releases and creates resource again.
	 */
	void reset();
protected:
	/**
	 * @see Resource
	 */
	bool create();
protected:
	UINT32 id_;
};

#endif