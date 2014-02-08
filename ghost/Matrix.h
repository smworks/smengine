/*
 * Matrix.h
 *
 *  Created on: 2012.06.27
 *      Author: MS
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#include "Multiplatform/Ghost.h"
#include "Vec3.h"

typedef float Mat3[9];
typedef float Mat4[16];

class Matrix {
public:
	// OpenGL column major order.
	static const short _11 = 0, _21 = 1, _31 = 2, _41 = 3,
					   _12 = 4, _22 = 5, _32 = 6, _42 = 7,
					   _13 = 8, _23 = 9, _33 = 10, _43 = 11,
					   _14 = 12, _24 = 13, _34 = 14, _44 = 15;

public:
	static void toMat3(Mat4 src, Mat3 dest);
	static void identity(Mat4 mat);
	static void invert(const Mat4 in, Mat4 out);
	static void translate(Mat4 mat, float x, float y, float z);
	static void rotateArbitary(
		Mat4 mat, float angle, float x, float y, float z);
	static void rotateX(Mat4 mat, float angle);
	static void rotateY(Mat4 mat, float angle);
	static void rotateZ(Mat4 mat, float angle);

	/**
	 * Calculates rotation matrix around all three axes.
	 * @param mat - matrix that will contain result.
	 * @param x - rotation angle around x axis.
	 * @param y - rotation angle around y axis.
	 * @param z - rotation angle around z axis.
	 */
	static void rotateXYZ(Mat4, float, float, float);

	/**
	 * Calculates inverse rotation matrix.
	 * @param mat - matrix that will contain result.
	 * @param x - rotation angle around x axis.
	 * @param y - rotation angle around y axis.
	 * @param z - rotation angle around z axis.
	 */
	static void rotateXYZInv(Mat4 mat, float x, float y, float z);

	/**
	 * Calculates scale matrix.
	 * @param mat - matrix that will contain result.
	 * @param x - rotation angle around x axis.
	 * @param y - rotation angle around y axis.
	 * @param z - rotation angle around z axis.
	 */
	static void scale(Mat4 mat, float x, float y, float z);

	/**
	 * Applies matrix multiplication.
	 * @param m1 - left matrix.
	 * @param m2 - right matrix.
	 * @param res - result matrix.
	 */
	static void multiply(const Mat4 m1, const Mat4 m2, Mat4 res);

	/**
	 * Transforms vector into new space by multiplying by specified matrix.
	 * @param vector - vector that will be transformed.
	 * @param mat - matrix containing transformation data.
	 */
	static void multiply(Vec3& vector, Mat4 mat);

	/**
	 * Calculates camera matrix so it would appear that
	 * camera is looking at specified target.
	 * @param mat - source matrix that will contain result.
	 * @param pos - position of the camera.
	 * @param target - position of the target that camera points to.
	 */
	static void cameraLookAt(Mat4 mat, Vec3 pos, Vec3 target);

	/**
	 * Generates and returns parallel (orthogonal) projection matrix.
	 * Using this matrix translating each float value represents a pixel,
	 * so for example cube with width of 100.0f and height of 100.0f
	 * will be 100px width and 100px height.
	 * Translation position at (0.0f, 0.0f, 0.0f)
	 * will correspond to bottom left screen corner.
	 * Increasing x coordinate will
	 * move object to right, increasing y coordinate will move object up.
	 * @param mat - source matrix.
	 * @param width - width of the screen in pixels.
	 * @param height - height of the screen in pixels.
	 * @param f - far clipping plane distance.
	 */
	static void projection2D(Mat4 mat, float width, float height, float f);

	/**
	 * Generates and assigns perspective projection matrix
	 * to specified matrix.
	 * @param mat - source matrix.
	 * @param fovy - field of view angle in y direction,
	 * @param aspect - ratio of width to height,
	 * @param nearPlane - distance to near clipping plane,
	 * @param farPlane - distance to far clipping plane.
	 */
	static void projection3D(
		Mat4 mat, float fovy, float aspect, float near, float far);
	
	/**
	 * Creates transformation matrix while using
	 * inverted rotation and translation matrices.
	 */
	static void transformInv(Mat4 mat, Vec3& pos, Vec3& rot);

	/**
	 * @param vec - vector that will contain result.
	 * @param mat - matrix containing transformations.
	 */
	static void getTranslation(Vec3& vec, Mat4 mat);

	/**
	 * @param vec - vector that will contain result.
	 * @param mat - matrix containing transformations.
	 */
	static void getRotation(Vec3& vec, Mat4 mat);

	/**
	 * @param vec - vector that will contain result.
	 * @param mat - matrix containing transformations.
	 */
	static void getScaling(Vec3& vec, Mat4 mat);

	/**
	 * Copies data from original matrix to target matrix.
	 * @param source - original matrix.
	 * @param destination - target matrix.
	 */
	static void copy(Mat4 source, Mat4 destination);

	/**
	 * Prints specified matrix.
	 * @param mat - matrix.
	 */
	static void print(Mat4 mat);
};

#endif /* MATRIX_H_ */
