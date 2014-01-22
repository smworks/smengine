/*
 * Matrix.cpp
 *
 *  Created on: 2012.06.27
 *      Author: MS
 */

#include "Matrix.h"
#include "Utils.h"

void Matrix::toMat3(Mat4 src, Mat3 dest) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			dest[i * 3 + j] = src[i * 4 + j];
		}
	}
}

void Matrix::identity(Mat4 mat) {
	mat[0] = 1.0f;	mat[1] = 0.0f;	mat[2] = 0.0f;	mat[3] = 0.0f;
	mat[4] = 0.0f;	mat[5] = 1.0f;	mat[6] = 0.0f;	mat[7] = 0.0f;
	mat[8] = 0.0f;	mat[9] = 0.0f;	mat[10] = 1.0f;	mat[11] = 0.0f;
	mat[12] = 0.0f;	mat[13] = 0.0f;	mat[14] = 0.0f;	mat[15] = 1.0f;
}

void Matrix::invert(const Mat4 m, Mat4 invOut) {
    Mat4 inv;
	Matrix::identity(inv);
	float det;
    int i;
    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];
    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];
    inv[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];
    inv[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];
    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];
    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];
    inv[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];
    inv[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];
    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];
    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];
    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];
    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];
    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];
    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];
    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];
    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];
    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
    if (det == 0) {
		for (i = 0; i < 16; i++)
			invOut[i] = m[i];
        return;
	}
    det = 1.0f / det;
    for (i = 0; i < 16; i++) {
        invOut[i] = inv[i] * det;
	}
}

void Matrix::translate(Mat4 mat, float x, float y, float z) {
	identity(mat);
	mat[12] = x;
	mat[13] = y;
	mat[14] = z;
}

void Matrix::rotateArbitary(
	Mat4 mat, float angle, float x, float y, float z)
{
	float c = cosPrec(angle);
	float s = sinPrec(angle);
	float omc = (1 - c);
	// First row.
	float x1y1 = x * x * omc + c;
	float x2y1 = y * x * omc + z * s;
	float x3y1 = x * z * omc - y * s;
	// Second row.
	float x1y2 = x * y * omc - z * s;
	float x2y2 = y * y * omc + c;
	float x3y2 = y * z * omc + x * s;
	// Third row.
	float x1y3 = x * z * omc + y * s;
	float x2y3 = y * z * omc - x * s;
	float x3y3 = z * z * omc + c;
	// 4x4 rotation matrix.
	mat[0] = x1y1;	mat[1] = x2y1;	mat[2] = x3y1;	mat[3] = 0.0f;
	mat[4] = x1y2;	mat[5] = x2y2;	mat[6] = x3y2;	mat[7] = 0.0f;
	mat[8] = x1y3;	mat[9] = x2y3;	mat[10] = x3y3;	mat[11] = 0.0f;
	mat[12] = 0.0f;	mat[13] = 0.0f;	mat[14] = 0.0f;	mat[15] = 1.0f;
}

void Matrix::rotateX(Mat4 mat, float angle) {
	identity(mat);
	mat[5] = cosPrec(angle);
	mat[6] = -sinPrec(angle);
	mat[9] = -mat[6];
	mat[10] = mat[5];
}

void Matrix::rotateY(Mat4 mat, float angle) {
	identity(mat);
	mat[0] = cosPrec(angle);
	mat[2] = sinPrec(angle);
	mat[8] = -mat[2];
	mat[10] = mat[0];
}

void Matrix::rotateZ(Mat4 mat, float angle) {
	identity(mat);
	mat[0] = cosPrec(angle);
	mat[1] = sinPrec(angle);
	mat[4] = -mat[1];
	mat[5] = mat[0];
}

void Matrix::rotateXYZ(Mat4 mat, float x, float y, float z) {
	Mat4 matX;
	Mat4 matY;
	Mat4 matZ;
	Mat4 yx;
	rotateX(matX, x);
	rotateY(matY, y);
	rotateZ(matZ, z);
	multiply(matY, matX, yx);
	multiply(yx, matZ, mat);
}

void Matrix::rotateXYZInv(Mat4 mat, float x, float y, float z) {
	Mat4 matX;
	Mat4 matY;
	Mat4 matZ;
	Mat4 zx;
	rotateX(matX, -x);
	rotateY(matY, -y);
	rotateZ(matZ, -z);
	multiply(matZ, matX, zx);
	multiply(zx, matY, mat);
}

void Matrix::scale(Mat4 mat, float x, float y, float z) {
	identity(mat);
	mat[0] = x;
	mat[5] = y;
	mat[10] = z;
}

void Matrix::multiply(const Mat4 m1, const Mat4 m2, Mat4 res)
{
    // First Column
    res[0] = m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2] + m1[12]*m2[3];
    res[1] = m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2] + m1[13]*m2[3];
    res[2] = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2] + m1[14]*m2[3];
    res[3] = m1[3]*m2[0] + m1[7]*m2[1] + m1[11]*m2[2] + m1[15]*m2[3];
    // Second Column
    res[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6] + m1[12]*m2[7];
    res[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6] + m1[13]*m2[7];
    res[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6] + m1[14]*m2[7];
    res[7] = m1[3]*m2[4] + m1[7]*m2[5] + m1[11]*m2[6] + m1[15]*m2[7];
    // Third Column
    res[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10] + m1[12]*m2[11];
    res[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10] + m1[13]*m2[11];
    res[10] = m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10] + m1[14]*m2[11];
    res[11] = m1[3]*m2[8] + m1[7]*m2[9] + m1[11]*m2[10] + m1[15]*m2[11];
    // Fourth Column
    res[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14] + m1[12]*m2[15];
    res[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14] + m1[13]*m2[15];
    res[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
    res[15] = m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];
}

void Matrix::multiply(Vec3& vec, Mat4 mat) {
	vec.setXYZ(
		vec.getX() * mat[0 * 4 + 0] + vec.getY() * mat[0 * 4 + 1]
			+ vec.getZ() * mat[0 * 4 + 2],
		vec.getX() * mat[1 * 4 + 0] + vec.getY() * mat[1 * 4 + 1]
			+ vec.getZ() * mat[1 * 4 + 2],
		vec.getX() * mat[2 * 4 + 0] + vec.getY() * mat[2 * 4 + 1]
			+ vec.getZ() * mat[2 * 4 + 2]);
}

void Matrix::cameraLookAt(Mat4 mat, Vec3 pos, Vec3 target) {
	Vec3 cameraF;
	Vec3 cameraS;
	Vec3 cameraU;
	Vec3 cameraUp(0.0f, 1.0f, 0.0f);
	Mat4 temp;
	Mat4 trans;
	cameraF.setXYZ(target.getX() + pos.getX(), target.getY() + pos.getY(),
			target.getZ() + pos.getZ());
	cameraF.normalize();
	cameraS.setXYZ(cameraF);
	cameraS.crossProduct(cameraUp);
	cameraU.setXYZ(cameraS);
	cameraU.crossProduct(cameraF);
	temp[ 0] = cameraS.getX();
	temp[ 1] = cameraU.getX();
	temp[ 2] = -cameraF.getX();
	temp[ 3] = 0.0f;
	temp[ 4] = cameraS.getY();
	temp[ 5] = cameraU.getY();
	temp[ 6] = -cameraF.getY();
	temp[ 7] = 0.0f;
	temp[ 8] = cameraS.getZ();
	temp[ 9] = cameraU.getZ();
	temp[10] = -cameraF.getZ();
	temp[11] = 0.0f;
	temp[12] = 0.0f;
	temp[13] = 0.0f;
	temp[14] = 0.0f;
	temp[15] = 1.0f;
	translate(trans, pos.getX(), pos.getY(), pos.getZ());
	multiply(temp, trans, mat);
}

void Matrix::projection2D(Mat4 mat, float width, float height) {
	//// Calculate essential matrix elements.
	//float x1y1 = 2.0f / (width);
	//float x2y2 = 2.0f / (height);
	//float x3y3 = -2.0f / (distance + 1.0f);
	//float x1y4 = - (width) / (width);
	//float x2y4 = - (height) / (height);
	//float x3y4 = - (distance - 1.0f) / (distance + 1.0f);
	//// 4x4 projection matrix.
	//mat[0] = x1y1;	mat[1] = 0.0f;	mat[2] = 0.0f;	mat[3] = 0.0f;
	//mat[4] = 0.0f;	mat[5] = x2y2;	mat[6] = 0.0f;	mat[7] = 0.0f;
	//mat[8] = 0.0f;	mat[9] = 0.0f;	mat[10] = x3y3;	mat[11] = 0.0f;
	//mat[12] = x1y4;	mat[13] = x2y4;	mat[14] = x3y4;	mat[15] = 1.0f;

	mat[0] = 2.0f / width;	mat[1] = 0.0f;	mat[2] = 0.0f;	mat[3] = 0.0f;
	mat[4] = 0.0f;	mat[5] = 2.0f / height;	mat[6] = 0.0f;	mat[7] = 0.0f;
	mat[8] = 0.0f;	mat[9] = 0.0f;	mat[10] = -1.0f;	mat[11] = 0.0f;
	mat[12] = -1.0f;	mat[13] = -1.0f;	mat[14] = 0.0f;	mat[15] = 1.0f;
}

void Matrix::projection3D(
	Mat4 mat, float fovy, float aspect, float nearPlane, float farPlane)
{
	// Calculate essential elements.
	float f = 1.0f / tanPrec(fovy / 2.0f);
	float x1y1 = f / aspect;
	float x3y3 = (nearPlane + farPlane) / (nearPlane - farPlane);
	float x3y4 = (2.0f * nearPlane * farPlane) / (nearPlane - farPlane);

	mat[ 0] = x1y1;	mat[ 1] = 0.0f;	mat[ 2] = 0.0f;	mat[ 3] = 0.0f;
	mat[ 4] = 0.0f;	mat[ 5] = f;	mat[ 6] = 0.0f;	mat[ 7] = 0.0f;
	mat[ 8] = 0.0f;	mat[ 9] = 0.0f;	mat[10] = x3y3;	mat[11] = -1.0f;
	mat[12] = 0.0f;	mat[13] = 0.0f;	mat[14] = x3y4;	mat[15] = 0.0f;
}

void Matrix::transformInv(Mat4 mat, Vec3& pos, Vec3& rot) {
	Mat4 mPos, mRot;
	translate(mPos, -pos.getX(), -pos.getY(), -pos.getZ());
	rotateXYZInv(mRot, rot.getX(), rot.getY(), rot.getZ());
	multiply(mPos, mRot, mat);

}

void Matrix::getTranslation(Vec3& vec, Mat4 mat) {
	vec.setXYZ(mat[12], mat[13], mat[14]);
}

void Matrix::getRotation(Vec3& vec, Mat4 mat) {
	// TODO:
}

void Matrix::getScaling(Vec3& vec, Mat4 mat) {
	// TODO:
}

void Matrix::copy(Mat4 source, Mat4 destination) {
	for (int i = 0; i < 16; i++) {
		destination[i] = source[i];
	}
}

void Matrix::print(Mat4 mat) {
	for (int i = 0; i < 4; i++) {
		LOGD("|%f %f %f %f|",
			mat[i * 4 + 0], mat[i * 4 + 1], mat[i * 4 + 2], mat[i * 4 + 3]);
	}
}