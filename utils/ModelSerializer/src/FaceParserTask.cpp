#include "FaceParserTask.h"
#include "../../../ghost/Utils.h"
#include "RawObject.h"

FaceParserTask::FaceParserTask(const char* data, RawObject& rawObject) :
	data(data),
	rawObject(rawObject)
{
}

void FaceParserTask::run()
{
	SIZE lineEnd = 0;
	SIZE pos = 0;
	SIZE faceCount = 0;
	while (true)
	{
		const char* found = strchr(data + pos, GHOST_NEWLINE);
		if (!found)
		{
			break;
		}
		lineEnd = found - data;
		if (data[pos] == 'f')
		{
			if (!parseFace(rawObject.faces.at(faceCount++), data + pos + 2, lineEnd - pos - 2))
			{
				LOGW("Error on line: %s", string(data + pos, lineEnd - pos - 2).c_str());
			}
		}
		pos = lineEnd + 1;
	}
}

/*
* Initializes specified Face structure.
*/
bool FaceParserTask::parseFace(Face& face, const char* line, SIZE length)
{
	const char* arr[3];
	arr[0] = line;
	arr[1] = arr[2] = 0;
	SIZE sizeArray[3];
	sizeArray[0] = sizeArray[1] = sizeArray[2] = 0;
	for (SIZE i = 0; i < length; i++)
	{
		if (line[i] == ' ')
		{
			if (arr[1] == 0)
			{
				arr[1] = line + i + 1;
				sizeArray[0] = i;
			}
			else
			{
				sizeArray[1] = i - sizeArray[0] - 1;
				arr[2] = line + i + 1;
				break;
			}
		}
	}
	if (arr[2] == 0)
	{
		return false;
	}
	sizeArray[2] = length - (arr[2] - line);
	for (int i = 0; i < Face::FACE_SIZE; i++)
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
			face.normIndices_[i] = toUint(
				arr[i] + secondSlash + 1, sizeArray[i] - secondSlash - 1) - 1;
			if (secondSlash - firstSlash > 1)
			{ // Has UV.
				face.texIndices_[i] = toUint(
					arr[i] + firstSlash + 1, secondSlash - firstSlash - 1) - 1;
			}
		}
		else if (firstSlash != 0)
		{ // Has UV, but no normals.
			face.texIndices_[i] = toUint(
				arr[i] + firstSlash + 1, sizeArray[i] - firstSlash - 1) - 1;
		}
		SIZE count = firstSlash == 0 ? sizeArray[i] : firstSlash;
		face.indices_[i] = toUint(arr[i], count) - 1;
	}
	return true;
}