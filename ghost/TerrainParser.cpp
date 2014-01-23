/*
 * TerrainParser.cpp
 *
 *  Created on: 2012.12.24
 *      Author: MS
 */

#include "TerrainParser.h"
#include "BoundingSphere.h"
#include "Shapes.h"
#include "Multiplatform/ServiceLocator.h"
#include "Resources/TextureRGBA.h"

TerrainGeometry::~TerrainGeometry() {
	for (SIZE i = 0; i < indices_->size(); i++) {
		delete (*indices_)[i];
	}
    for (SIZE i = 0; i < bounds_->size(); i++) {
		delete (*bounds_)[i];
	}
	indices_->clear();
	delete indices_;
	bounds_->clear();
	delete bounds_;
	delete vertices_;
	delete normals_;
	delete uv_;
}

TerrainParser::TerrainParser(
	ServiceLocator* services, TextureRGBA* heightMap, float heightRatio, float heightOffset)
{
	services_ = services;
	heightMap_ = heightMap;
	heightRatio_ = heightRatio;
	heightOffset_ = heightOffset;
	segmentSize_ = 128;
	dx_ = 0.5f;
	dz_ = 0.5f;
}

TerrainParser::~TerrainParser() {
	// TODO Auto-generated destructor stub
}

bool TerrainParser::load(TerrainGeometry& geometry, BoundingVolume *& bv) {
	PROFILE("Started generating terrain.");
	UINT32 width = heightMap_->getWidth();
	UINT32 height = heightMap_->getHeight();
	if (width % 2 != 0) {
		LOGE("Terrain width must be power of 2.");
		return false;
	}
	vector<float>* vertices = NEW vector<float>();
	vertices->reserve(width * 3 * height);\
	vector<UINT16>* fullIndices = 0; //new vector<UINT16>();
	vector<float>* uv = NEW vector<float>;
	uv->reserve(height * width * 2);
	UINT8 color[] = {0, 0, 0, 255};
	int blue = 0;
	Shapes::grid(height - 1, width - 1, dx_, dz_, vertices, fullIndices);
	for (int j = height - 1; j >= 0; j--) {
		for (unsigned int i = 0; i < width; i++) {
			UINT8* pixel = heightMap_->getPixel(j, i);
			float heightVal = heightOffset_ + heightRatio_ * pixel[0];
			(*vertices)[(height - j - 1) * width * 3 + i * 3 + 1] = heightVal;
			blue = pixel[2];
			if (blue > 0) {
				int b = 255 - blue;
				if (b < 70) {
					b += 70;
				}
				color[0] = color[1] = color[2] = b;
			} else {
				color[0] = color[2] = 0;
				color[1] = heightMap_->getPixel(j, i)[0];
				color[1] = 255;
				color[0] = color[2] = 0;
			}
			uv->push_back((float) i / width);
			uv->push_back((float) j / height);
		}
	}
	filterHeights(vertices, width, height);
	vector<float>* normals = NEW vector<float>();
	normals->resize(vertices->size(), 0.0f);
	for (UINT32 i = 0; i < height - 1; i++) {
		for (UINT32 j = 0; j < width * 3 - 3; j += 3) {

			int offset = i * width * 3 + j;
			Vec3 a(
				vertices->at(offset + 0),
				vertices->at(offset + 1),
				vertices->at(offset + 2));
			offset = i * width * 3 + j + 3;
			Vec3 b(
				vertices->at(offset + 0),
				vertices->at(offset + 1),
				vertices->at(offset + 2));
			offset = (i + 1) * width * 3 + j;
			Vec3 c(
				vertices->at(offset + 0),
				vertices->at(offset + 1),
				vertices->at(offset + 2));
			offset = (i + 1) * width * 3 + j + 3;
			Vec3 d(
				vertices->at(offset + 0),
				vertices->at(offset + 1),
				vertices->at(offset + 2));

			Vec3 u(b); u -= a;
			Vec3 v(c); v -= a;

			Vec3 res(v); res.crossProduct(u);
			res.normalize();

			offset = i * width * 3 + j;
			(*normals)[offset + 0] = res.getX();
			(*normals)[offset + 1] = res.getY();
			(*normals)[offset + 2] = res.getZ();
			offset = i * width * 3 + j + 3;
			(*normals)[offset + 0] = res.getX();
			(*normals)[offset + 1] = res.getY();
			(*normals)[offset + 2] = res.getZ();
			offset = (i + 1) * width * 3 + j;
			(*normals)[offset + 0] = res.getX();
			(*normals)[offset + 1] = res.getY();
			(*normals)[offset + 2] = res.getZ();
			offset = (i + 1) * width * 3 + j + 3;
			(*normals)[offset + 0] = res.getX();
			(*normals)[offset + 1] = res.getY();
			(*normals)[offset + 2] = res.getZ();
		}
	}
	vector<float> heights;
	float minHeight = FLT_MAX, maxHeight = -FLT_MAX;
	SIZE size = vertices->size();
	for (SIZE i = 0; i < size; i += 3) {
		float hVal = vertices->at(i + 1);
		if (minHeight > hVal) {
			minHeight = hVal;
		}
		if (maxHeight < hVal) {
			maxHeight = hVal;
		}
	}
	float offset = (maxHeight - minHeight) * 0.5f + minHeight;
	for (UINT32 i = 0; i < size; i += 3) {
		float hVal = vertices->at(i + 1) - offset;
		vertices->at(i + 1) = hVal;
		heights.push_back(hVal);
	}
	geometry.normals_ = normals;
	geometry.uv_ = uv;
	int segmentRows = height / segmentSize_ + (height % segmentSize_ == 0 ? 0 : 1);
	int segmentCols = width / segmentSize_ + (width % segmentSize_ == 0 ? 0 : 1);
	int segmentCount = segmentRows * segmentCols;
	vector<vector<GLushort>*>* indices = NEW vector<vector<GLushort>*>();
	vector<BoundingVolume*>* bounds = NEW vector<BoundingVolume*>();
	indices->reserve(segmentCount);
	bounds->reserve(segmentCount);
	for (unsigned int i = 0; i < height; i += segmentSize_) {
		for (unsigned int j = 0; j < width; j += segmentSize_) {
			bounds->push_back(generateBoundingVolume(
					i, j, segmentSize_, segmentSize_, height, width, dx_, dz_));
			indices->push_back(generateSubTerrainIndices(
				i, j, segmentSize_, segmentSize_, width, height));
		}
	}
	geometry.bounds_ = bounds;
	geometry.vertices_ = vertices;
	geometry.indices_ = indices;
	LOGD("Terrain vertex count: %u", (UINT32) vertices->size() / 3);
	LOGD("Terrain index array count: %u", (UINT32) indices->size());
	PROFILE("Terrain generated.");
	return true;
}

BoundingVolume* TerrainParser::generateBoundingVolume(
	int rowOffset, int colOffset, int rows, int cols, int height,
	int width, float dx, float dz)
{
	//float centerX = -(width * dx / 2.0f) + colOffset * dx + (cols * dx / 2.0f);
	//float centerZ = -(height * dz / 2.0f) + rowOffset * dz + (rows * dz / 2.0f);
	////float radius = (float) Math.sqrt(cols * dx * cols * dx + rows * dz * rows * dz);
	float radius = 70.0f;
	BoundingSphere* bs = NEW BoundingSphere(radius);
	//Vec3 pos(centerX, 0.0, centerZ);
	//bs->setPos(pos);
	return bs;
}

vector<GLushort>* TerrainParser::generateSubTerrainIndices(int rowOffset, int colOffset,
		int rows, int cols, int width, int height) {
	int h = rowOffset + rows < height ? rows :  height - rowOffset - 1;
	int w = colOffset + cols < width ? cols : width - colOffset - 1;
	vector<GLushort>* indices = NEW vector<GLushort>();
	indices->reserve(h * w * 6);
	for (int i = rowOffset; i < h + rowOffset; i++) {
		for (int j = colOffset; j < w + colOffset; j++) {
			indices->push_back((i + 1) * width + j);
			indices->push_back(i * width + j + 1);
			indices->push_back(i * width + j);
			indices->push_back((i + 1) * width + j + 1);
			indices->push_back(i * width + j + 1);
			indices->push_back((i + 1) * width + j);
		}
	}
	return indices;
}

void TerrainParser::filterHeights(
	vector<float>* vertices, int width, int height)
{
	float sum, heights;
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			sum = 0.0f; heights = 0.0f;
			for (int i = row - 1; i < row + 1; i++) {
				for (int j = col - 1; j < col + 1; j++) {
					if (i > 0 && i < height && j > 0 && j < width) {
						sum += (*vertices)[i * width * 3 + j * 3 + 1];
						heights += 1.0f;
					}
				}
			}
			(*vertices)[row * width * 3 + col * 3 + 1] = sum / heights;
		}
	}
}
