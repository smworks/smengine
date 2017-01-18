#ifndef OBJPARSER_H_
#define OBJPARSER_H_

#include "../../../ghost/ThreadManager.h"
#include "../../../ghost/Color.h"
#include "../../../ghost/ModelData.h"
#include "MaterialIndex.h"
#include "Face.h"
#include "../../../ghost/ServiceProvider.h"
#include "ObjProperties.h"
#include "VertexProperties.h"

class ServiceLocator;

class ObjParser : public ServiceProvider {
public:
	explicit ObjParser(ServiceLocator* serviceLocator);

	bool parse(ModelData& model, const string& file);
	
private:

	/*
	* Scans through model content and
	* counts the number of vertices and faces.
	*/
	ObjProperties getObjProperties(string obj) const;
	bool parseMaterial(ModelData& model, string file) const;
	void* getAllocatedVertexBuffer(bool hasUV, bool hasNormals, SIZE vertexCount);

	/**
	 * Rearrange faces and material indices, to have as little
	 * material changes as possible during render stage.
	 */
	void rearrangeFacesAndMaterials(vector<MaterialIndex> &matIndices,
		vector<ModelData::Material> &materials, vector<Face> &faces);

	VertexProperties getVertexProperties(ObjProperties objProperties);
};

#endif
