/*
 * ShapeParser.h
 *
 *  Created on: 2013.02.16
 *      Author: Martynas Šustavičius
 */

#ifndef SHAPEPARSER_H_
#define SHAPEPARSER_H_

#include "Multiplatform/Ghost.h"
class Vec3;
class Texture;

class ShapeParser {
public:
	static bool parse(vector<Vec3>* vertices, vector<float>* uv,
		vector<UINT16>* indices, Texture* scheme,
		float wallHeight);
};

#endif
