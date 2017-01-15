/*
 * BoundingVolume.h
 *
 *  Created on: 2012.07.07
 *      Author: Martynas Šustavičius
 */

#ifndef BOUNDINGVOLUME_H_
#define BOUNDINGVOLUME_H_

#include "Multiplatform/Ghost.h"
#include "Vec3.h"
#include "Camera.h"

class BoundingVolume {
public:
	enum Type {SPHERE, BOX, MESH, CAPSULE, TERRAIN, UNKNOWN};
	enum State {INSIDE, OUTSIDE, INTERSECT};
public:
	BoundingVolume() {}
	virtual ~BoundingVolume() {}

	/**
	 * Checks if bounding volume is inside the view frustum.
	 * @param camera - camera containing six view frustum planes.
	 * @param pos - position of object.
	 * @param scale - scaling of object.
	 */
	virtual State isInFrustum(Camera* camera, Vec3& pos, Vec3& scale) = 0;

	/**
	 * @return Type of bounding volume.
	 */
	virtual Type getType() = 0;

	/**
	 * @return New copy of original bounding volume.
	 */
	virtual BoundingVolume* copy() = 0;
};

#endif /* BOUNDINGVOLUME_H_ */
