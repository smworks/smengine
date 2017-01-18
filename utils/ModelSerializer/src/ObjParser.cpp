#include "ObjParser.h"
#include "UniqueKey.h"
#include "../../../ghost/Multiplatform/Ghost.h"
#include "../../../ghost/Multiplatform/ServiceLocator.h"
#include "../../../ghost/Multiplatform/FileManager.h"
#include "../../../ghost/BoundingSphere.h"
#include "../../../ghost/BoundingBox.h"
#include "../../../ghost/Utils.h"
#include "../../../ghost/Resources/Texture.h"
#include "../../../ghost/Resources/Renderable.h"
#include "PositionParserTask.h"
#include "NormalParserTask.h"
#include "FaceParserTask.h"
#include "UVParserTask.h"
#include "../../../ghost/ServiceProvider.h"

ObjParser::ObjParser(ServiceLocator* serviceLocator): ServiceProvider(serviceLocator)
{
}

bool ObjParser::parse(ModelData& model, const string& file)
{
	string obj = getFileManager()->loadText(file);
	THROWONASSERT(obj.length() == 0, "Unable to load file: %s", file.c_str());
	
	
	ObjProperties objProperties = getObjProperties(obj);
	LOGD("Size adjust ratio: %f.", 1.0f / objProperties.maxVertexPos);
	auto hasUV = objProperties.uvCount > 0;
	auto hasNormals = objProperties.normalCount > 0;
	if (objProperties.faceCount * 3 > UINT_MAX)
	{
		LOGW("3D model cannot have more indices than: %d.", UINT_MAX);
		return false;
	}
	UINT8* vertices;
	ModelData::VertexType vertexType;
	UINT32 posOffset = 0, normOffset = 0, uvOffset = 0, vertexSize;
	if (hasUV && hasNormals)
	{
		vertices = reinterpret_cast<UINT8*>(NEW VertexPNT[objProperties.vertexCount]);
		vertexSize = sizeof(VertexPNT);
		posOffset = offsetof(VertexPNT, pos);
		normOffset = offsetof(VertexPNT, normals);
		uvOffset = offsetof(VertexPNT, uv);
		vertexType = ModelData::PNT;
	}
	else if (hasUV && !hasNormals)
	{
		vertices = reinterpret_cast<UINT8*>(NEW VertexPT[objProperties.vertexCount]);
		vertexSize = sizeof(VertexPT);
		posOffset = offsetof(VertexPT, pos);
		uvOffset = offsetof(VertexPT, uv);
		vertexType = ModelData::PT;
	}
	else if (!hasUV && hasNormals)
	{
		vertices = reinterpret_cast<UINT8*>(NEW VertexPN[objProperties.vertexCount]);
		vertexSize = sizeof(VertexPN);
		posOffset = offsetof(VertexPN, pos);
		normOffset = offsetof(VertexPN, normals);
		vertexType = ModelData::PN;
	}
	else
	{
		vertices = reinterpret_cast<UINT8*>(NEW VertexP[objProperties.vertexCount]);
		vertexSize = sizeof(VertexP);
		posOffset = offsetof(VertexP, pos);
		vertexType = ModelData::P;
	}
	vector<Face> faces;
	faces.resize(objProperties.faceCount);
	string line;
	SIZE pos = 0;
	SIZE lineEnd = 0;
	SIZE params = 0;
	MaterialIndex matInd;
	SIZE matOff = 0;
	SIZE matSize = 0;
	vector<string> arr;
	const char* data = obj.c_str();
	UINT64 posThread = 0, normalThread = 0, uvThread = 0, faceThread = 0;
	Vec3 vmin(FLT_MAX), vmax(FLT_MIN), radius(0.0f);
	posThread = getThreadManager()->execute(NEW PositionParserTask(
		data, vertices, posOffset, vertexSize, &vmin, &vmax, &radius, objProperties.maxVertexPos));
	if (hasNormals)
	{
		normalThread = getThreadManager()->execute(NEW NormalParserTask(data, vertices, normOffset, vertexSize));
	}
	if (hasUV)
	{
		uvThread = getThreadManager()->execute(NEW UVParserTask(data, vertices, uvOffset, vertexSize));
	}
	faceThread = getThreadManager()->execute(NEW FaceParserTask(data, &faces));

	vector<MaterialIndex> matIndices;
	auto& materials = model.getMaterials();
	while (true)
	{
		lineEnd = obj.find(GHOST_NEWLINE, pos);
		if (lineEnd == string::npos)
		{
			break;
		}
		switch (obj[pos])
		{
		case 'f': // Faces.
			line = obj.substr(pos, lineEnd - pos);
			stringSplit(line, arr, ' ');
			params = arr.size();
			if (params == 4)
			{
				matSize++;
			}
			break;
		case 'm': // Material file.
			line = obj.substr(pos, lineEnd - pos);
			stringSplit(line, arr, ' ');
			if (!parseMaterial(model, arr[1]))
			{
				LOGW("Unable to parse material file: %s", arr[1].c_str());
				return false;
			}
			break;
		case 'u': // Use material.
			line = obj.substr(pos, lineEnd - pos);
			stringSplit(line, arr, ' ');
			if (materials.size() == 0)
			{
				LOGW("Material file must be defined first");
			}
			else if (arr.size() < 2)
			{
				LOGE("usemtl in .obj file has no name.");
			}
			else
			{
				SIZE size = materials.size();
				bool found = false;
				for (SIZE i = 0; i < size; i++)
				{
					if (strcmp(materials[i].name, arr[1].c_str()) == 0)
					{
						found = true;
					}
				}
				if (found)
				{
					if (matSize == 0)
					{ // First element.
						matInd.name_ = arr[1];
						matInd.offset_ = matOff;
					}
					else
					{
						matInd.size_ = matSize;
						matOff += matSize;
						matIndices.push_back(matInd);
						matInd.name_ = arr[1];
						matInd.offset_ = matOff;
						matSize = 0;
					}
				}
				else
				{
					LOGW("Material %s not found.", arr[1].c_str());
					if (matSize == 0)
					{ // First element.
						matInd.name_ = "default";
						matInd.offset_ = matOff;
					}
					else
					{
						matInd.size_ = matSize;
						matOff += matSize;
						matIndices.push_back(matInd);
						matInd.name_ = "default";
						matInd.offset_ = matOff;
						matSize = 0;
					}
				}
			}
			break;
		case 'o': // Object groups.
			break;
		case '#': // Comments.
			break;
		}
		pos = lineEnd + 1;
	}


	getThreadManager()->join(posThread);
	if (hasNormals)
	{
		getThreadManager()->join(normalThread);
	}
	if (hasUV)
	{
		getThreadManager()->join(uvThread);
	}
	getThreadManager()->join(faceThread);
	PROFILE("Finished reading basic data for object: %s.", file.c_str());
	matInd.size_ = matSize;
	matIndices.push_back(matInd);
	//rearrangeFacesAndMaterials(matIndices, materials, faces);
	vector<UINT16>* indexArray = 0;
	vector<UINT32>* indexArrayInt = 0;
	UINT32 indexReserve = 0;
	for (UINT32 i = 0; i < matIndices.size(); i++)
	{
		indexReserve += matIndices[i].size_ * 3;
	}
	bool useShort = true;
	if (indexReserve >= USHRT_MAX)
	{
		useShort = false;
		indexArrayInt = NEW vector<UINT32>();
		indexArrayInt->reserve(indexReserve);
	}
	else
	{
		indexArray = NEW vector<UINT16>();
		indexArray->reserve(indexReserve);
	}
	vector<float>* vs = NEW vector<float>();
	PROFILE("Started optimizations.");
	unordered_map<UniqueKey, int, UniqueKeyHash>* hm = NEW unordered_map<UniqueKey, int, UniqueKeyHash>();
	unordered_map<UniqueKey, int, UniqueKeyHash>::const_iterator it;
	UINT32 floatsInVertex = vertexSize / sizeof(float);
	float* vertex = NEW float[floatsInVertex];
	for (SIZE i = 0; i < objProperties.faceCount; i++)
	{
		Face& face = faces[i];
		for (int j = 0; j < Face::FACE_SIZE; j++)
		{
			UINT32 index = face.indices_[j];
			UniqueKey key;
			key.buffer[0] = index;
			key.buffer[1] = face.normIndices_[j];
			key.buffer[2] = face.texIndices_[j];
			it = hm->find(key);
			if (it != hm->end())
			{
				if (useShort)
				{
					indexArray->push_back(it->second);
				}
				else
				{
					indexArrayInt->push_back(it->second);
				}
			}
			else
			{
				UINT32 vInd = 0;
				UINT32 offset = index * vertexSize + posOffset;
				memcpy(&vertex[vInd++], &vertices[offset + 0], sizeof(float));
				memcpy(&vertex[vInd++], &vertices[offset + sizeof(float)], sizeof(float));
				memcpy(&vertex[vInd++], &vertices[offset + 2 * sizeof(float)], sizeof(float));
				if (objProperties.normalCount > 0)
				{
					index = face.normIndices_[j];
					offset = index * vertexSize + normOffset;
					memcpy(&vertex[vInd++], &vertices[offset + 0], sizeof(float));
					memcpy(&vertex[vInd++], &vertices[offset + sizeof(float)], sizeof(float));
					memcpy(&vertex[vInd++], &vertices[offset + 2 * sizeof(float)], sizeof(float));
				}
				if (objProperties.uvCount > 0)
				{
					index = face.texIndices_[j];
					offset = index * vertexSize + uvOffset;
					memcpy(&vertex[vInd++], &vertices[offset + 0], sizeof(float));
					memcpy(&vertex[vInd++], &vertices[offset + sizeof(float)], sizeof(float));
				}
				if (useShort)
				{
					indexArray->push_back((UINT16) (vs->size() / floatsInVertex));
				}
				else
				{
					indexArrayInt->push_back((UINT32) vs->size() / floatsInVertex);
				}
				hm->insert(pair<UniqueKey, int>(key, vs->size() / floatsInVertex));
				for (SIZE i = 0; i < floatsInVertex; i++)
				{
					vs->push_back(vertex[i]);
				}
			}
		}
	}
	PROFILE("Finished optimizing. Hash map size: %u.", (UINT32) hm->size());
	delete hm;
	PROFILE("Hash map deleted.");
	delete [] vertex;
	if (useShort)
	{
		UINT16* indexArr = NEW UINT16[indexArray->size()];
		memcpy(indexArr, &(*indexArray)[0], indexArray->size() * sizeof(UINT16));
		model.setIndices(Renderable::INDEX_TYPE_USHORT, indexArr, indexArray->size());
		LOGD("Indices: %u", (UINT32) indexArray->size());
	}
	else
	{
		UINT32* indexArr = NEW UINT32[indexArrayInt->size()];
		memcpy(indexArr, &(*indexArrayInt)[0], indexArrayInt->size() * sizeof(UINT32));
		model.setIndices(Renderable::INDEX_TYPE_UINT, indexArr, indexArrayInt->size());
		LOGI("3D model is too large for some mobile devices.");
		LOGD("Indices: %u", (UINT32) indexArrayInt->size());
	}
	delete indexArray;
	delete indexArrayInt;
	SIZE vertexCount = vs->size() / floatsInVertex;
	void* vertexArray = getAllocatedVertexBuffer(hasUV, hasNormals, vertexCount);
	memcpy(vertexArray, &(*vs)[0], vs->size() * sizeof(float));
	model.setVertices(vertexType, reinterpret_cast<UINT8*>(vertexArray), vertexCount);
	LOGD("Vertex count: %u.", (UINT32) vertexCount);
	delete vs;
	delete [] vertices;
	// Create separate model part array.
	vector<ModelData::Part>& parts = model.getParts();
	parts.resize(matIndices.size());
	UINT32 offset = 0;
	for (UINT32 i = 0; i < matIndices.size(); i++)
	{
		MaterialIndex mi = matIndices[i];
		if (mi.name_.length() > 0)
		{
			for (UINT32 j = 0; j < materials.size(); j++)
			{
				if (strcmp(mi.name_.c_str(), materials[j].name) == 0)
				{
					parts[i].material_ = j;
				}
			}
		}
		parts[i].offset_ = offset;
		parts[i].indexCount_ = (UINT32) mi.size_ * 3;
		offset += parts[i].indexCount_;
	}
	// Calculate bounding volume.
	float width = vmax.getX() - vmin.getX();
	float height = vmax.getY() - vmin.getY();
	float depth = vmax.getZ() - vmin.getZ();
	float r = radius.length();
	float volumeBox = width * height * depth;
	float volumeSphere = 4.0f / 3.0f * (float) SMART_PI * r * r * r;
	LOGD("Sphere radius: %f. Box width: %f, height: %f, depth: %f.", r, width, height, depth);
	LOGD("Calculating optimal bounding volume for mesh. "
		"Box volume: %f, Sphere volume: %f. Optimal is %s.",
		volumeBox, volumeSphere, volumeSphere < volumeBox ? "sphere" : "box");
	BoundingVolume* bounds = 0;
	if (volumeSphere < volumeBox)
	{
		bounds = NEW BoundingSphere(radius.length());
	}
	else
	{
		bounds = NEW BoundingBox(Vec3(width, height, depth));
	}
	model.setBoundingVolume(bounds);
	PROFILE("Finished parsing object: %s.", file.c_str());
	return true;
}

void* ObjParser::getAllocatedVertexBuffer(bool hasUV, bool hasNormals, SIZE vertexCount)
{
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
			if (strcmp(mi.name_.c_str(), materials[j].name) == 0)
			{
				for (SIZE k = mi.offset_; k < mi.offset_ + mi.size_; k++)
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
		mi.name_ = string(materials[i].name);
		mi.offset_ = offset;
		mi.size_ = arrFaces[i].size();
		matIndices.push_back(mi);
		for (UINT32 j = 0; j < arrFaces[i].size(); j++)
		{
			faces.push_back(arrFaces[i][j]);
		}
		offset += mi.size_;
	}
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

/*
 * Loads material data into entity model.
 */
bool ObjParser::parseMaterial(ModelData& model, string file) const
{
	auto obj = getFileManager()->loadText(file);
	if (obj.length() == 0)
	{
		LOGW("Material %s not found.", file.c_str());
	}

	string line;
	vector<string> arr;
	size_t lineEnd = 0, pos = 0;
	ModelData::Material mat;
	mat.setName("default");
	while (true)
	{
		lineEnd = obj.find(GHOST_NEWLINE, pos);
		if (lineEnd == string::npos)
		{
			break;
		}
		line = obj.substr(pos, lineEnd - pos);
		if (line.length() == 0)
		{
			pos = lineEnd + 1;
			continue;
		}
		stringSplit(line, arr, ' ');
		if (arr.size() < 2)
		{
			pos = lineEnd + 1;
			continue;
		}

		switch (obj[pos])
		{
		case 'K':
			switch (obj[pos + 1])
			{
			case 'a': // Ambient color.
				mat.ambient_.setRGB(
					toFloat(arr[1].c_str()),
					toFloat(arr[2].c_str()),
					toFloat(arr[3].c_str()));
				break;
			case 'd': // Diffuse color.
				mat.diffuse_.setRGB(
					toFloat(arr[1].c_str()),
					toFloat(arr[2].c_str()),
					toFloat(arr[3].c_str()));
				break;
			case 's': // Specular color.
				mat.specular_.setRGB(
					toFloat(arr[1].c_str()),
					toFloat(arr[2].c_str()),
					toFloat(arr[3].c_str()));
				break;
			}
			break;
		case 'N': // Specular color intensity.
			mat.specIntensity_ = toFloat(arr[1].c_str());
			break;
		case 'd': // Transparency.
			mat.transparency_ = toFloat(arr[1].c_str());
			break;
		case 'm': // Texture.
			{
				SIZE index = arr[1].find_last_of("/\\");
				string texture;
				if (index != string::npos)
				{
					texture = arr[1].substr(index + 1);
				}
				else
				{
					texture = arr[1];
				}
				if (texture.length() > 0)
				{
					mat.texture_ = Texture::load(getServiceLocator(), texture);
				}
			}
			break;
		case 'n':
			if (arr.size() < 2)
			{
				LOGE("Used materials has no name.");
				return false;
			}
			else
			{
				model.getMaterials().push_back(mat);
				mat = ModelData::Material();
				mat.setName(arr[1]);
			}
			break;
		}
		pos = lineEnd + 1;
	}

	model.getMaterials().push_back(mat);

	return true;
}
