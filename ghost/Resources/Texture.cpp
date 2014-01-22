/*
 * Texture.cpp
 *
 *  Created on: 2012.07.24
 *      Author: Martynas Šustavičius
 */

#include "Texture.h"
#include "../ScriptManager.h"
#include "../ResourceManager.h"

const string Texture::ATTR_WRAP_U = "wrap_u";
const string Texture::ATTR_WRAP_V = "wrap_v";
const string Texture::VAL_CLAMP = "clamp";
const string Texture::VAL_REPEAT = "repeat";
const string Texture::ATTR_TYPE = "type";
const string Texture::VAL_MONO = "mono";
const string Texture::VAL_RGBA = "rgba";
 
Texture::Texture(ServiceLocator* services) : Resource(services) {
}

Texture::~Texture() {}

void Texture::line(UINT32 startRow, UINT32 startCol,
	UINT32 endRow, UINT32 endCol, UINT8* color)
{
    INT32 width(endCol - startCol);
    INT8 const ix((width > 0) - (width < 0));
    width = (width < 0 ? (width * -1) : width) << 1;
    INT32 height(endRow - startRow);
	INT8 const iy((height > 0) - (height < 0));
    height = (height < 0 ? (height * -1) : height) << 1;
    setPixel(color, startRow, startCol);
    if (width >= height) {
        INT32 error(height - (width >> 1));
        while (startCol != endCol) {
            if ((error >= 0) && (error || (ix > 0))) {
                error -= width;
                startRow += iy;
            }
            error += height;
            startCol += ix;
			setPixel(color, startRow, startCol);
        }
    }
    else {
        INT32 error(width - (height >> 1));
        while (startRow != endRow) {
            if ((error >= 0) && (error || (iy > 0))) {
                error -= height;
                startCol += ix;
            }
            error += width;
            startRow += iy;
			setPixel(color, startRow, startCol);
        }
    }
}

void Texture::rectangle(UINT32 startRow, UINT32 startCol,
	UINT32 width, UINT32 height, UINT8* color)
{
	for (UINT32 i = startRow; i < height; i++) {
		for (UINT32 j = startCol; j < width; j++) {
			setPixel(color, i, j);
		}
	}
}

void Texture::circle(UINT32 row, UINT32 col, UINT32 radius, UINT8* color) {
	int error = 1 - radius;
	int errorY = 1;
	int errorX = -2 * radius;
	int x = radius, y = 0;
	setPixel(color, row + radius, col);
	setPixel(color, row - radius, col);
	setPixel(color, row, col + radius);
	setPixel(color, row, col - radius);
	while(y < x) {
		// >= 0 produces a slimmer circle. =0 produces the circle picture at radius 11 above
		if(error > 0) {
			x--;
			errorX += 2;
			error += errorX;
		}
		y++;
		errorY += 2;
		error += errorY;    
		setPixel(color, row + y, col + x);
		setPixel(color, row + y, col - x);
		setPixel(color, row - y, col + x);
		setPixel(color, row - y, col - x);
		setPixel(color, row + x, col + y);
		setPixel(color, row + x, col - y);
		setPixel(color, row - x, col + y);
		setPixel(color, row - x, col - y);
	}
}