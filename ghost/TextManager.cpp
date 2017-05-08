#include "TextManager.h"

#ifdef ENABLE_FONTS

#include "Multiplatform/ServiceLocator.h"
#include "Multiplatform/FileManager.h"
#include "Resources/Symbol.h"
#include "Resources/Texture.h"
#include "ResourceManager.h"
#include "Shapes.h"
#include "../dependencies/includes/freetype/ftglyph.h"

struct GlyphObject {
	UINT8* buffer;
	SIZE width;
	SIZE height;
	long kerningOffset;
	int offsetLeft;
	int offsetTop;
	long advanceX;
	long advanceY;
	int row;
};

TextManager::TextManager(ServiceLocator* services) :
	services(services),
	fontSize_(0),
	symbolCache_(0),
	error_(false)
{
	float vertices[] = {
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	};
	memcpy(vertices, vertices, 18 * sizeof(float));
	err_ = FT_Init_FreeType(&lib_);
	if (err_) {
		LOGW("Failed to initialize font library.");
		error_ = true;
		return;
	}
	SIZE size;
	string path = GHOST_FONTS + string("arial.ttf");
	if (services->getFileManager()->loadRaw(
		fontBuffer_, size, path.c_str()))
	{
		LOGD("Loading font with size: %u.", (UINT32) size);
		err_ = FT_New_Memory_Face(
			lib_, (const FT_Byte*) fontBuffer_, size, 0, &face_);
		if (err_) {
			LOGW("Unable to load font from memory. Error code: %d.", err_);
			error_ = true;
			return;
		}
		err_ = FT_Select_Charmap(face_, FT_ENCODING_UNICODE);
		if (err_) {
			LOGW("Unable to set charmap encoding to unicode.");
		}
		setFontSize(12);
	} else {
		LOGE("Unable to load font \"%s\" from file.", path.c_str());
		error_ = true;
		return;
	}
	symbolCache_ = NEW Symbol**[SYMBOL_CACHE_SIZE];
	for (SIZE i = 0; i < SYMBOL_CACHE_SIZE; i++) {
		symbolCache_[i] = NEW Symbol*[SYMBOL_CACHE_SIZE];
		for (SIZE j = 0; j < SYMBOL_CACHE_SIZE; j++) {
			symbolCache_[i][j] = 0;
		}
	}
	LOGD("Created text manager.");
}

TextManager::~TextManager() {
	if (symbolCache_ != 0) {
		for (SIZE i = 0; i < SYMBOL_CACHE_SIZE; i++) {
			delete [] symbolCache_[i];
		}
		delete [] symbolCache_;
	}
	err_ = FT_Done_FreeType(lib_);
	if (err_) {
		LOGW("Failed to release font library.");
	}
	delete [] fontBuffer_;
	LOGD("Deleted text manager.");
}

void TextManager::setFontSize(SIZE size) {
	if (error_) {
		LOGW("Unable to call setFontSize(), because text manager not initialized properly.");
		return;
	}
	if (size != fontSize_) {
		fontSize_ = size;
		err_ = FT_Set_Pixel_Sizes(face_, 0, (FT_UInt) fontSize_);
		if (err_) {
			LOGE("Unable to set text size. Freetype error code: %d.", err_);
		}
	}
}

Texture* TextManager::getText(const string& text, int size) {
	if (error_) {
		LOGW("Unable to call getText(), because text manager not initialized properly.");
		return 0;
	}
	setFontSize(size);
	string name = "cache_";
	name += text;
	if (services->getRM()->has(Resource::TEXTURE_2D, name)) {
		Texture* texture = dynamic_cast<Texture*>(
			services->getRM()->get(Resource::TEXTURE_2D, name));
		if (texture != 0) {
			return texture;
		}
	}
	vector<GlyphObject> glyphs;
	FT_UInt previous = 0;
	int width = 0;
	int row = 0;
	FT_GlyphSlot glyphSlot = face_->glyph;
	FT_Vector delta;
	int heightOffset = 0;
	int widthOffset = 0;
	long yOffset = 0; //- face_->bbox.yMin >> 6;
	long maxHeight = 0;
	for (UINT32 i = 0; i < text.length(); i++) {
		if (text[i] == '\n') {
			widthOffset = 0;
			row++;
			heightOffset += fontSize_;
			continue;
		}
		err_ = FT_Load_Char(face_, text[i], FT_LOAD_RENDER);
		if (err_) {
			LOGW("Unable to select, load and render character."
				" Error code: %d", err_);
			continue;
		}
		FT_Bitmap bitmap = glyphSlot->bitmap;
		FT_UInt glyphIndex = FT_Get_Char_Index(face_, text[i]);
		err_ = FT_Get_Kerning(face_, previous, glyphIndex,
			FT_KERNING_DEFAULT, &delta);
		if (err_) {
			LOGW("Unable to get kerning for character."
				" Error code: %d", err_);
			continue;
		}
		FT_Glyph glyph;
		FT_BBox box;
		FT_Get_Glyph(glyphSlot, &glyph);
		FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_UNSCALED, &box);
		if ((box.yMin >> 6) < yOffset) {
			yOffset = box.yMin >> 6;
		}
		if ((box.yMax >> 6) - (box.yMin >> 6) > maxHeight) {
			maxHeight = (box.yMax >> 6) - (box.yMin >> 6);
		}
		GlyphObject tmp;
		tmp.kerningOffset = delta.x >> 6;
		tmp.buffer = NEW UINT8[bitmap.rows * bitmap.width];
		memcpy(tmp.buffer, bitmap.buffer, bitmap.rows * bitmap.width);
		tmp.height = bitmap.rows;
		tmp.width = bitmap.width;
		tmp.row = row;
		tmp.offsetLeft = glyphSlot->bitmap_left;
		if (tmp.offsetLeft < 0) {
			tmp.offsetLeft = 0;
		}
		tmp.offsetLeft += widthOffset + tmp.kerningOffset;
		tmp.offsetTop = glyphSlot->bitmap_top;
		tmp.advanceX = glyphSlot->advance.x >> 6;
		tmp.advanceY = glyphSlot->advance.y >> 6;
		glyphs.push_back(tmp);
		previous = glyphIndex;
		widthOffset += tmp.advanceX + tmp.kerningOffset;
		if (widthOffset > width) {
			width += widthOffset;
		}
		FT_Done_Glyph(glyph);
	}
	yOffset = yOffset * -1;
	SIZE height = heightOffset + fontSize_;
	Texture* texture = Texture::createAtlasMono(services, width, (UINT32) height);
	UINT8 color[] = {0};
	texture->rectangle(0, 0, width, (UINT32) height, color);
	for (SIZE i = 0; i < glyphs.size(); i++) {
		GlyphObject g = glyphs[i];
		//LOGI("Symbol: %c, top offset: %d, left offset: %d."
		//		" Width: %d, height: %d, advanceX: %d, advanceY: %d"
		//		", kerning offset: %d, ymin: %d. "
		//		"Height: %d",
		//	text[i], g.offsetTop, g.offsetLeft,
		//	g.width, g.height,
		//	g.advanceX, g.advanceY, g.kerningOffset, yOffset, height);
    
        for (SIZE row = 0; row < g.height; row++) {
			for (SIZE col = 0; col < g.width; col++) {
				color[0] = g.buffer[row * g.width + col];
				if (color[0] > 0) {
					texture->setPixel(color,
						(height - 1) - fontSize_ + g.offsetTop + yOffset
							+ (fontSize_ - maxHeight) / 2
							- row - g.row * fontSize_,
						g.offsetLeft + col);
				}
			}
		}
		delete [] g.buffer;
	}
	//stringstream ss("\n");
	//for (int i = 0; i < texture->getHeight(); i++) {
	//	for (int j = 0; j < texture->getWidth(); j++) {
	//		int pixel = (int) texture->getPixel(i, j)[0];
	//		ss << (pixel < 100 ? pixel < 10 ? "  " : " " : "") << pixel;
	//	}
	//	ss << "\n";
	//}
	//LOGI("%s", ss.str().c_str());
	services->getRM()->add(name, texture);
	return texture;
}

Symbol* TextManager::getSymbol(char symbol) {
	SIZE index = (SIZE) symbol;
	if (fontSize_ < SYMBOL_CACHE_SIZE && index < SYMBOL_CACHE_SIZE &&
		symbolCache_[fontSize_][index] != 0)
	{
		return symbolCache_[fontSize_][index];
	}
	stringstream ss;
	ss << "symbol_" << symbol << fontSize_;
	string name = ss.str();
	Symbol* cachedSymbol = dynamic_cast<Symbol*>(
		services->getRM()->get(Resource::SYMBOL, name));
	if (cachedSymbol != 0) {
		return cachedSymbol;
	}
	Symbol* s = NEW Symbol(services);
	s->create();
	FT_GlyphSlot glyphSlot = face_->glyph;
	err_ = FT_Load_Char(face_, symbol,  FT_LOAD_RENDER);
	if (err_) {
		LOGW("Unable to load character."
			" Error code: %d", err_);
		return 0;
	}
	FT_Bitmap bitmap = glyphSlot->bitmap;
	FT_Glyph glyph;
	FT_Get_Glyph(glyphSlot, &glyph);
		err_ = FT_Load_Char(face_, symbol,  FT_LOAD_RENDER);
	if (err_) {
		LOGW("Unable to get glyph."
			" Error code: %d", err_);
		return 0;
	}
	s->setWidth(bitmap.width);
	s->setHeight(bitmap.rows);
	s->setOffsetX(glyphSlot->bitmap_left);
	s->setOffsetY(glyphSlot->bitmap_top - bitmap.rows);
	s->setAdvance(glyphSlot->advance.x >> 6);
	s->setFontSize(fontSize_);
	if (s->getWidth() > 0 && s->getHeight() > 0) {
		Texture* texture = Texture::createAtlasMono(services, s->getWidth(), s->getHeight());
		UINT8 color[] = {0};
		texture->rectangle(0, 0, texture->getWidth(), texture->getHeight(), color);
		for (int row = 0; row < bitmap.rows; row++) {
			for (int col = 0; col < bitmap.width; col++) {
				color[0] = bitmap.buffer[row * bitmap.width + col];
				if (color[0] > 0) {
					texture->setPixel(color, bitmap.rows - row - 1, col);
				}
			}
		}
		services->getRM()->add(name, texture);
		texture->commit();
		s->setTexture(texture);
	}
	services->getRM()->add(name, s);
	FT_Done_Glyph(glyph);
	if (fontSize_ < SYMBOL_CACHE_SIZE && index < SYMBOL_CACHE_SIZE ) {
		symbolCache_[fontSize_][index] = s;
	}
	return s;
}

INT32 TextManager::getMaxHeight(string text) {
	INT32 max = 0;
	for (SIZE i = 0; i < text.length(); i++) {
		Symbol* symbol = getSymbol(text[i]);
		INT32 temp = symbol->getHeight() - symbol->getOffsetY();
		if (temp > max) {
			max = temp;
		}
	}
	return max;
}

#endif