/*
 * BoundingSphere.h
 *
 *  Created on: 2012.07.07
 *      Author: Martynas Šustavičius
 */

#ifndef BOUNDINGSPHERE_H_
#define BOUNDINGSPHERE_H_

#include "Multiplatform/Ghost.h"
#include "BoundingVolume.h"

class BoundingSphere : public BoundingVolume {
public:
	BoundingSphere();

	/**
	 * Creates bounding sphere with specified radius.
	 * @param radius - radius of the sphere.
	 * coordinate space.
	 */
	BoundingSphere(float radius);

	~BoundingSphere();

	/**
	 * @see BoundingVolume
	 */
	State isInFrustum(Camera* camera, Vec3& pos, Vec3& scale);

	/**
	 * @see BoundingVolume
	 */
	Type getType();

	/**
	 * @see BoundingVolume
	 */
	BoundingVolume* copy();

	/**
	 * @param radius - new radius.
	 */
	void setRadius(float radius);

	/**
	 * @param scale - scale vector that contains sphere
	 * scaling.
	 * Maximum scale component will be selected.
	 * @return Sphere radius.
	 */
	float getRadius(Vec3 scale) const;
private:
	float radius_;
};

#endif /* BOUNDINGSPHERE_H_ */
