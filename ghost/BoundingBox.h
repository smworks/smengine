/*
 * BoundingBox.h
 *
 *  Created on: 2013.02.08
 *      Author: Martynas Šustavičius
 */

#ifndef BOUNDINGBOX_H_
#define BOUNDINGBOX_H_

#include "BoundingVolume.h"

class BoundingBox : public BoundingVolume {
public:
	BoundingBox();

	/**
	 * Creates bounding box with specified sizes.
	 * Center of box is (0, 0, 0).
	 * @param sizes - vector with box sizes for each axis.
	 */
	BoundingBox(const Vec3& sizes);

	~BoundingBox();

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
	 * @param sizes - vector containing box dimensions.
	 */
	void setSizes(Vec3& sizes);

	/**
	 * @return Vector with box sizes for each axis.
	 */
	Vec3 getSizes(Vec3 scale);
private:
	Vec3 sizes;
	Vec3 corners[8];
};

#endif /* BOUNDINGSPHERE_H_ */
