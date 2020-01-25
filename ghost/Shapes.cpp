/*
 * Shapes.cpp
 *
 *  Created on: 2012.07.06
 *      Author: MS
 */

#include "Shapes.h"
#include "Resources/Vertex.h"
#include "Vec3.h"

void Shapes::grid(
	int rows, int cols, float dx, float dz, vector<float>* vertices, vector<UINT16>* indices)
{
	if (rows <= 0 || cols <= 0) {
		LOGW("Grid not created, due to wrong parameters.");
		return;
	}
	int vertRows = rows + 1;
	int vertCols = cols + 1;
	float width = dx * cols;
	float height = dz * rows;
	float xOffset = -width * 0.5f;
	float zOffset = -height * 0.5f;
	// Generates vertex list.
	if (vertices != 0) {
		for (int i = 0; i < vertRows; i++) {
			for (int j = 0; j < vertCols; j++) {
				vertices->push_back(j * dx + xOffset);
				vertices->push_back(0.0f);
				vertices->push_back(i * dz + zOffset);
			}
		}
	}
	// Quad looks like this:
	// C_____B
	//  |  /|
	//  | / |
	// A|/  |D
	// Indices make ABC and DBA triangles.
	// Generates index list.
	if (indices != 0) {
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				indices->push_back((i + 1) * vertCols + j);
				indices->push_back(i * vertCols + j + 1);
				indices->push_back(i * vertCols + j);
				indices->push_back((i + 1) * vertCols + j + 1);
				indices->push_back(i * vertCols + j + 1);
				indices->push_back((i + 1) * vertCols + j);
			}
		}
	}
}

void* Shapes::createShape(Shapes::VertexType vertexType, float* positions, float* normals, float* coordinates, int vertexCount) {
    switch (vertexType) {
    case Shapes::VERTEX_POS_NORM_TEX:
        break;
    case Shapes::VERTEX_POS:
		{
        vector<VertexP>* cbo = NEW vector<VertexP>();
		cbo->reserve(vertexCount);
        VertexP v;
        for (int i = 0; i < vertexCount; i++) {
            v.pos[0] = positions[i * 3 + 0];
            v.pos[1] = positions[i * 3 + 1];
            v.pos[2] = positions[i * 3 + 2];
            cbo->push_back(v);
        }
		return (void*) cbo;
		}
        break;
    case Shapes::VERTEX_TEX:
		{
        vector<VertexT>* cbo = NEW vector<VertexT>();
		cbo->reserve(vertexCount);
        VertexT v;
        for (int i = 0; i < vertexCount; i++) {
            v.uv[0] = coordinates[i * 2 + 0];
            v.uv[1] = coordinates[i * 2 + 1];
            cbo->push_back(v);
        }
		return (void*) cbo;
		}
    case Shapes::VERTEX_POS_NORM:
        break;
    case Shapes::VERTEX_POS_TEX:
        {
		vector<VertexPT>* cbo = NEW vector<VertexPT>();
        cbo->reserve(vertexCount);
        VertexPT v;
        for (int i = 0; i < vertexCount; i++) {
            v.pos[0] = positions[i * 3 + 0];
            v.pos[1] = positions[i * 3 + 1];
            v.pos[2] = positions[i * 3 + 2];
            v.uv[0] = coordinates[i * 2 + 0];
            v.uv[1] = coordinates[i * 2 + 1];
            cbo->push_back(v);
        }
        return (void*) cbo;
		}
        break;
    }
    return 0;
}

void* Shapes::getShape(ShapeType shapeType, VertexType vertexType) {
    switch (shapeType) {
    case SHAPE_GROUND_PLANE:

        break;
    case SHAPE_SCREEN_PLANE:
    {
        return createShape(vertexType, g_planePositions, 0, (float*) g_planeUV, 6);
        break;
    }
    default:
        return 0;
        break;
    }
    return 0;
}

void Shapes::getSphere(UINT32 rings, UINT32 sectors,
	vector<Vec3>* vertices, vector<float>* normals,
	vector<float>* textureCoords, vector<UINT16>* indices)
{
	if (rings == 0 || sectors == 0) {
		LOGW("Creating sphere with less than 1 ring or 1 sector.");
	}
	static const float radius = 0.5f;
	float const R = 1.0f / (float) (rings - 1);
    float const S = 1.0f / (float) (sectors - 1);
    UINT32 r, s;
    vertices->resize(rings * sectors * 3);
	if (normals != 0) {
		normals->resize(rings * sectors * 3);
	}
	if (textureCoords != 0) {
	    textureCoords->resize(rings * sectors * 2);
	}
    vector<Vec3>::iterator v = vertices->begin();
    vector<float>::iterator n;
	if (normals != 0) {
		normals->begin();
	}
	vector<float>::iterator t;
	if (textureCoords != 0) {
		t = textureCoords->begin();
	}
    for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
        float y = (float) sin(-SMART_PI * 0.5f + SMART_PI * r * R);
        float x = (float) (cos(2 * SMART_PI * s * S)
			* sin(SMART_PI * r * R));
        float z = (float) (sin(2 * SMART_PI * s * S)
			* sin(SMART_PI * r * R));
        *v++ = Vec3(x * radius, y * radius, z * radius);
		if (normals != 0) {
			*n++ = x;
			*n++ = y;
			*n++ = z;
		}
		if (textureCoords != 0) {
			*t++ = s * S;
			*t++ = r * R;
		}

    }
    indices->resize(rings * sectors * 6);
    vector<UINT16>::iterator i = indices->begin();
    for (r = 0; r < rings - 1; r++) for (s = 0; s < sectors - 1; s++) {
        *i++ = r * sectors + s;
        *i++ = r * sectors + (s+1);
        *i++ = (r+1) * sectors + (s+1);
		*i++ = (r+1) * sectors + (s+1);
        *i++ = (r+1) * sectors + s;
		*i++ = r * sectors + s;
    }
}