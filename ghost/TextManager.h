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

class ServiceLocator;
class Texture;
class Symbol;

#ifndef ENABLE_FONTS
class TextManager {
public:
	TextManager(ServiceLocator* services) {}
	~TextManager() {}
	void setFontSize(SIZE size) {}
	Texture* getText(const string& text, int size) { return 0; }
	Symbol* getSymbol(char symbol) { return 0; }
	INT32 getMaxHeight(string text) { return 0; }
};
#else

#include "../dependencies/includes/freetype/ft2build.h"
#include "../dependencies/includes/freetype/freetype.h"
class Shader;

#define SYMBOL_CACHE_SIZE 256

class TextManager {
public:
	TextManager(ServiceLocator* services);
	~TextManager();

	/**
	 * Specify font size in pixels.
	 * @param size - size in pixels.
	 */
	void setFontSize(SIZE size);

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
	float vertices_[18];
private:
	FT_Library lib_;
	FT_Error err_;
	FT_Face face_;
	INT8* fontBuffer_;
	ServiceLocator* services_;
	SIZE fontSize_;
	Symbol*** symbolCache_;
	bool error_;
};

#endif
#endif