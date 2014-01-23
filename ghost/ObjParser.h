/*
 * ObjParser.h
 *
 *  Created on: 2012.06.25
 *      Author: MS
 */

#ifndef OBJPARSER_H_
#define OBJPARSER_H_

#include "ThreadManager.h"
#include "Color.h"
#include "Vec3.h"
#include "Resources/Vertex.h"
#include "BoundingVolume.h"
#include "ModelData.h"

class ServiceLocator;

class ObjParser {
public:
	struct MaterialIndex {
		MaterialIndex() :
			name_(""),
			offset_(0),
			size_(0)
		{}
		string name_;
		SIZE offset_;
		SIZE size_;
	};

	/**
	 * Contains one face data.
	 */
	struct Face {
		/** Vertex indices. */
		UINT32 indices_[3];
		/** Texture indices. */
		UINT32 texIndices_[3];
		/** Normal indices. */
		UINT32 normIndices_[3];
		/** Face size. */
		static const int FACE_SIZE = 3;
	};
public:
	static bool parse(ModelData& model, const string& file, ServiceLocator* services);
	static bool parseFace(Face& face, const char* line, SIZE length);
private:
	static void countComponents(const string& obj, UINT32& vertices,
		UINT32& normals, UINT32& uvCoordinates, UINT32& faces, float& maxVertexPos);
	static bool parseMaterial(
		ModelData& model, const string& file, ServiceLocator* services);

};

#endif
