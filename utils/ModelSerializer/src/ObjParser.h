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
#include "RawObject.h"

class ServiceLocator;

class ObjParser : public ServiceProvider
{
public:
	explicit ObjParser(ServiceLocator* serviceLocator);

	bool parse(ModelData& model, const string& file);

private:
	/*
	* Scans through model content and
	* counts the number of vertices and faces.
	*/
	ObjProperties getObjProperties(string obj) const;
	static VertexProperties getVertexProperties(ObjProperties objProperties);
	RawObject loadRawObject(ModelData& model, const string& file);

	/**
	 * Rearrange faces and material indices, to have as little
	 * material changes as possible during render stage.
	 */
	static void rearrangeFacesAndMaterials(vector<MaterialIndex>& matIndices,
	                                vector<ModelData::Material>& materials, vector<Face>& faces);

	void do_work(ModelData& modelData, RawObject& rawObject,
	             vector<float>*& vs, UINT32& floatsInVertex) const;

	static void* getAllocatedVertexBuffer(RawObject& rawObject, SIZE vertexCount);


	vector<ModelData::Part> divideIntoPartsByMaterial(vector<ModelData::Material>& materials, RawObject& rawObject) const;
	BoundingVolume* getBoundingVolume(RawObject& rawObject) const;
};

#endif
