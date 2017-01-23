#ifndef OBJPARSER_H_
#define OBJPARSER_H_

#include "../../../ghost/ThreadManager.h"
#include "../../../ghost/Color.h"
#include "../../../ghost/ModelData.h"
#include "Face.h"
#include "../../../ghost/ServiceProvider.h"
#include "RawObject.h"

class ServiceLocator;

class ObjParser : public ServiceProvider
{
public:
	explicit ObjParser(ServiceLocator* serviceLocator);
	
	ModelData parse(string) const;

private:
	RawObject loadRawObject(ModelData& model, const string& file) const;
	Face parseFace(string line) const;
	vector<ModelData::Material> parseMaterial(string file) const;
	vector<ModelData::Material> parseMaterials(RawObject rawObject) const;
	void setVerticesAndIndices(ModelData& modelData, RawObject& rawObject) const;
	static void setupLastMaterialRange(RawObject& rawObject, SIZE materialFaceCount);
	static void* getAllocatedVertexBuffer(RawObject& rawObject, SIZE vertexCount);
	static ModelData::VertexType getVertexType(RawObject& rawObject);
	static vector<ModelData::Part> divideIntoPartsByMaterial(vector<ModelData::Material>& materials, RawObject& rawObject);
	BoundingVolume* getBoundingVolume(RawObject& rawObject) const;
};

#endif
