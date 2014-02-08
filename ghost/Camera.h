/*
 * Camera.h
 *
 *  Created on: 2012.07.07
 *      Author: Martynas Šustavičius
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "Multiplatform/Ghost.h"
#include "Matrix.h"

class Node;
class Settings;

class Camera {
public:
	enum Planes {
		PLANE_LEFT, PLANE_RIGHT,
		PLANE_TOP, PLANE_BOTTOM,
		PLANE_NEAR, PLANE_FAR
	};
public:
	struct Plane {
		float a, b, c, d;
	};
public:
	Camera(Settings* settings);
	~Camera();

	/**
	 * Updates camera data.
	 * @param timeDelta - time elapsed since
	 * last update call in milliseconds.
	 */
	void update(double timeDelta);

	/**
	 * Updates projection matrices.
	 * Method should be called every time
	 * resolution changes.
	 * @param width - width of the screen.
	 * @param height - height of the screen.
	 */
	void updateProjections(
		UINT32 width, UINT32 height, float fov, float nearDist, float farDist);

	/**
	 * When camera follows a node, its position is calculated
	 * relative to node position.
	 * @param node - node that camera should follow.
	 */
	void follow(Node* node);

	/**
	 * When lookAt node is specified,
	 * cameras position won't change, but
	 * rotation will always be directed to specified
	 * nodes position.
	 * @param node - node that camera should be looking at.
	 */
	void lookAt(Node* node);

	/**
	 * Retrieves plane data from specified matrix.
	 * @param matrix - matrix containing multiplied
	 * projection and model-view matrix matrices.
	 */
	void extractPlanes(Mat4 matrix);

	/**
	 * @param vec - vector that will contain end position.
	 * @param length - distance from camera.
	 * @returns Vector containing line from camera to object.
	 */
	void getRayEnd(Vec3& vec, float length);

	/**
	 * @return 2D Projection matrix.
	 */
	Mat4& getProjection2D();

	/**
	 * @return 3D Projection matrix.
	 */
	Mat4& getProjection3D();

	/**
	 * @return Vector containing camera position.
	 */
	Vec3& getPos();

	/**
	 * @return Vector containing camera rotation.
	 */
	Vec3& getRot();

	/**
	 * @return Matrix with combined position and rotation.
	 */
	Mat4& getMatrix();

	/**
	 * @param i - index of plane (0 to 5).
	 * @return reference to plane structure.
	 */
	Plane& getPlane(UINT32 i);

	/**
	 * Prints information about camera object.
	 */
	void print();

private:
	Vec3 pos_, rot_;
	Mat4 projection2D_, projection3D_,
		matPos_, matRot_, mat_;
	float width_, height_;
	Plane planes[6];
	Node* follow_, * lookAt_;
	Settings* settings_;
};

#endif /* CAMERA_H_ */
