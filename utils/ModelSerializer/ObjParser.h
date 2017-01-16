#ifndef OBJPARSER_H_
#define OBJPARSER_H_

#include "../../ghost/ThreadManager.h"
#include "../../ghost/Color.h"
#include "../../ghost/Vec3.h"
#include "../../ghost/Resources/Vertex.h"
#include "../../ghost/BoundingVolume.h"
#include "../../ghost/ModelData.h"

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
		UINT32 indices_[3];
		UINT32 texIndices_[3];
		UINT32 normIndices_[3];
		static const int FACE_SIZE = 3;
	};
public:
	bool parse(ModelData& model, const string& file, ServiceLocator* services);
	static bool parseFace(Face& face, const char* line, SIZE length);
private:
	void countComponents(const string& obj, UINT32& vertices,
		UINT32& normals, UINT32& uvCoordinates, UINT32& faces, float& maxVertexPos);
	bool parseMaterial(
		ModelData& model, const string& file, ServiceLocator* services);
	void* ObjParser::getAllocatedVertexBuffer(bool hasUV, bool hasNormals, SIZE vertexCount);

	/**
	 * Rearrange faces and material indices, to have as little
	 * material changes as possible during render stage.
	 */
	void rearrangeFacesAndMaterials(vector<ObjParser::MaterialIndex> &matIndices,
		vector<ModelData::Material> &materials, vector<ObjParser::Face> &faces);
};

#endif
