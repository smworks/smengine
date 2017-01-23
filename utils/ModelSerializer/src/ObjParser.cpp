#include "ObjParser.h"
#include "UniqueKey.h"
#include "../../../ghost/Multiplatform/Ghost.h"
#include "../../../ghost/Multiplatform/ServiceLocator.h"
#include "../../../ghost/Multiplatform/FileManager.h"
#include "../../../ghost/BoundingSphere.h"
#include "../../../ghost/BoundingBox.h"
#include "../../../ghost/Utils.h"
#include "../../../ghost/ServiceProvider.h"
#include "RawObject.h"
#include "../../../ghost/Resources/Vertex.h"
#include "../../../ghost/Resources/Texture.h"

ObjParser::ObjParser(ServiceLocator* serviceLocator): ServiceProvider(serviceLocator)
{
}

ModelData ObjParser::parse(string file) const
{
	ModelData model;
	PROFILE("Parsing %s file into temporary object", file.c_str());
	RawObject rawObject = loadRawObject(model, file);
	PROFILE("Parsing material files for %s", file.c_str());
	model.setMaterials(parseMaterials(rawObject));
	PROFILE("Creating vertices");
	setVerticesAndIndices(model, rawObject);
	PROFILE("Dividing model into separate parts for each material");
	model.setParts(divideIntoPartsByMaterial(model.getMaterials(), rawObject));
	PROFILE("Calculating bounding box");
	model.setBoundingVolume(getBoundingVolume(rawObject));
	PROFILE("Finished parsing object: %s.", file.c_str());
	return model;
}

RawObject ObjParser::loadRawObject(ModelData& model, const string& file) const
{
	string obj = getFileManager()->loadText(file);
	THROWONASSERT(obj.length() != 0, "Unable to load file: %s", file.c_str());

	RawObject rawObject;
	SIZE materialFaceCount = 0;
	SIZE pos = 0;
	while (true)
	{
		SIZE lineEnd = obj.find(GHOST_NEWLINE, pos);
		if (lineEnd == string::npos)
		{
			break;
		}
		SIZE firstEmptySpace = obj.find(' ', pos);
		string key = obj.substr(pos, firstEmptySpace - pos);
		string line = trim(obj.substr(firstEmptySpace + 1, lineEnd - firstEmptySpace - 1));

		if (key == "v")
		{
			auto arr = stringSplit(line, ' ');
			rawObject.positions.push_back(Vec3(stof(arr[0]), stof(arr[1]), stof(arr[2])));
		}
		else if (key == "vn")
		{
			auto arr = stringSplit(line, ' ');
			rawObject.normals.push_back(Vec3(stof(arr[0]), stof(arr[1]), stof(arr[2])));
		}
		else if (key == "vt")
		{
			auto arr = stringSplit(line, ' ');
			rawObject.uvCoordinates.push_back(Vec2(stof(arr[0]), stof(arr[1])));
		}
		else if (key == "f")
		{
			materialFaceCount++;
			rawObject.faces.push_back(parseFace(line));
		}
		else if (key == "mtllib")
		{
			rawObject.materialFiles.push_back(line);
		}
		else if (key == "usemtl")
		{
			setupLastMaterialRange(rawObject, materialFaceCount);
			materialFaceCount = 0;
			rawObject.materialRanges.push_back(MaterialRange(line, 0, 0));
		}

		setupLastMaterialRange(rawObject, materialFaceCount);

		pos = lineEnd + 1;
	}



	return rawObject;
}

Face ObjParser::parseFace(string line) const
{
	auto cline = line.c_str();
	SIZE length = line.length();
	Face face;
	const char* arr[3];
	arr[0] = cline;
	arr[1] = arr[2] = 0;
	SIZE sizeArray[3];
	sizeArray[0] = sizeArray[1] = sizeArray[2] = 0;
	for (SIZE i = 0; i < length; i++)
	{
		if (cline[i] == ' ')
		{
			if (arr[1] == 0)
			{
				arr[1] = cline + i + 1;
				sizeArray[0] = i;
			}
			else
			{
				sizeArray[1] = i - sizeArray[0] - 1;
				arr[2] = cline + i + 1;
				break;
			}
		}
	}
	THROWONASSERT(arr[2] != 0, "Incorrect face line");
	sizeArray[2] = length - (arr[2] - cline);
	for (auto i = 0; i < Face::FACE_SIZE; i++)
	{
		SIZE firstSlash = 0, secondSlash = 0;
		for (SIZE j = 0; j < sizeArray[i]; j++)
		{
			if (arr[i][j] == '/')
			{
				if (firstSlash == 0)
				{
					firstSlash = j;
				}
				else
				{
					secondSlash = j;
					break;
				}
			}
		}
		if (secondSlash != 0)
		{ // Has normals.
			face.normIndices[i] = toUint(
				arr[i] + secondSlash + 1, sizeArray[i] - secondSlash - 1) - 1;
			if (secondSlash - firstSlash > 1)
			{ // Has UV.
				face.texIndices[i] = toUint(
					arr[i] + firstSlash + 1, secondSlash - firstSlash - 1) - 1;
			}
		}
		else if (firstSlash != 0)
		{ // Has UV, but no normals.
			face.texIndices[i] = toUint(
				arr[i] + firstSlash + 1, sizeArray[i] - firstSlash - 1) - 1;
		}
		SIZE count = firstSlash == 0 ? sizeArray[i] : firstSlash;
		face.posIndices[i] = toUint(arr[i], count) - 1;
	}
	return face;
}

vector<ModelData::Material> ObjParser::parseMaterial(string file) const
{
	auto obj = getFileManager()->loadText(file);
	if (obj.length() == 0)
	{
		LOGW("Material %s not found.", file.c_str());
	}

	string line;
	vector<string> arr;
	size_t lineEnd = 0, pos = 0;
	vector<ModelData::Material> materials;
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
		arr = stringSplit(line, ' ');
		if (arr.size() < 2)
		{
			pos = lineEnd + 1;
			continue;
		}

		switch (obj[pos])
		{
		case 'K':
			ASSERT(materials.size() > 0, "Colors before material name in %s", file.c_str());
			switch (obj[pos + 1])
			{
			case 'a': // Ambient color.
				materials.back().ambient.setRGB(
					toFloat(arr[1].c_str()),
					toFloat(arr[2].c_str()),
					toFloat(arr[3].c_str()));
				break;
			case 'd': // Diffuse color.
				materials.back().diffuse.setRGB(
					toFloat(arr[1].c_str()),
					toFloat(arr[2].c_str()),
					toFloat(arr[3].c_str()));
				break;
			case 's': // Specular color.
				materials.back().specular.setRGB(
					toFloat(arr[1].c_str()),
					toFloat(arr[2].c_str()),
					toFloat(arr[3].c_str()));
				break;
			}
			break;
		case 'N': // Specular color intensity.
			ASSERT(materials.size() > 0, "Specular intensity before material name in %s", file.c_str());
			materials.back().specIntensity = toFloat(arr[1].c_str());
			break;
		case 'd': // Transparency.
			ASSERT(materials.size() > 0, "Transparency before material name in %s", file.c_str());
			materials.back().transparency = toFloat(arr[1].c_str());
			break;
		case 'm': // Texture.
			{
				ASSERT(materials.size() > 0, "Texture before material name in %s", file.c_str());
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
					materials.back().texture = Texture::load(getServiceLocator(), texture);
				}
			}
			break;
		case 'n':
			THROWONASSERT((arr.size() >= 2), "Used material has no name");
			materials.push_back(ModelData::Material(arr[1]));
			break;
		}
		pos = lineEnd + 1;
	}

	return materials;
}

void ObjParser::setupLastMaterialRange(RawObject& rawObject, SIZE materialFaceCount)
{
	if (rawObject.materialRanges.size() > 0)
	{
		auto& lastMaterialRange = rawObject.materialRanges.back();
		lastMaterialRange.faceCount = materialFaceCount;

		if (rawObject.materialRanges.size() > 1)
		{
			auto sfbmr = rawObject.materialRanges[rawObject.materialRanges.size() - 2];
			lastMaterialRange.faceOffset = sfbmr.faceOffset + sfbmr.faceCount;
		}
	}
}

void* ObjParser::getAllocatedVertexBuffer(RawObject& rawObject, SIZE vertexCount)
{
	bool hasUV = rawObject.uvCoordinates.size() > 0;
	bool hasNormals = rawObject.normals.size() > 0;
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

ModelData::VertexType ObjParser::getVertexType(RawObject& rawObject)
{
	bool hasUV = rawObject.uvCoordinates.size() > 0;
	bool hasNormals = rawObject.normals.size() > 0;
	if (hasUV && hasNormals)
	{
		return ModelData::VertexType::PNT;
	}
	if (hasUV && !hasNormals)
	{
		return ModelData::VertexType::PT;
	}
	if (!hasUV && hasNormals)
	{
		return ModelData::VertexType::PN;
	}

	return ModelData::VertexType::P;
}

vector<ModelData::Material> ObjParser::parseMaterials(RawObject rawObject) const
{
	vector<ModelData::Material> materials;
	for (string materialFile : rawObject.materialFiles)
	{
		for (auto material : parseMaterial(materialFile))
		{
			materials.push_back(material);
		}
	}
	return materials;
}

void ObjParser::setVerticesAndIndices(ModelData& modelData, RawObject& rawObject) const
{
	vector<float>* vs = NEW vector<float>();
	SIZE vertexCount = 0;
	for (auto& face : rawObject.faces)
	{
		for (auto i = 0; i < Face::FACE_SIZE; i++)
		{
			Vec3 pos = rawObject.positions[face.posIndices[i]];
			vs->push_back(pos.getX());
			vs->push_back(pos.getY());
			vs->push_back(pos.getZ());

			if (rawObject.normals.size() > 0)
			{
				Vec3 normal = rawObject.normals[face.normIndices[i]];
				vs->push_back(normal.getX());
				vs->push_back(normal.getY());
				vs->push_back(normal.getZ());
			}
			if (rawObject.uvCoordinates.size() > 0)
			{
				Vec2 uv = rawObject.uvCoordinates[face.texIndices[i]];
				vs->push_back(uv.vec[0]);
				vs->push_back(uv.vec[1]);
			}
			vertexCount++;
		}
	}

	void* vertexArray = getAllocatedVertexBuffer(rawObject, vertexCount);
	memcpy(vertexArray, &(*vs)[0], vs->size() * sizeof(float));
	modelData.setVertices(getVertexType(rawObject), reinterpret_cast<UINT8*>(vertexArray), vertexCount);
	LOGD("Vertex count: %u.", static_cast<UINT32>(vertexCount));
	delete vs;
}

vector<ModelData::Part> ObjParser::divideIntoPartsByMaterial(
	vector<ModelData::Material>& materials, RawObject& rawObject)
{
	// Create separate model part array.
	vector<ModelData::Part> parts(rawObject.materialRanges.size());
	UINT32 offset = 0;
	for (UINT32 i = 0; i < rawObject.materialRanges.size(); i++)
	{
		MaterialRange mr = rawObject.materialRanges[i];
		if (mr.materialName.length() > 0)
		{
			for (UINT32 j = 0; j < materials.size(); j++)
			{
				if (strcmp(mr.materialName.c_str(), materials[j].name) == 0)
				{
					parts[i].material_ = j;
				}
			}
		}
		parts[i].offset_ = offset;
		parts[i].indexCount_ = static_cast<UINT32>(mr.faceCount) * 3;
		offset += parts[i].indexCount_;
	}
	return parts;
}


BoundingVolume* ObjParser::getBoundingVolume(RawObject& rawObject) const
{
	Vec3 minVertex;
	Vec3 maxVertex;
	float radius = 0.0f;

	for (auto vec : rawObject.positions)
	{
		if (vec.length() > radius)
		{
			radius = vec.length();
		}
		if (minVertex.getX() > vec.getX())
		{
			minVertex.setX(vec.getX());
		}
		if (minVertex.getY() > vec.getY())
		{
			minVertex.setY(vec.getY());
		}
		if (minVertex.getZ() > vec.getZ())
		{
			minVertex.setZ(vec.getZ());
		}
		if (maxVertex.getX() < vec.getX())
		{
			maxVertex.setX(vec.getX());
		}
		if (maxVertex.getY() < vec.getY())
		{
			maxVertex.setY(vec.getY());
		}
		if (maxVertex.getZ() < vec.getZ())
		{
			maxVertex.setZ(vec.getZ());
		}
	}

	float width = maxVertex.getX() - minVertex.getX();
	float height = maxVertex.getY() - minVertex.getY();
	float depth = maxVertex.getZ() - minVertex.getZ();
	float volumeBox = width * height * depth;
	float volumeSphere = 4.0f / 3.0f * static_cast<float>(SMART_PI) * radius * radius * radius;
	LOGD("Sphere radius: %f. Box width: %f, height: %f, depth: %f.", radius, width, height, depth);
	LOGD("Calculating optimal bounding volume for mesh. "
		"Box volume: %f, Sphere volume: %f. Optimal is %s.",
		volumeBox, volumeSphere, volumeSphere < volumeBox ? "sphere" : "box");
	return volumeSphere < volumeBox
		       ? static_cast<BoundingVolume*>(NEW BoundingSphere(radius))
		       : static_cast<BoundingVolume*>(NEW BoundingBox(Vec3(width, height, depth)));
}
