#include "ObjParser.h"
#include "UniqueKey.h"
#include "../../../ghost/Multiplatform/Ghost.h"
#include "../../../ghost/Multiplatform/ServiceLocator.h"
#include "../../../ghost/Multiplatform/FileManager.h"
#include "../../../ghost/BoundingSphere.h"
#include "../../../ghost/BoundingBox.h"
#include "../../../ghost/Utils.h"
#include "../../../ghost/Resources/Renderable.h"
#include "PositionParserTask.h"
#include "NormalParserTask.h"
#include "FaceParserTask.h"
#include "UVParserTask.h"
#include "../../../ghost/ServiceProvider.h"
#include "MaterialParserTask.h"
#include "RawObject.h"
#include "../../../ghost/Resources/Vertex.h"

ObjParser::ObjParser(ServiceLocator* serviceLocator): ServiceProvider(serviceLocator)
{
}

bool ObjParser::parse(ModelData& model, const string& file)
{
	RawObject rawObject = loadRawObject(model, file);

	PROFILE("Finished reading basic data for object: %s.", file.c_str());

	//rearrangeFacesAndMaterials(matIndices, materials, faces);

	vector<float>* vs;
	UINT32 floatsInVertex;
	do_work(model, rawObject, vs, floatsInVertex);

	
	SIZE vertexCount = vs->size() / floatsInVertex;
	void* vertexArray = getAllocatedVertexBuffer(rawObject, vertexCount);
	memcpy(vertexArray, &(*vs)[0], vs->size() * sizeof(float));
	model.setVertices(rawObject.vertexProperties.vertexType, reinterpret_cast<UINT8*>(vertexArray), vertexCount);
	LOGD("Vertex count: %u.", (UINT32) vertexCount);
	delete vs;
	model.setParts(divideIntoPartsByMaterial(model.getMaterials(), rawObject));
	model.setBoundingVolume(getBoundingVolume(rawObject));
	PROFILE("Finished parsing object: %s.", file.c_str());
	return true;
}

RawObject ObjParser::loadRawObject(ModelData& model, const string& file)
{
	string obj = getFileManager()->loadText(file);
	THROWONASSERT(obj.length() != 0, "Unable to load file: %s", file.c_str());

	ObjProperties obProp = getObjProperties(obj);
	LOGI("Size adjust ratio: %f.", 1.0f / obProp.maxVertexPos);

	VertexProperties vxProp = getVertexProperties(obProp);

	RawObject rawObject = RawObject(obProp, vxProp);
	auto data = obj.c_str();

	UINT64 posThread = getThreadManager()->execute(NEW PositionParserTask(data, rawObject));
	UINT64 normalThread = vxProp.hasNormals() ? getThreadManager()->execute(NEW NormalParserTask(data, rawObject)) : 0;
	UINT64 uvThread = vxProp.hasUV() ? getThreadManager()->execute(NEW UVParserTask(data, rawObject)) : 0;
	UINT64 faceThread = getThreadManager()->execute(NEW FaceParserTask(data, rawObject));
	UINT64 materialThread = getThreadManager()->execute(NEW MaterialParserTask(
		getServiceLocator(), obj, rawObject, model
	));

	getThreadManager()->join(posThread);
	if (normalThread != 0)
	{
		getThreadManager()->join(normalThread);
	}
	if (uvThread != 0)
	{
		getThreadManager()->join(uvThread);
	}
	getThreadManager()->join(faceThread);
	getThreadManager()->join(materialThread);

	return rawObject;
}

void* ObjParser::getAllocatedVertexBuffer(RawObject& rawObject, SIZE vertexCount)
{
	bool hasUV = rawObject.vertexProperties.hasUV();
	bool hasNormals = rawObject.vertexProperties.hasNormals();
	if (hasUV && hasNormals)
	{
		return NEW VertexPNT[vertexCount];
	}
	if (hasUV && !hasNormals)
	{
		return NEW VertexPT[vertexCount];
	}
	if (!hasUV && hasNormals)
	{
		return NEW VertexPN[vertexCount];
	}

	return NEW VertexP[vertexCount];
}

void ObjParser::rearrangeFacesAndMaterials(vector<MaterialIndex>& matIndices,
                                           vector<ModelData::Material>& materials, vector<Face>& faces)
{
	if (matIndices.size() <= materials.size())
	{
		return;
	}

	vector<vector<Face>> arrFaces;
	for (UINT32 i = 0; i < materials.size(); i++)
	{
		vector<Face> fac;
		arrFaces.push_back(fac);
	}
	SIZE size = matIndices.size();
	for (UINT32 i = 0; i < size; i++)
	{
		for (UINT32 j = 0; j < materials.size(); j++)
		{
			MaterialIndex& mi = matIndices[i];
			if (strcmp(mi.name.c_str(), materials[j].name) == 0)
			{
				for (SIZE k = mi.offset; k < mi.offset + mi.size; k++)
				{
					arrFaces[j].push_back(faces[k]);
				}
			}
		}
	}
	faces.clear();
	UINT32 offset = 0;
	matIndices.clear();
	for (UINT32 i = 0; i < arrFaces.size(); i++)
	{
		MaterialIndex mi;
		mi.name = string(materials[i].name);
		mi.offset = offset;
		mi.size = arrFaces[i].size();
		matIndices.push_back(mi);
		for (UINT32 j = 0; j < arrFaces[i].size(); j++)
		{
			faces.push_back(arrFaces[i][j]);
		}
		offset += mi.size;
	}
}

VertexProperties ObjParser::getVertexProperties(ObjProperties objProperties)
{
	VertexProperties vertexProperties;
	bool hasUV = objProperties.uvCount > 0;
	bool hasNormals = objProperties.normalCount > 0;
	if (hasUV && hasNormals)
	{
		vertexProperties.vertexSize = sizeof(VertexPNT);
		vertexProperties.positionOffset = offsetof(VertexPNT, pos);
		vertexProperties.normalOffset = offsetof(VertexPNT, normals);
		vertexProperties.uvOffset = offsetof(VertexPNT, uv);
		vertexProperties.vertexType = ModelData::PNT;
	}
	else if (hasUV && !hasNormals)
	{
		vertexProperties.vertexSize = sizeof(VertexPT);
		vertexProperties.positionOffset = offsetof(VertexPT, pos);
		vertexProperties.uvOffset = offsetof(VertexPT, uv);
		vertexProperties.vertexType = ModelData::PT;
	}
	else if (!hasUV && hasNormals)
	{
		vertexProperties.vertexSize = sizeof(VertexPN);
		vertexProperties.positionOffset = offsetof(VertexPN, pos);
		vertexProperties.normalOffset = offsetof(VertexPN, normals);
		vertexProperties.vertexType = ModelData::PN;
	}
	else
	{
		vertexProperties.vertexSize = sizeof(VertexP);
		vertexProperties.positionOffset = offsetof(VertexP, pos);
		vertexProperties.vertexType = ModelData::P;
	}
	return vertexProperties;
}

ObjProperties ObjParser::getObjProperties(string obj) const
{
	ObjProperties properties;
	size_t pos = 0;
	const char* data = obj.c_str();
	float vertexPos;
	while (true)
	{
		switch (obj[pos])
		{
		case 'v':
			switch (obj[pos + 1])
			{
			case ' ':
				properties.vertexCount += 3;
				{
					const char* found = strchr(data + pos + 1, GHOST_NEWLINE);
					if (found)
					{
						SIZE lineEnd = found - data;
						bool one = true;
						SIZE offset = 0;
						const char* start = data + pos + 2;
						for (SIZE i = 2; i < lineEnd; i++)
						{
							if (start[i] == ' ')
							{
								if (one)
								{
									vertexPos = fabs(toFloat(start));
									if (vertexPos > properties.maxVertexPos)
									{
										properties.maxVertexPos = vertexPos;
									}
									one = false;
									offset = i;
								}
								else
								{
									vertexPos = fabs(toFloat(start + offset + 1));
									if (vertexPos > properties.maxVertexPos)
									{
										properties.maxVertexPos = vertexPos;
									}
									offset = i;
									vertexPos = fabs(toFloat(start + offset + 1));
									if (vertexPos > properties.maxVertexPos)
									{
										properties.maxVertexPos = vertexPos;
									}
									break;
								}
							}
						}
					}
				}
				break;
			case 'n':
				properties.normalCount += 3;
				break;
			case 't':
				properties.uvCount += 2;
				break;
			}
			break;
		case 'f': // Faces.
			properties.faceCount++;
			break;
		case 'o': // Object groups.
			break;
		case '#': // Comments.
			break;
		}
		pos = obj.find(GHOST_NEWLINE, pos);
		if (pos == string::npos)
		{
			break;
		}
		pos++;
	}
	return properties;
}

void ObjParser::do_work(ModelData& modelData, RawObject& rawObject, vector<float>*& vs, UINT32& floatsInVertex) const
{
	auto lambda = [](int a, MaterialIndex b) { return a + b.size; };
	UINT32 indexCount = 3 * accumulate(rawObject.matIndices.begin(), rawObject.matIndices.end(), 0, lambda);
	bool useShort = indexCount < USHRT_MAX;
	SIZE indexSize = useShort ? sizeof(UINT16) : sizeof(UINT32);
	SIZE indexArraySize = indexCount * indexSize;

	UINT8* indexArray = new UINT8[indexArraySize];
	UINT32 indexOffset = 0;

	vs = NEW vector<float>();
	PROFILE("Started optimizations.");
	unordered_map<UniqueKey, int, UniqueKeyHash>* hm = NEW unordered_map<UniqueKey, int, UniqueKeyHash>();
	unordered_map<UniqueKey, int, UniqueKeyHash>::const_iterator it;
	floatsInVertex = rawObject.vertexProperties.vertexSize / sizeof(float);
	float* vertex = NEW float[floatsInVertex];
	for (SIZE i = 0; i < rawObject.objProperties.faceCount; i++)
	{
		Face& face = rawObject.faces[i];
		for (auto j = 0; j < Face::FACE_SIZE; j++)
		{
			UINT32 index = face.indices_[j];
			UniqueKey key;
			key.buffer[0] = index;
			key.buffer[1] = face.normIndices_[j];
			key.buffer[2] = face.texIndices_[j];
			it = hm->find(key);
			if (it != hm->end())
			{
				if (useShort) {
					short shortIndex = it->second;
					memcpy(indexArray + indexOffset, &shortIndex, indexSize);
				}
				else
				{
					memcpy(indexArray + indexOffset, &index, indexSize);
				}
				indexOffset += indexSize;
			}
			else
			{
				UINT32 vInd = 0;
				UINT32 offset = index * rawObject.vertexProperties.vertexSize + rawObject.vertexProperties.positionOffset;
				memcpy(&vertex[vInd++], &rawObject.vertices[offset + 0], sizeof(float));
				memcpy(&vertex[vInd++], &rawObject.vertices[offset + sizeof(float)], sizeof(float));
				memcpy(&vertex[vInd++], &rawObject.vertices[offset + 2 * sizeof(float)], sizeof(float));
				if (rawObject.objProperties.normalCount > 0)
				{
					index = face.normIndices_[j];
					offset = index * rawObject.vertexProperties.vertexSize + rawObject.vertexProperties.normalOffset;
					memcpy(&vertex[vInd++], &rawObject.vertices[offset + 0], sizeof(float));
					memcpy(&vertex[vInd++], &rawObject.vertices[offset + sizeof(float)], sizeof(float));
					memcpy(&vertex[vInd++], &rawObject.vertices[offset + 2 * sizeof(float)], sizeof(float));
				}
				if (rawObject.objProperties.uvCount > 0)
				{
					index = face.texIndices_[j];
					offset = index * rawObject.vertexProperties.vertexSize + rawObject.vertexProperties.uvOffset;
					memcpy(&vertex[vInd++], &rawObject.vertices[offset + 0], sizeof(float));
					memcpy(&vertex[vInd++], &rawObject.vertices[offset + sizeof(float)], sizeof(float));
				}

				if (useShort) {
					short shortIndex = static_cast<short>(vs->size() / floatsInVertex);
					memcpy(indexArray + indexOffset, &shortIndex, indexSize);
				}
				else
				{
					int intIndex = vs->size() / floatsInVertex;
					memcpy(indexArray + indexOffset, &intIndex, indexSize);
					
				}
				indexOffset += indexSize;

				hm->insert(pair<UniqueKey, int>(key, vs->size() / floatsInVertex));
				for (SIZE i = 0; i < floatsInVertex; i++)
				{
					vs->push_back(vertex[i]);
				}
			}
		}
	}
	PROFILE("Finished optimizing. Hash map size: %u.", (UINT32)hm->size());
	delete hm;
	PROFILE("Hash map deleted.");
	delete[] vertex;

	modelData.setIndices(indexArray, indexCount,
		useShort ? Renderable::INDEX_TYPE_USHORT : Renderable::INDEX_TYPE_UINT);
	LOGD("Index count: %u", indexArraySize);
	if (!useShort)
	{
		LOGI("3D model is too large for some mobile devices.");
	}
}

vector<ModelData::Part> ObjParser::divideIntoPartsByMaterial(
	vector<ModelData::Material>& materials, RawObject& rawObject) const
{
	// Create separate model part array.
	vector<ModelData::Part> parts(rawObject.matIndices.size());
	UINT32 offset = 0;
	for (UINT32 i = 0; i < rawObject.matIndices.size(); i++)
	{
		MaterialIndex mi = rawObject.matIndices[i];
		if (mi.name.length() > 0)
		{
			for (UINT32 j = 0; j < materials.size(); j++)
			{
				if (strcmp(mi.name.c_str(), materials[j].name) == 0)
				{
					parts[i].material_ = j;
				}
			}
		}
		parts[i].offset_ = offset;
		parts[i].indexCount_ = static_cast<UINT32>(mi.size) * 3;
		offset += parts[i].indexCount_;
	}
	return parts;
}


BoundingVolume* ObjParser::getBoundingVolume(RawObject& rawObject) const
{
	float width = rawObject.maxVertex.getX() - rawObject.minVertex.getX();
	float height = rawObject.maxVertex.getY() - rawObject.minVertex.getY();
	float depth = rawObject.maxVertex.getZ() - rawObject.minVertex.getZ();
	float r = rawObject.radius;
	float volumeBox = width * height * depth;
	float volumeSphere = 4.0f / 3.0f * static_cast<float>(SMART_PI) * r * r * r;
	LOGD("Sphere radius: %f. Box width: %f, height: %f, depth: %f.", r, width, height, depth);
	LOGD("Calculating optimal bounding volume for mesh. "
		"Box volume: %f, Sphere volume: %f. Optimal is %s.",
		volumeBox, volumeSphere, volumeSphere < volumeBox ? "sphere" : "box");
	return volumeSphere < volumeBox
		                         ? static_cast<BoundingVolume*>(NEW BoundingSphere(rawObject.radius))
		                         : static_cast<BoundingVolume*>(NEW BoundingBox(Vec3(width, height, depth)));
}