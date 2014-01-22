/*
 * MD2Parser.cpp
 *
 *  Created on: 2012.12.27
 *      Author: MS
 */

#include "MD2Parser.h"

bool MD2Parser::parse(MD2Object& obj, const string& file, ServiceLocator* services) {
	INT8* data;
	SIZE size;
	services->getFileManager()->loadRaw(data, size, (GHOST_MODELS + file).c_str());
	if (data == 0) {
		LOGW("Failed to load file %s.", file.c_str());
		return false;
	}
	MD2Header header;
	memcpy((char*) &header, data, sizeof(MD2Header));
	if (header.id != MD2_ID
		|| header.version != MD2_VERSION) {
		LOGW("File %s is not valid MD2 file.", file.c_str());
		delete[] data;
		return false;
	}
	LOGD("Id: %d.", header.id);
	LOGD("Version: %d.", header.version);
	LOGD("Skin width: %d.", header.skinWidth);
	LOGD("Skin height: %d.", header.skinHeight);
	LOGD("Frame size: %d.", header.frameSize);
	LOGD("Skin count: %d.", header.numSkins);
	LOGD("Vertex count: %d.", header.numVerts);
	LOGD("UV count: %d.", header.numTexCoord);
	LOGD("Triangle count: %d.", header.numTriangles);
	LOGD("GL command count: %d.", header.numGLCommands);
	LOGD("Skin offset: %d.", header.offsetSkins);
	LOGD("UV offset: %d.", header.offsetTexCoord);
	LOGD("Triangle offset: %d.", header.offsetTriangles);
	LOGD("Frame offset: %d.", header.offsetFrames);
	LOGD("GL command offset: %d.", header.offsetGLCommands);
	LOGD("End offset: %d.", header.offsetEnd);
	obj.header_ = header;
	obj.skins_ = NEW MD2Skin[header.numSkins];
	memcpy(obj.skins_,
		data + header.offsetSkins,
		header.numSkins * sizeof(MD2Skin));
	for (int i = 0; i < header.numSkins; i++) {
		LOGD("Skin %d: %s", i, obj.skins_[i].name);
	}
	obj.uv_ = NEW MD2UV[header.numTexCoord];
	memcpy(obj.uv_,
		data + header.offsetTexCoord,
		header.numTexCoord * sizeof(MD2UV));
	obj.triangles_ = NEW MD2Triangle[header.numTriangles];
	memcpy(obj.triangles_,
		data + header.offsetTriangles,
		header.numTriangles * sizeof(MD2Triangle));
	obj.commands_ = NEW int[header.numGLCommands];
	memcpy(obj.commands_,
		data + header.offsetGLCommands,
		header.numGLCommands * sizeof(int));
	obj.frames_ = NEW MD2Frame[header.numFrames];
	INT8* offset = data + header.offsetFrames;
	for (int i = 0; i < header.numFrames; i++) {
		obj.frames_[i].vertices = NEW MD2Vertex[header.numVerts];
		memcpy(obj.frames_[i].scale,
			offset,
			3 * sizeof(float));
		offset += 3 * sizeof(float);
		memcpy(obj.frames_[i].translate,
			offset,
			3 * sizeof(float));
		offset += 3 * sizeof(float);
		memcpy(obj.frames_[i].name,
			offset,
			16 * sizeof(char));
		LOGD("Frame name: %s.", obj.frames_[i].name);
		offset += 16 * sizeof(char);
		memcpy(obj.frames_[i].vertices,
			offset,
			header.numVerts * sizeof(MD2Vertex));
		offset += header.numVerts * sizeof(MD2Vertex);
	}
	delete[] data;
	return true;
}
