/*
 * TextureAtlas.cpp
 *
 *  Created on: 2013.07.16
 *      Author: Martynas Šustavičius
 */

#include "TextureAtlas.h"
#include "Multiplatform/ServiceLocator.h"
#include "Multiplatform/GraphicsManager.h"
#include "Shapes.h"
#include "Utils.h"
#include "Resources/TextureRGBA.h"
#include "Resources/TextureMono.h"

TextureAtlas::TextureAtlas(ServiceLocator* services) :
    idCounter_(0),
    services_(services),
    textures_(0),
    it_(textures_.begin()),
    textureRGBA_(0),
	textureMono_(0)

{
	textureRGBA_ = NEW TextureRGBA(services);
	textureRGBA_->create(STARTING_WIDTH, STARTING_HEIGHT);
	textureMono_ = NEW TextureMono(services);
	textureMono_->create(STARTING_WIDTH, STARTING_HEIGHT);
	byteMap_ = NEW bool[STARTING_WIDTH * STARTING_HEIGHT];
	skylineRGBA_.push_back(Skyline(0, 0, STARTING_WIDTH));
	skylineMono_.push_back(Skyline(0, 0, STARTING_WIDTH));
	LOGD("Created texture atlas.");
}

TextureAtlas::~TextureAtlas() {
	for (UINT32 i = 0; i < textures_.size(); i++) {
		services_->getGraphicsManager()->unsetVertexBuffer(textures_[i].cbo);
	}
	delete [] byteMap_;
	delete textureMono_;
	delete textureRGBA_;
	LOGD("Deleted texture atlas.");
}

bool TextureAtlas::create(UINT32& id, UINT32 width, UINT32 height, int type) {
	Texture* texture = type == Texture::MONO ?
		dynamic_cast<Texture*>(textureMono_) : dynamic_cast<Texture*>(textureRGBA_);
    TextureHeader th;
    th.width = width;
    th.height = height;
	th.type = type;
    UINT8 white[] = {static_cast<UINT8>(type == Texture::MONO ? 0 : 255), 255, 255, 255};
    if (findSpace(th)) {
		texture->rectangle(
			th.offsetRow, th.offsetCol, th.width, th.height, white);
		float positions[] = {
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f
        };
		float y = (float) th.offsetRow / texture->getWidth();
		float h = (float) th.height / texture->getHeight();
		float x = (float) th.offsetCol / texture->getWidth();
		float w = (float) th.width / texture->getHeight();
		float uv[] = {
            x, y,
            x + w, y,
            x + w, y + h,
            x + w, y + h,
            x, y + h,
            x, y
		};
		int vertexCount = sizeof(positions) / 3;
        vector<VertexPT>* cboArr = NEW vector<VertexPT>();
        cboArr->reserve(vertexCount);
        VertexPT v;
        for (int i = 0; i < vertexCount; i++) {
            v.pos[0] = positions[i * 3 + 0];
            v.pos[1] = positions[i * 3 + 1];
            v.pos[2] = positions[i * 3 + 2];
            v.uv[0] = uv[i * 2 + 0];
            v.uv[1] = uv[i * 2 + 1];
            cboArr->push_back(v);
        }
		services_->getGraphicsManager()->setVertexBuffer(
			th.cbo, &(*cboArr)[0], (UINT32) cboArr->size() * sizeof(VertexPT));
        th.id = idCounter_++;
        textures_.push_back(th);
        id = th.id;
		delete cboArr;
		return true;
    }
	LOGE("There is not enough free space in texture atlas.");
    return false;
}

bool TextureAtlas::clear(UINT32 id) {
    it_ = textures_.begin();
	while (it_ != textures_.end()) {
		if ((*it_).id == id) {
            TextureHeader& th = (*it_);
			Texture* texture = th.type == Texture::MONO ?
				dynamic_cast<Texture*>(textureMono_) : dynamic_cast<Texture*>(textureRGBA_);
            UINT8 white[] = {0, 255, 0, 255};
            for (UINT32 i = th.offsetRow; i < th.offsetRow + th.height; i++) {
                for (UINT32 j = th.offsetCol; j < th.offsetCol + th.width; j++) {
                    texture->setPixel(white, i, j);
                }
            }
			services_->getGraphicsManager()->unsetVertexBuffer((*it_).cbo);
			textures_.erase(it_);
			return true;
		}
		it_++;
	}
    return false;
}

TextureAtlas::TextureHeader& TextureAtlas::getTextureHeader(UINT32& id) {
    for (UINT32 i = 0; i < textures_.size(); i++) {
        if (textures_[i].id == id) {
            return textures_[i];
        }
    }
    static TextureHeader empty;
    return empty;
}

UINT32 TextureAtlas::getId(int type) {
	return type == Texture::MONO ? textureMono_->getId() : textureRGBA_->getId();
}

Texture* TextureAtlas::getTexture(int type) {
	return type == Texture::MONO ?
		dynamic_cast<Texture*>(textureMono_) : dynamic_cast<Texture*>(textureRGBA_);
}

bool TextureAtlas::findSpace(TextureHeader& th) {
	Texture* texture = th.type == Texture::MONO ?
		dynamic_cast<Texture*>(textureMono_) : dynamic_cast<Texture*>(textureRGBA_);
	vector<Skyline>& skyline = Texture::MONO ? skylineMono_ : skylineRGBA_;
	UINT32 height = texture->getHeight();
	SIZE size = skyline.size();
	INT32 fitIndex = -1;
	for (SIZE i = 0; i < size; i++) {
		Skyline& sl = skyline[i];
		if (th.width > sl.width || th.height + sl.row > height) {
			continue;
		}
		fitIndex = (INT32) i;
		break;
	}
	if (fitIndex == -1) {
		LOGW("Unable to find space in atlas for texture.");
		return false;
	}
	Skyline& sl = skyline[fitIndex];
	th.offsetRow = sl.row;
	th.offsetCol = sl.col;
	if (th.width == sl.width) {
		sl.row += th.height;
		return true;
	}
	Skyline first(sl.row + th.height, sl.col, th.width);
	sl.col += th.width;
	sl.width -= th.width;
	skyline.insert(skyline.begin() + fitIndex, first);
	return true;
}