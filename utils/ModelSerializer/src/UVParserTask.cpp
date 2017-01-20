#include "UVParserTask.h"
#include "../../../ghost/Utils.h"
#include "RawObject.h"

UVParserTask::UVParserTask(const char* data, RawObject& rawObject):
	data(data),
	rawObject(rawObject)
{
}

void UVParserTask::run()
{
	SIZE lineEnd = 0;
	SIZE pos = 0;
	SIZE lineLength = 255;
	char* line = NEW char[lineLength];
	UINT32 index = 0;
	float uvVec[2];
	while (true)
	{
		const char* found = strchr(data + pos, GHOST_NEWLINE);
		if (!found)
		{
			break;
		}
		lineEnd = found - data;
		if (data[pos] == 'v')
		{
			if (data[pos + 1] == 't')
			{
				if (lineEnd - pos > lineLength)
				{
					lineLength = lineEnd - pos;
					delete[] line;
					line = NEW char[lineLength];
				}
				memcpy(line, data + pos, lineEnd - pos);
				for (SIZE i = 3; i < lineEnd; i++)
				{
					if (line[i] == ' ')
					{
						uvVec[0] = toFloat(line + 2);
						uvVec[1] = toFloat(line + i + 1);
						break;
					}
				}
				memcpy(&rawObject.vertices[index++ * rawObject.vertexProperties.vertexSize + rawObject.vertexProperties.uvOffset], &uvVec[0], sizeof(float) * 2);
			}
		}
		pos = lineEnd + 1;
	}
	delete[] line;
}
