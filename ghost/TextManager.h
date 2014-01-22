/*
 * TextManager.h
 *
 *  Created on: 2012.07.11
 *      Author: Martynas Šustavičius
 */

#ifndef TEXTMANAGER_H_
#define TEXTMANAGER_H_

#include "Multiplatform/Ghost.h"
#include "Vec3.h"

#include "../dependencies/includes/freetype/ft2build.h"
#include "../dependencies/includes/freetype/freetype.h"
class Shader;
class ServiceLocator;
class Texture;
class Symbol;

#define SYMBOL_CACHE_SIZE 256

class TextManager {
public:
	TextManager(ServiceLocator* services);
	~TextManager();

	/**
	 * Specify font size in pixels.
	 * @param size - size in pixels.
	 */
	void setFontSize(int size);

	/**
	 * @param text - text that will be rendered on texture.
	 * @param size - size of the text in pixels.
	 * @return Texture with specified text.
	 */
	Texture* getText(const string& text, int size);

	/**
	 * @param symbol - symbol code.
	 * @return Structure describing symbol.
	 */
	Symbol* getSymbol(char symbol);

	/**
	 * @param text - string that will be measured for height.
	 * @return Maximum text height in pixels.
	 */
	INT32 getMaxHeight(string text);
public:
	GLfloat vertices_[18];
private:
	FT_Library lib_;
	FT_Error err_;
	FT_Face face_;
	INT8* fontBuffer_;
	ServiceLocator* services_;
	int fontSize_;
	Symbol*** symbolCache_;
};

#endif
