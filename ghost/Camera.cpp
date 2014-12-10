/*
 * Camera.cpp
 *
 *  Created on: 2012.07.07
 *      Author: Martynas Šustavičius
 */

#include "Camera.h"
#include "Node.h"
#include "Multiplatform/Database.h"

Camera::Camera(Database* database) :
	width_(0.0f),
	height_(0.0f),
	follow_(0),
	lookAt_(0),
	database_(database)
{
	Matrix::identity(projection2D_);
	Matrix::identity(projection3D_);
	Matrix::identity(mat_);
	Matrix::identity(matPos_);
	Matrix::identity(matRot_);
	LOGD("Created camera.");
}

Camera::~Camera() {
	LOGD("Deleted camera.");
}

void Camera::update(double timeDelta) {
	if (lookAt_ != 0) {
		if (follow_ != 0) {
			Matrix::cameraLookAt(mat_,
				Vec3(
					-pos_.getX() - follow_->getPos().getX(),
					-pos_.getY() - follow_->getPos().getY(),
					-pos_.getZ() - follow_->getPos().getZ()
				),
				lookAt_->getPos());
		}
		else {
			Matrix::cameraLookAt(mat_,
				Vec3(-pos_.getX(), -pos_.getY(), -pos_.getZ()),
				lookAt_->getPos());
		}
	}
	else {
		if (follow_ != 0) {
			Matrix::translate(
				matPos_,
				-pos_.getX() - follow_->getPos().getX(),
				-pos_.getY() - follow_->getPos().getY(),
				-pos_.getZ() - follow_->getPos().getZ());
		}
		else {
			Matrix::translate(
				matPos_, -pos_.getX(), -pos_.getY(), -pos_.getZ());
		}
		Matrix::rotateXYZInv(matRot_, floatTo360Bounds(rot_.getX()),
			floatTo360Bounds(rot_.getY()), floatTo360Bounds(rot_.getZ()));
		Matrix::multiply(matRot_, matPos_, mat_);
	}
}

void Camera::updateProjections(
	UINT32 width, UINT32 height, float fov, float nearDist, float farDist)
{
	width_ = (float) width;
	height_ = (float) height;
	Matrix::projection3D(projection3D_,
		fov, width_ / height_, nearDist, farDist);
	Matrix::projection2D(projection2D_,	width_, height_, farDist);
}

void Camera::follow(Node* node) {
	follow_ = node;
}

void Camera::lookAt(Node* node) {
	lookAt_ = node;
}

void normalize(Camera::Plane& p) {
	float length = sqrt(
		p.a * p.a +
		p.b * p.b +
		p.c * p.c);
	p.a /= length;
	p.b /= length;
	p.c /= length;
	p.d /= length;
}

void Camera::extractPlanes(Mat4 matrix) {
	// PLANE_LEFT clipping plane.
	planes[PLANE_LEFT].a = matrix[Matrix::_41] + matrix[Matrix::_11];
	planes[PLANE_LEFT].b = matrix[Matrix::_42] + matrix[Matrix::_12];
	planes[PLANE_LEFT].c = matrix[Matrix::_43] + matrix[Matrix::_13];
	planes[PLANE_LEFT].d = matrix[Matrix::_44] + matrix[Matrix::_14];
	// PLANE_RIGHT clipping plane.
	planes[PLANE_RIGHT].a = matrix[Matrix::_41] - matrix[Matrix::_11];
	planes[PLANE_RIGHT].b = matrix[Matrix::_42] - matrix[Matrix::_12];
	planes[PLANE_RIGHT].c = matrix[Matrix::_43] - matrix[Matrix::_13];
	planes[PLANE_RIGHT].d = matrix[Matrix::_44] - matrix[Matrix::_14];
	// PLANE_TOP clipping plane.
	planes[PLANE_TOP].a = matrix[Matrix::_41] - matrix[Matrix::_21];
	planes[PLANE_TOP].b = matrix[Matrix::_42] - matrix[Matrix::_22];
	planes[PLANE_TOP].c = matrix[Matrix::_43] - matrix[Matrix::_23];
	planes[PLANE_TOP].d = matrix[Matrix::_44] - matrix[Matrix::_24];
	// PLANE_BOTTOM clipping plane.
	planes[PLANE_BOTTOM].a = matrix[Matrix::_41] + matrix[Matrix::_21];
	planes[PLANE_BOTTOM].b = matrix[Matrix::_42] + matrix[Matrix::_22];
	planes[PLANE_BOTTOM].c = matrix[Matrix::_43] + matrix[Matrix::_23];
	planes[PLANE_BOTTOM].d = matrix[Matrix::_44] + matrix[Matrix::_24];
	// PLANE_NEAR clipping plane.
	planes[PLANE_NEAR].a = matrix[Matrix::_41] + matrix[Matrix::_31];
	planes[PLANE_NEAR].b = matrix[Matrix::_42] + matrix[Matrix::_32];
	planes[PLANE_NEAR].c = matrix[Matrix::_43] + matrix[Matrix::_33];
	planes[PLANE_NEAR].d = matrix[Matrix::_44] + matrix[Matrix::_34];
	// PLANE_FAR clipping plane.
	planes[PLANE_FAR].a = matrix[Matrix::_41] - matrix[Matrix::_31];
	planes[PLANE_FAR].b = matrix[Matrix::_42] - matrix[Matrix::_32];
	planes[PLANE_FAR].c = matrix[Matrix::_43] - matrix[Matrix::_33];
	planes[PLANE_FAR].d = matrix[Matrix::_44] - matrix[Matrix::_34];

	for (int i = 0; i < 6; i++) {
		normalize(planes[i]);
	}
}

void Camera::getRayEnd(Vec3& vec, float length) {
	static Mat4 inv;
	Matrix::identity(inv);
	static Vec3 move;
	move.setXYZ(0.0f, 0.0f, -length);
	vec.setXYZ(pos_);
	Matrix::transformInv(inv, pos_, rot_);
	Matrix::multiply(move, inv);
	vec.addXYZ(move.getX(), move.getY(), move.getZ());
}

Mat4& Camera::getProjection2D() {
	return projection2D_;
}

Mat4& Camera::getProjection3D() {
	return projection3D_;
}

Vec3& Camera::getPos() {
	return pos_;
}

Vec3& Camera::getRot() {
	return rot_;
}

Mat4& Camera::getMatrix() {
	return mat_;
}

Camera::Plane& Camera::getPlane(UINT32 i) {
	return planes[i];
}

void Camera::print() {
	LOGD("CAMERA");
	LOGD("	Position:");
	pos_.print("	");
	LOGD("	Rotation:");
	rot_.print("	");
}
