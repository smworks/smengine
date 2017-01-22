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

	ModelData parse(string) const;

private:
	/*
	* Scans through model content and
	* counts the number of vertices and faces.
	*/
	ObjProperties getObjProperties(string obj) const;
	static VertexProperties getVertexProperties(ObjProperties objProperties);
	RawObject loadRawObject(ModelData& model, const string& file) const;

	void setVerticesAndIndices(ModelData& modelData, RawObject& rawObject) const;

	static void* getAllocatedVertexBuffer(RawObject& rawObject, SIZE vertexCount);


	static vector<ModelData::Part> divideIntoPartsByMaterial(vector<ModelData::Material>& materials, RawObject& rawObject);
	BoundingVolume* getBoundingVolume(RawObject& rawObject) const;
};

#endif
