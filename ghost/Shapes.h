/*
 * Shapes.h
 *
 *  Created on: 2012.07.06
 *      Author: MS
 */

#ifndef SHAPES_H_
#define SHAPES_H_

#include "Multiplatform/Ghost.h"
#include "Resources/Vertex.h"
class Vec3;

const float g_planeVertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
};

const float g_planeUV[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f
};

class Shapes {
public:
    enum ShapeType {SHAPE_GROUND_PLANE, SHAPE_SCREEN_PLANE};
    enum VertexType {
		VERTEX_POS_NORM_TEX, VERTEX_POS, VERTEX_TEX, VERTEX_POS_NORM, VERTEX_POS_TEX};
public:
	/**
	 * Generates 2D grid.
	 * @param rows - number of grid segments vertically. Minimal value 1.
	 * @param cols - number of grid segments horizontally. Minimal value 1.
	 * @param dx - width of the plane segment.
	 * @param dz - height of the plane segment.
	 * @param vertices - initialized vertex array.
	 * @param indices - initialized index array.
	 */
	static void grid(
		int rows, int cols, float dx, float dz,
		vector<float>* vertices, vector<UINT16>* indices);

    /**
     * Returns geometry for specified shape type.
     * @param shapeType - type of the required shape.
     * @param vertexType - type of the vertex used to create geometry.
     * @return Pointer to vector containing specified type of vertices,
     * or 0 if error occured.
     */
    static void* getShape(ShapeType shapeType, VertexType vertexType);

	/**
	 * Fills specified arrays with data that represent sphere.
	 * By default radius is 0.5f to make a sphere fit into a
	 * cube by the dimensions of 1.0 x 1.0 x 1.0.
	 * @param rings - number of rings.
	 * @param sectors - number of sectors.
	 * @param vertices - initialized vertex array.
	 * @param normals - initialized normal array or 0 if not needed.
	 * @param textureCoords - initialized texture coordinate array
	 * or 0 if not needed.
	 * @param indices - initialized index array.
	 */
	static void getSphere(UINT32 rings, UINT32 sectors,
		vector<Vec3>* vertices, vector<float>* normals,
		vector<float>* textureCoords, vector<UINT16>* indices);
private:
    /**
     * Combines specified arrays into one combinet vertex array.
     * @param vertexType - type of the vertices to be used for geometry.
     * @param positions - array of geometry 3d point positions.
     * @param normals - array of geometry normals.
     * @param coordinates - array of geometry texture coordinates.
     */
    static void* createShape(VertexType vertexType,
        float* positions, float* normals, float* coordinates, int vertexCount);
};

#endif /* SHAPES_H_ */
