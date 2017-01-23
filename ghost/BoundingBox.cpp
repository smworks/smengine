/*
 * BoundingBox.cpp
 *
 *  Created on: 2013.02.08
 *      Author: Martynas Šustavičius
 */

#include "BoundingBox.h"

BoundingBox::BoundingBox() : sizes(Vec3(1.0f)) {}
BoundingBox::BoundingBox(const Vec3& sizes) : sizes(sizes) {}
BoundingBox::~BoundingBox() {}

BoundingVolume::State BoundingBox::isInFrustum(Camera* camera, Vec3& pos, Vec3& scale) {
	//float dist;
	//Vec3& vec = getSizes();
	//vec *= 0.5f;
	//corners[0].setXYZ(vec);
	//corners[1].setXYZ(vec * -1.0f);
	//corners[2].setXYZ(vec * Vec3(-1.0f, 0.0f, 0.0f));
	//corners[3].setXYZ(vec * Vec3(-1.0f, -1.0f, 0.0f));
	//corners[4].setXYZ(vec * Vec3(0.0f, -1.0f, 0.0f));
	//corners[5].setXYZ(vec * Vec3(0.0f, -1.0f, -1.0f));
	//corners[6].setXYZ(vec * Vec3(0.0f, 0.0f, -1.0f));
	//corners[7].setXYZ(vec * Vec3(-1.0f, 0.0f, -1.0f));
	//for (int i = 0; i < 6; i++) {
	//	Camera::Plane& p = camera->getPlane(i);
	//	for (int j = 0; j < 8; j++) {
	//		Vec3& c = corners[i];
	//		dist = p.a * c.getX()  + p.b * c.getY() + p.c * c.getZ() + p.d;
	//		if (dist < 0.0f) {
	//			return BoundingVolume::OUTSIDE;
	//		}
	//	}
	//}
	return BoundingVolume::INSIDE;
}

BoundingVolume::Type BoundingBox::getType() {
	return BOX;
}

BoundingVolume* BoundingBox::copy() {
	BoundingBox* bv = NEW BoundingBox();
	*bv = *this;
	return bv;
}

void BoundingBox::setSizes(Vec3& sizes) {
	sizes = sizes;
}

Vec3 BoundingBox::getSizes(Vec3 scale) {
	return Vec3(sizes.getX() * scale.getX(),
		sizes.getY() * scale.getY(), sizes.getZ() * scale.getZ());
}