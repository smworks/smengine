/*
 * BoundingSphere.cpp
 *
 *  Created on: 2012.07.07
 *      Author: Martynas Šustavičius
 */

#include "BoundingSphere.h"

BoundingSphere::BoundingSphere() : radius_(0.0f) {}
BoundingSphere::BoundingSphere(float radius) : radius_(radius) {}
BoundingSphere::~BoundingSphere() {}

BoundingVolume::State BoundingSphere::isInFrustum(
	Camera* camera, Vec3& pos, Vec3& scale) {
	float dist;
	float x = pos.getX();
	float y = pos.getY();
	float z = pos.getZ();
	for (int i = 0; i < 6; i++) {
		Camera::Plane& p = camera->getPlane(i);
		dist = p.a * x + p.b * y + p.c * z + p.d;
		if (dist + getRadius(scale) < 0.0f) {
			return BoundingVolume::OUTSIDE;
		}
	}
	return BoundingVolume::INSIDE;
}

BoundingVolume::Type BoundingSphere::getType() {
	return SPHERE;
}

BoundingVolume* BoundingSphere::copy() {
	BoundingSphere* sphere = NEW BoundingSphere();
	*sphere = *this;
	return sphere;
}

void BoundingSphere::setRadius(float radius) {
	radius_ = radius;
}

float BoundingSphere::getRadius(Vec3 scale = Vec3(1.0f)) const
{
	float a = scale.getX() > scale.getY() ? scale.getX() : scale.getY();
	a = a > scale.getZ() ? a : scale.getZ();
	return radius_ * a;
}
