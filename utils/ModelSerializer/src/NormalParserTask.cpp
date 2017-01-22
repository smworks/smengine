#include "NormalParserTask.h"


#include "../../../ghost/Utils.h"
#include "RawObject.h"

NormalParserTask::NormalParserTask(const char* data, RawObject& rawObject):
	data(data),
	rawObject(rawObject)
{
}

void NormalParserTask::run()
{
	SIZE lineEnd = 0;
	SIZE pos = 0;
	SIZE lineLength = 255;
	char* line = NEW char[lineLength];
	UINT32 index = 0;
	float normVec[3];
	while (true)
	{
		auto* found = strchr(data + pos, GHOST_NEWLINE);
		if (!found)
		{
			break;
		}
		lineEnd = found - data;
		if (data[pos] == 'v' && data[pos + 1] == 'n')
		{
			if (lineEnd - pos > lineLength)
			{
				lineLength = lineEnd - pos;
				delete[] line;
				line = NEW char[lineLength];
			}
			bool one = true;
			SIZE off = 0;
			memcpy(line, data + pos, lineEnd - pos);
			for (SIZE i = 3; i < lineEnd; i++)
			{
				if (line[i] == ' ')
				{
					if (one)
					{
						normVec[0] = toFloat(line + 2);
						one = false;
						off = i;
					}
					else
					{
						normVec[1] = toFloat(line + off + 1);
						off = i;
						normVec[2] = toFloat(line + off + 1);
						break;
					}
				}
			}
			rawObject.normals.push_back(normVec);
		}
		pos = lineEnd + 1;
	}
	delete[] line;
}
