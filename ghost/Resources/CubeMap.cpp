/*
 * TextureRGBA.cpp
 *
 *  Created on: 2012.06.30
 *      Author: Martynas Ðustavièius
 */

#include "CubeMap.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Multiplatform/GraphicsManager.h"
#include "../Multiplatform/FileManager.h"
#include "../Utils.h"
#include "Texture.h"

CubeMap::CubeMap(ServiceLocator* services) : Resource(services), id_(0) {}

CubeMap::~CubeMap() {
	release();
}

void CubeMap::release() {
	if (id_ != 0) {
		getServiceLocator()->getGraphicsManager()->unsetTexture(id_);
		id_ = 0;
	}
}

SIZE CubeMap::getSize() {
	return sizeof(CubeMap);
}

Resource::Type CubeMap::getType() {
	return Resource::CUBE_MAP;
}

bool CubeMap::isValid() {
	return id_ > 0;
}

UINT32 CubeMap::getId() {
	return id_;
}

bool CubeMap::create() {
	RETURN(false, true, "CUBEMAP not properly implemented");
	//vector<string> files;
	//stringSplit(getAttribute(ATTR_FILE), files, ',');
	//if (files.size() != 6) {
	//	LOGW("Cube map does not contain 6 textures. Contained textures: %u.", (UINT32) files.size());
	//	return false;
	//}
	//UINT8** sides = NEW UINT8*[6];
	//UINT32 width = 0, height = 0;
	//for (UINT32 i = 0; i < 6; i++) { // TODO: fix issue with last parameter true.
	//	bool alpha;
	//	sides[i] = reinterpret_cast<UINT8*>(loadPng(getServiceLocator(),
	//		(GHOST_SPRITES + files[i]).c_str(), width, height, alpha, true));
	//}
	//int num = width;
	//while (num > 2) {
	//	num /= 2;
	//}
	//bool success = true;
	//if (num != 2) {
	//	LOGW("Cube texture width (%d) is "
	//		"not a power of two.", width);
	//	success = false;
	//}
	//if (success && !getServiceLocator()->getGraphicsManager()->setCubeMap(
	//	id_, sides, width, height, false, false, true))
	//{
	//	LOGW("Unable to create cube map.");
	//	success = false;
	//}
	//for (int i = 0; i < 6; i++) {
	//	delete [] sides[i];
	//}
	//delete [] sides;
	//return success;
}