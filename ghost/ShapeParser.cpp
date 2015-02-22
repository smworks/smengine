/*
 * ShapeParser.cpp
 *
 *  Created on: 2013.02.16
 *      Author: Martynas Šustavičius
 */

#include "ShapeParser.h"
#include "Vec3.h"
#include "Resources/Texture.h"

bool isVertexNecessary(Texture* scheme, UINT32 row, UINT32 col) {
	UINT8* d = scheme->getRawData();
	UINT32 w = scheme->getWidth();
	UINT32 h = scheme->getHeight();
	if (!d[row * w * 4 + col * 4]) {
		return false;
	}
	if (row == 0 || col == 0 ||
		row == h - 1 || col == w - 1)
	{
		UINT32 neighbours = 0;
		UINT32 fromRow = row == 0 ? 0 : row - 1;
		UINT32 toRow = row == h - 1 ? row : row + 1;
		UINT32 fromCol = col == 0 ? 0 : col - 1;
		UINT32 toCol = col == w - 1 ? col : col + 1;
		for (UINT32 i = fromRow; i <= toRow; i++) {
			for (UINT32 j = fromCol; j <= toCol; j++) {
				if (d[i * w * 4 + j * 4]) {
					neighbours++;
				}
			}
		}
		if (neighbours != 3) {
			return true;
		}
		if (row > 0 && row < h - 1 && d[(row - 1) * w * 4 + col * 4] &&
			d[(row + 1) * w * 4 + col * 4])
		{
			return false;
		}
		if (col > 0 && col < w - 1 && d[row * w * 4 + (col - 1) * 4] &&
			d[row * w * 4 + (col + 1) * 4])
		{
			return false;
		}
		return true;
	}
	UINT32 neighbours = 0;
	for (UINT32 i = row - 1; i <= row + 1; i++) {
		for (UINT32 j = col - 1; j <= col + 1; j++) {
			if (d[i * w * 4 + j * 4]) {
				neighbours++;
			}
		}
	}
	if (neighbours == 3) {
		if (d[(row - 1) * w * 4 + col * 4] &&
			d[(row + 1) * w * 4 + col * 4])
		{
			return false;
		}
		if (d[row * w * 4 + (col - 1) * 4] &&
			d[row * w * 4 + (col + 1) * 4])
		{
			return false;
		}
		if (d[(row - 1) * w * 4 + (col - 1) * 4] &&
			d[(row + 1) * w * 4 + (col + 1) * 4])
		{
			return false;
		}
		if (d[(row - 1) * w * 4 + (col + 1) * 4] &&
			d[(row + 1) * w * 4 + (col - 1) * 4])
		{
			return false;
		}
	}
	else if (neighbours == 5) {
		if (d[(row - 1) * w * 4 + col * 4] &&
			d[(row + 1) * w * 4 + col * 4] &&
			d[row * w * 4 + (col - 1) * 4] &&
			d[row * w * 4 + (col + 1) * 4])
		{
			return false;
		}
	}
	return true;
}

bool ShapeParser::parse(vector<Vec3>* vertices, vector<float>* uv,
	vector<UINT16>* indices, Texture* scheme, float wallHeight)
{
	PROFILE("Generating 3D shape. Width=%d, Height=%d, Wall height=%f",
		scheme->getWidth(), scheme->getHeight(), wallHeight);
	UINT32 w = scheme->getWidth();
	UINT32 h = scheme->getHeight();
	UINT16* map = NEW UINT16[w * h];
	fill(map, map + w * h, 0);
	UINT8* raw = scheme->getRawData();
	UINT32 index = 0;
	for (UINT32 row = 0; row < h; row++) {
		for (UINT32 col = 0; col < w; col++) {
			if (isVertexNecessary(scheme, row, col)) {
				vertices->push_back(
					Vec3((float) col, 0.0f, -(float) row));
				vertices->push_back(
					Vec3((float) col, wallHeight, -(float) row));
				map[row * w + col] = index;
				index += 2;
			}
		}
	}
	for (UINT32 row = 0; row < h; row++) {
		for (UINT32 col = 0; col < w; col++) {
			if (raw[row * w * 4 + col * 4] > 0) {
				UINT32 length = 1;
				while (true) {
					UINT32 index = row * w * 4 + col * 4 + length * 4;
					if (col * 4 + length * 4 < w * 4 && raw[index] > 0) {
						length++;
						continue;
					}
					break;
				}
				if (length > 1) {
					UINT16 startBotVertex = map[row * w + col];
					UINT16 startTopVertex = startBotVertex + 1;
					UINT16 endBotVertex = map[row * w + col + length - 1];
					UINT16 endTopVertex = endBotVertex + 1;
					indices->push_back(startBotVertex);
					indices->push_back(endBotVertex);
					indices->push_back(endTopVertex);
					indices->push_back(endTopVertex);
					indices->push_back(startTopVertex);
					indices->push_back(startBotVertex);
					col += length;
				}
			}
		}
	}
	for (UINT32 col = 0; col < w; col++) {
		for (UINT32 row = 0; row < h; row++) {
			if (raw[row * w * 4 + col * 4] > 0) {
				UINT32 length = 1;
				while (true) {
					UINT32 index = (row + length) * w * 4 + col * 4;
					if (row + length < h && raw[index] > 0) {
						length++;
						continue;
					}
					break;
				}
				if (length > 1) {
					UINT16 startBotVertex = map[row * w + col];
					UINT16 startTopVertex = startBotVertex + 1;
					UINT16 endBotVertex =
						map[(row + length - 1) * w + col];
					UINT16 endTopVertex = endBotVertex + 1;
					indices->push_back(startBotVertex);
					indices->push_back(endBotVertex);
					indices->push_back(endTopVertex);
					indices->push_back(endTopVertex);
					indices->push_back(startTopVertex);
					indices->push_back(startBotVertex);
					row += length;
				}
			}
		}
	}
	delete [] map;
	PROFILE("Shape generated. Vertex count: %u, index count: %u.",
		(UINT32) vertices->size(), (UINT32) indices->size());
	return true;
}