#ifndef OBJPARSER_H_
#define OBJPARSER_H_

#include "../../../ghost/ThreadManager.h"
#include "../../../ghost/Color.h"
#include "../../../ghost/ModelData.h"
#include "MaterialIndex.h"
#include "Face.h"
#include "../../../ghost/ServiceProvider.h"

class ServiceLocator;

class ObjParser : public ServiceProvider {
public:
	explicit ObjParser(ServiceLocator* serviceLocator);

	bool parse(ModelData& model, const string& file);
	
private:
	void countComponents(const string& obj, UINT32& vertices,
		UINT32& normals, UINT32& uvCoordinates, UINT32& faces, float& maxVertexPos);
	bool parseMaterial(ModelData& model, const string& file);
	void* getAllocatedVertexBuffer(bool hasUV, bool hasNormals, SIZE vertexCount);

	/**
	 * Rearrange faces and material indices, to have as little
	 * material changes as possible during render stage.
	 */
	void rearrangeFacesAndMaterials(vector<MaterialIndex> &matIndices,
		vector<ModelData::Material> &materials, vector<Face> &faces);
};

#endif
