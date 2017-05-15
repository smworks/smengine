#include "Texture.h"
#include "../ScriptManager.h"
#include "../ResourceManager.h"
#include "TextureRGB.h"
#include "TextureRGBA.h"
#include "TextureMono.h"
#include "AtlasTexture.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Multiplatform/FileManager.h"
#include "Vertex.h"
#include "../Shapes.h"
#include "../Multiplatform/GraphicsManager.h"
#include "../Multiplatform/ImageLoader.h"

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
    ImageLoader::RawImage data = ImageLoader::loadPng(sl, (GHOST_SPRITES + name).c_str());
	ASSERT(data.buffer != 0, "Unable to load file \"%s\".", name.c_str());
	Texture* texture = data.alpha ? static_cast<Texture*>(NEW TextureRGBA(sl)) : static_cast<Texture*>(NEW TextureRGB(sl));
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
    LOGW("Do not use this method as it only provides the buffer for PNG "
         "and is otherwise incompatible with other formats");
//    ImageLoader::RawImage data = ImageLoader::pngToRaw(buffer);
//	Texture* texture = data.alpha ? static_cast<Texture*>(NEW TextureRGBA(sl)) : NEW TextureRGB(sl);
//	addDimensions(texture, data.width, data.height);
//	texture->getAttributes().setPointer(ATTR_BUFFER, data.buffer);
//	texture->create();
//	return texture;
    return 0;
}

Texture* Texture::createRGBA(ServiceLocator* sl, string name, UINT32 w, UINT32 h) {
	return prepareNewTexture(NEW TextureRGBA(sl), name, w, h);
}

Texture* Texture::createRGB(ServiceLocator* sl, string name, UINT32 w, UINT32 h) {
	return prepareNewTexture(NEW TextureRGB(sl), name, w, h);
}

Texture* Texture::createMono(ServiceLocator* sl, string name, UINT32 w, UINT32 h) {
	return prepareNewTexture(NEW TextureMono(sl), name, w, h);
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

bool Texture::create() {
	string file = getAttribute(ATTR_FILE);
	buffer_ = reinterpret_cast<UINT8*>(getAttributes().getPointer(ATTR_BUFFER));
	width_ = getAttributes().getInt(Texture::ATTR_WIDTH, 0);
	height_ = getAttributes().getInt(Texture::ATTR_HEIGHT, 0);
	if (buffer_ == 0 && file.length() > 0 && getFileManager()->fileExists(file.c_str())) {
        ImageLoader::RawImage png = ImageLoader::loadPng(getServiceLocator(), file.c_str());
		RETURN(false, png.buffer == 0, "Unable to load file \"%s\".", file.c_str());
		buffer_ = reinterpret_cast<UINT8*>(png.buffer);
		width_ = png.width;
		height_ = png.height;
        LOGD("Loaded texture: %s.", file.c_str());
	}
	else if (width_ <= 0 || height_ <= 0) {
		LOGE("Texture height or width unspecified or less than 1 pixel.");
		delete[] buffer_;
		buffer_ = 0;
		return false;
	}
	if (buffer_ == 0) {
		buffer_ = createBuffer(width_, height_);
        LOGD("Created texture: %s.", file.c_str());
	}
	vector<VertexPT>* cbo = static_cast<vector<VertexPT>*>(
        Shapes::getShape(Shapes::SHAPE_SCREEN_PLANE, Shapes::VERTEX_POS_TEX));
    getGraphicsManager()->setVertexBuffer(
        cbo_, &(*cbo)[0], (UINT32) cbo->size() * sizeof(VertexPT));
    delete cbo;
	
	return true;
}


void Texture::addDimensions(Texture* texture, UINT32 w, UINT32 h) {
	ASSERT(w > 0 && h > 0, "Texture dimensions must be bigger than 0.");
	texture->getAttributes().setInt(Texture::ATTR_WIDTH, w);
	texture->getAttributes().setInt(Texture::ATTR_HEIGHT, h);
}

Texture* Texture::prepareNewTexture(Texture* texture, string name, UINT32 w, UINT32 h) {
	addDimensions(texture, w, h);
    texture->setAttribute(Texture::ATTR_FILE, name);
	texture->create();
	return texture;
}