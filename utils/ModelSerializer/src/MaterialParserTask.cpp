#include "MaterialParserTask.h"
#include "../../../ghost/Utils.h"
#include "../../../ghost/ModelData.h"
#include "../../../ghost/Resources/Texture.h"
#include "../../../ghost/Multiplatform/FileManager.h"
#include "MaterialIndex.h"
#include "RawObject.h"

MaterialParserTask::MaterialParserTask(ServiceLocator* sl, string data,
                                       RawObject& rawObject,
                                       ModelData& modelData) :
	ServiceProvider(sl),
	data(data),
	rawObject(rawObject),
	modelData(modelData)
{
}

void MaterialParserTask::run()
{
	MaterialIndex matInd;
	auto& materials = modelData.getMaterials();
	SIZE matOff = 0;
	vector<string> arr;
	SIZE params = 0;
	string line;
	SIZE pos = 0;
	SIZE lineEnd = 0;
	SIZE matSize = 0;
	while (true)
	{
		lineEnd = data.find(GHOST_NEWLINE, pos);
		if (lineEnd == string::npos)
		{
			break;
		}
		switch (data[pos])
		{
		case 'f': // Faces.
			line = data.substr(pos, lineEnd - pos);
			stringSplit(line, arr, ' ');
			params = arr.size();
			if (params == 4)
			{
				matSize++;
			}
			break;
		case 'm': // Material file.
			line = data.substr(pos, lineEnd - pos);
			stringSplit(line, arr, ' ');
			THROWONASSERT(parseMaterial(arr[1]), "Unable to parse material file: %s", arr[1].c_str());
			break;
		case 'u': // Use material.
			line = data.substr(pos, lineEnd - pos);
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
						matInd.name = arr[1];
						matInd.offset = matOff;
					}
					else
					{
						matInd.size = matSize;
						matOff += matSize;
						rawObject.matIndices.push_back(matInd);
						matInd.name = arr[1];
						matInd.offset = matOff;
						matSize = 0;
					}
				}
				else
				{
					LOGW("Material %s not found.", arr[1].c_str());
					if (matSize == 0)
					{ // First element.
						matInd.name = "default";
						matInd.offset = matOff;
					}
					else
					{
						matInd.size = matSize;
						matOff += matSize;
						rawObject.matIndices.push_back(matInd);
						matInd.name = "default";
						matInd.offset = matOff;
						matSize = 0;
					}
				}
			}
			break;
		case 'o': // Object groups.
		case '#': // Comments.
			break;
		}
		pos = lineEnd + 1;
	}
	matInd.size = matSize;
	rawObject.matIndices.push_back(matInd);
}

/*
* Loads material data into entity model.
*/
bool MaterialParserTask::parseMaterial(string file) const
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
				modelData.getMaterials().push_back(mat);
				mat = ModelData::Material();
				mat.setName(arr[1]);
			}
			break;
		}
		pos = lineEnd + 1;
	}

	modelData.getMaterials().push_back(mat);

	return true;
}
