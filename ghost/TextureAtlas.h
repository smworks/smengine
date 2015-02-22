/*
 * TextureAtlas.h
 *
 *  Created on: 2013.07.16
 *      Author: Martynas Šustavičius
 */

#ifndef TEXTUREATLAS_H_
#define TEXTUREATLAS_H_

#include "Multiplatform/Ghost.h"
class Texture;
class ServiceLocator;

/**
 * This class stores smaller textures of
 * variable size in bigger ones of appropriate type.
 */
class TextureAtlas {
public:
	static const UINT16 STARTING_WIDTH = 512; //2048;
	static const UINT16 STARTING_HEIGHT = 512; //2048;
	struct TextureHeader {
	    UINT32 id;
		UINT32 offsetRow, offsetCol;
		UINT32 width, height;
		UINT32 cbo;
		float uv[12];
		int type;
	};
	struct Skyline {
		Skyline(UINT32 offsetRow, UINT32 offsetCol, UINT32 lineWidth) :
			row(offsetRow), col(offsetCol), width(lineWidth) {}
		UINT32 row;
		UINT32 col;
		UINT32 width;
#ifdef SMART_DEBUG
		void print() {
			LOGD("Line offset row: %d, offset col: %d, width: %d",
				row, col, width);
		}
#endif
	};
public:
    TextureAtlas(ServiceLocator* services);
    ~TextureAtlas();

	/**
	 * Allocates space on texture atlas for specified
	 * dimension texture.
	 * @param id - id of the texture.
     * @param cbo - id of vertex position and UV buffer.
	 * @param width - texture width in pixels.
	 * @param height - texture height in pixels.
	 * @param type - texture type (Texture::Type).
	 * @return True if allocation was sucessful.
	 */
    bool create(UINT32& id, UINT32 width, UINT32 height, int type);

    /**
     * @param id - id of the texture.
     * @return True if texture was removed.
     */
    bool clear(UINT32 id);

    /**
     * @param id - id of the texture.
     * @return Texture header that belongs to texture
     * with specified id.
     */
    TextureHeader& getTextureHeader(UINT32& id);

    /**
	 * @param type - type of texture atlas (Texture::Type).
     * @return Id of texture atlas texture.
     */
     UINT32 getId(int type);

	 /**
	  * @param type - type of the texture (Texture::Type).
	  * @return Texture that is used for atlas of specified type.
	  */
	 Texture* getTexture(int type);
private:
	/**
	 * Method searches if atlas contains enough free space
	 * for new texture. Algorithm used: skyline.
	 * @param textureHeader - header that will contain
	 * valid offets in atlas if enough space for texture is
	 * found.
	 * @return True if enough space was found in atlas.
	 */
	bool findSpace(TextureHeader& textureHeader);
private:
    /** Id counter. */
    UINT32 idCounter_;
    /** Handle to all engine services. */
    ServiceLocator* services_;
    /** Array of contained textures. */
	vector<TextureHeader> textures_;
	/** Texture iterator. */
	vector<TextureHeader>::iterator it_;
	/** Texture that contains other RGBA textures. */
	Texture* textureRGBA_;
	/** Texture that contains other monochrome textures. */
	Texture* textureMono_;
	/** Byte array used to find free space. */
	bool* byteMap_;
	/** RGBA texture skyline. */
	vector<Skyline> skylineRGBA_;
	/** Monochrome texture skyline. */
	vector<Skyline> skylineMono_;
};

#endif
