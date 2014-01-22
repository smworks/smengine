/*
 * ParserTerrain.h
 *
 *  Created on: 2012.12.24
 *      Author: Martynas Šustavičius
 */

#ifndef TERRAINPARSER_H_
#define TERRAINPARSER_H_

#include "Multiplatform/Ghost.h"

class ServiceLocator;
class TextureRGBA;
class BoundingVolume;
class ServiceLocator;

struct TerrainGeometry {
	TerrainGeometry() :
		vertices_(0),
		normals_(0),
		uv_(0),
		indices_(0),
		bounds_(0)
	{}
	~TerrainGeometry();
	vector<float>* vertices_;
	vector<float>* normals_;
	vector<float>* uv_;
	vector<vector<UINT16>*>* indices_;
	vector<BoundingVolume*>* bounds_;
};

class TerrainParser {
public:
	/**
	 * Creates terrain object.
	 * @param system - system.
	 * @param heightMap - image containing terrain heights.
	 * @param heightRatio - height scale modifier between 0.0f and 1.0f.
	 * @param heightOffset - height offset.
	 */
	TerrainParser(ServiceLocator* services, TextureRGBA*, float, float);
	~TerrainParser();

	/**
	 * Creates terrain from specified data.
	 */
	bool load(TerrainGeometry& geometry, BoundingVolume*& bv);

	/**
	 * Removes one vertex from vertex grid.
	 * @param indices - vector containing vectors containing indices.
	 * @param vertices - pointer to vertex vector.
	 * @param row - row in grid.
	 * @param col - col in grid.
	 * @param width - grid width vertex count.
	 */
	void removeVertex(
			vector<vector<GLushort>*>* indices, vector<float>* vertices, UINT32 row, UINT32 col, UINT32 width);

	/**
	 * Creates bounding volume.
	 * @param rowOffset - row index from where to generate indices.
	 * @param colOffset - column index from where to generate indices.
	 * @param rows - height of the sub-terrain.
	 * @param cols - width of the sub-terrain.
	 * @param height - height of the original terrain.
	 * @param width - width of the original terrain.
	 * @param dx - width of the plane segment.
	 * @param dz - height of the plane segment.
	 * @return Bounding volume containing plane segment.
	 */
	BoundingVolume* generateBoundingVolume(int rowOffset, int colOffset,
			int rows, int cols, int height, int width, float dx, float dz);

	/**
	 * Generates indices for specified size sub-terrain.
	 * @param rowOffset - row index from where to generate indices.
	 * @param colOffset - column index from where to generate indices.
	 * @param rows - height of the sub-terrain.
	 * @param cols - width of the sub-terrain.
	 * @param width - width of the original terrain.
	 * @param height - height of the original terrain.
	 * @return Generated indices for sub-terrain.
	 */
	vector<GLushort>* generateSubTerrainIndices(
		int, int, int, int, int, int);

	/**
	 * Makes vertices on terrain much smoother.
	 * @param vertices - array of terrain vertices.
	 * @param width - width of the terrain.
	 * @param height - height of the terrain.
	 */
	void filterHeights(vector<float>*, int, int);
	float getDX() { return dx_; }
	float getDZ() { return dz_; }
private:
	/** Terrain height map. */
	TextureRGBA* heightMap_;
	/** Height ratio. */
	float heightRatio_;
	/** Height offset. */
	float heightOffset_;
	/** One rectangle width. */
	float dx_;
	/** One rectangle height. */
	float dz_;
	/** Size of one terrain segment. */
	int segmentSize_;
	/** Service locator. */
	ServiceLocator* services_;
};

#endif
