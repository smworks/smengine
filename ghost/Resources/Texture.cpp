/*
 * Texture.cpp
 *
 *  Created on: 2012.07.24
 *      Author: Martynas Šustavičius
 */

#include "Texture.h"
#include "../ScriptManager.h"
#include "../ResourceManager.h"
#include "TextureRGB.h"
#include "TextureRGBA.h"
#include "TextureMono.h"
#include "AtlasTexture.h"
#include "../../dependencies/includes/libpng/png.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Multiplatform/FileManager.h"
#include "Vertex.h"
#include "../Shapes.h"
#include "../Multiplatform/GraphicsManager.h"

const string Texture::ATTR_WRAP_U = "wrap_u";
const string Texture::ATTR_WRAP_V = "wrap_v";
const string Texture::VAL_CLAMP = "clamp";
const string Texture::VAL_REPEAT = "repeat";
const string Texture::ATTR_TYPE = "type";
const string Texture::VAL_MONO = "mono";
const string Texture::VAL_RGBA = "rgba";
const string Texture::VAL_RGB = "rgb";
 
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
	for (UINT32 i = startRow; i < height + startRow; i++) {
		for (UINT32 j = startCol; j < width + startCol; j++) {
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

void Texture::filledCircle(UINT32 row, UINT32 col, UINT32 radius, UINT8* color) {
	int error = 1 - radius;
	int errorY = 1;
	int errorX = -2 * radius;
	int x = radius, y = 0;
	line(row + radius, col, row - radius, col, color);
	line(row, col + radius, row, col - radius, color);
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
		line(row + y, col + x, row + y, col - x, color);
		line(row - y, col + x, row - y, col - x, color);
		line(row + x, col + y, row + x, col - y, color);
		line(row - x, col + y, row - x, col - y, color);
	}
}

Texture* Texture::load(ServiceLocator* sl, string name, bool useRM) {
	if (useRM) {
		Texture* t = static_cast<Texture*>(sl->getRM()->get(Resource::TEXTURE_2D, name));
		if (t != 0) {
			return t;
		}
	}
	PNGData data = loadPng(sl, (GHOST_SPRITES + name).c_str());
	ASSERT(data.buffer != 0, "Unable to load file \"%s\".", name.c_str());
	Texture* texture = data.alpha ? static_cast<Texture*>(NEW TextureRGBA(sl)) : NEW TextureRGB(sl);
	addDimensions(texture, data.width, data.height);
	texture->getAttributes().setString(ATTR_FILE, name);
	texture->getAttributes().setPointer(ATTR_BUFFER, data.buffer);
	texture->create();
	if (useRM) {
		sl->getRM()->add(name, texture);
	}
	return texture;
}


Texture* Texture::load(ServiceLocator* sl, INT8* buffer) {
	PNGData data = pngToRaw(buffer);
	Texture* texture = data.alpha ? static_cast<Texture*>(NEW TextureRGBA(sl)) : NEW TextureRGB(sl);
	addDimensions(texture, data.width, data.height);
	texture->getAttributes().setPointer(ATTR_BUFFER, data.buffer);
	texture->create();
	return texture;
}

Texture* Texture::createRGBA(ServiceLocator* sl, UINT32 w, UINT32 h) {
	return prepareNewTexture(NEW TextureRGBA(sl), w, h);
}

Texture* Texture::createRGB(ServiceLocator* sl, UINT32 w, UINT32 h) {
	return prepareNewTexture(NEW TextureRGB(sl), w, h);
}

Texture* Texture::createMono(ServiceLocator* sl, UINT32 w, UINT32 h) {
	return prepareNewTexture(NEW TextureMono(sl), w, h);
}

Texture* Texture::createAtlasMono(ServiceLocator* sl, UINT32 w, UINT32 h) {
	Texture* texture = NEW AtlasTexture(sl, Texture::MONO);
	addDimensions(texture, w, h);
	texture->create();
	return texture;
}

Texture::Texture(ServiceLocator* services) :
	Resource(services),
	width_(0),
	height_(0),
	buffer_(0),
	cbo_(0)
{}

Texture::PNGData Texture::loadPng(ServiceLocator* services, const char* path, bool upperLeft) {
	ASSERT(path != 0, "No image file path specified. Unable to load image.");
	SIZE size;
	INT8* bytes = 0;
	services->getFileManager()->loadRaw(bytes, size, path);
	PNGData png = pngToRaw(bytes, upperLeft);
	delete[] bytes;
	ASSERT(png.buffer != 0, "Unable to convert PNG to raw format for path: \"%s\".", path);
	LOGD("Loaded PNG: %s.", path);
	return png;
}

bool Texture::create() {
	ASSERT(getServiceLocator() != 0, "Service locator not specified for TextureRGB object.");
	string file = getAttribute(ATTR_FILE);
	buffer_ = reinterpret_cast<UINT8*>(getAttributes().getPointer(ATTR_BUFFER));
	width_ = getAttributes().getInt(Texture::ATTR_WIDTH, 0);
	height_ = getAttributes().getInt(Texture::ATTR_HEIGHT, 0);
	if (buffer_ == 0 && file.length() > 0) {
		PNGData png = loadPng(getServiceLocator(), file.c_str());
		RETURN(false, png.buffer == 0, "Unable to load file \"%s\".", file.c_str());
		buffer_ = reinterpret_cast<UINT8*>(png.buffer);
		width_ = png.width;
		height_ = png.height;
	}
	if (width_ <= 0 || height_ <= 0) {
		LOGE("Texture height or width unspecified or less than 1 pixel.");
		delete[] buffer_;
		buffer_ = 0;
		return false;
	}
	if (buffer_ == 0) {
		buffer_ = createBuffer(width_, height_);
	}
	vector<VertexPT>* cbo = static_cast<vector<VertexPT>*>(
        Shapes::getShape(Shapes::SHAPE_SCREEN_PLANE, Shapes::VERTEX_POS_TEX));
    getServiceLocator()->getGraphicsManager()->setVertexBuffer(
        cbo_, &(*cbo)[0], (UINT32) cbo->size() * sizeof(VertexPT));
    delete cbo;
	LOGD("Loaded texture: %s.", file.c_str());
	return true;
}


void Texture::addDimensions(Texture* texture, UINT32 w, UINT32 h) {
	ASSERT(w > 0 && h > 0, "Texture dimensions must be bigger than 0.");
	texture->getAttributes().setInt(Texture::ATTR_WIDTH, w);
	texture->getAttributes().setInt(Texture::ATTR_HEIGHT, h);
}

Texture* Texture::prepareNewTexture(Texture* texture, UINT32 w, UINT32 h) {
	addDimensions(texture, w, h);
	texture->create();
	return texture;
}

struct PngBuffer {
	INT8* buffer;
	SIZE bytesRead;
};

void readPng(png_structp png, png_bytep data, png_size_t size) {
#ifdef ENABLE_GRAPHICS
	png_voidp a = png_get_io_ptr(png);
	PngBuffer* pngBuffer = (PngBuffer*) a;
	memcpy(data, pngBuffer->buffer + pngBuffer->bytesRead, size);
	pngBuffer->bytesRead += size;
#endif
}

#define HEADER_SIZE 8
Texture::PNGData Texture::pngToRaw(INT8* in, bool upperLeft) {
	PNGData png;
#ifdef ENABLE_GRAPHICS
	int res;
	ASSERT(in != 0, "No PNG byte buffer specified.");
	png_byte header[HEADER_SIZE];
	memcpy(header, in, HEADER_SIZE);
	res = png_sig_cmp(header, 0, HEADER_SIZE);
	ASSERT(res == 0, "Specified PNG byte buffer does not contain valid PNG header.");
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	ASSERT(png_ptr, "Unable to create PNG structure.");
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		LOGE("Unable to create PNG info.");
		return png;
	}
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		LOGE("Unable to create PNG end info");
		return png;
	}
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		LOGE("Error during setjmp.");
		return png;
	}
	// Initialize PNG data reader.
	PngBuffer pngBuffer;
	pngBuffer.buffer = in;
	pngBuffer.bytesRead = HEADER_SIZE;
	png_set_read_fn(png_ptr, (png_voidp) &pngBuffer, readPng);
	// Let libpng know you already read the first 8 bytes.
	png_set_sig_bytes(png_ptr, HEADER_SIZE);
	// Read all the info up to the image data.
	png_read_info(png_ptr, info_ptr);
	// Variables to pass to get info.
	int bit_depth, colorType;
	png_uint_32 pngW, pngH;
	// Get info about PNG.
	png_get_IHDR(png_ptr, info_ptr, &pngW, &pngH, &bit_depth, &colorType,
	NULL, NULL, NULL);
	// Log image info.
	string col = "Unknown";

	if (colorType == PNG_COLOR_TYPE_GRAY) {
		col = "PNG_COLOR_TYPE_GRAY";
	} else if (colorType == PNG_COLOR_TYPE_PALETTE) {
		col = "PNG_COLOR_TYPE_PALETTE";
	} else if (colorType == PNG_COLOR_TYPE_RGB) {
		col = "PNG_COLOR_TYPE_RGB";
	} else if (colorType == PNG_COLOR_TYPE_RGB_ALPHA) {
		col = "PNG_COLOR_TYPE_RGB_ALPHA";
		png.alpha = true;
	} else if (colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
		col = "PNG_COLOR_TYPE_GRAY_ALPHA";
		png.alpha = true;
	}

	if (bit_depth == 16) {
		LOGD("Image channel size is 16 bit, so striping it to 8.");
        png_set_strip_16(png_ptr);
	}
	if (colorType == PNG_COLOR_TYPE_PALETTE) {
		LOGD("Converting png from 8 bit to 24.");
		png.alpha = false;
        png_set_palette_to_rgb(png_ptr);
	}
	if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
		LOGD("Converting png from grey to rgb.");
		png_set_gray_to_rgb(png_ptr);
	}
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
		LOGD("Adding alpha channel to RGB image.");
		png.alpha = true;
		png_set_tRNS_to_alpha(png_ptr);
	}
	LOGD("Image. Width=%d, height=%d, bit depth=%d, color type=%s", pngW, pngH, bit_depth, col.c_str());
	png.width = pngW;
	png.height = pngH;
	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);
	// Row size in bytes.
	SIZE rowbytes = png_get_rowbytes(png_ptr, info_ptr);
	// Allocate the image_data as a big block, to be given to opengl.
	SIZE size = rowbytes * png.height;
	png_byte *image_data = NEW png_byte[size];
	if (!image_data) {
		// Clean up memory and close stuff.
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		LOGE("Unable to allocate image_data.");
		return png;
	}
	// Row_pointers is for pointing to image_data for reading the png with libpng.
	png_bytep *row_pointers = NEW png_bytep[png.height];
	if (!row_pointers) {
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		delete[] image_data;
		LOGE("Unable to allocate row_pointer");
		return png;
	}
	// Set the individual row_pointers to point at the correct offsets of image_data.
	if (upperLeft) {
		for (UINT32 i = 0; i < png.height; ++i) {
			row_pointers[i] = image_data + i * rowbytes;
		}
	} else {
		for (UINT32 i = 0; i < png.height; ++i) {
			row_pointers[png.height - 1 - i] = image_data + i * rowbytes;
		}
	}
	// Read the png into image_data through row_pointers.
	png_read_image(png_ptr, row_pointers);
	INT8* buffer = NEW INT8[size];
	memcpy(buffer, image_data, size);
	// Clean up memory and close stuff
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	delete[] image_data;
	delete[] row_pointers;
	png.buffer = reinterpret_cast<UINT8*>(buffer);
#endif
	return png;
}