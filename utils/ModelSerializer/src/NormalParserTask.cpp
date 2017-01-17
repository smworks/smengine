#include "NormalParserTask.h"
#include "../../../ghost/Utils.h"

NormalParserTask::NormalParserTask(const char* data, UINT8* vertices, UINT32 offset, UINT32 size):
	data(data),
	vertices(vertices),
	offset(offset),
	size(size)
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
		if (data[pos] == 'v')
		{
			if (data[pos + 1] == 'n')
			{
				if (lineEnd - pos > lineLength)
				{
					lineLength = lineEnd - pos;
					delete[] line;
					line = NEW char[lineLength];
				}
				bool one = true;
				SIZE offset = 0;
				memcpy(line, data + pos, lineEnd - pos);
				for (SIZE i = 3; i < lineEnd; i++)
				{
					if (line[i] == ' ')
					{
						if (one)
						{
							normVec[0] = toFloat(line + 2);
							one = false;
							offset = i;
						}
						else
						{
							normVec[1] = toFloat(line + offset + 1);
							offset = i;
							normVec[2] = toFloat(line + offset + 1);
							break;
						}
					}
				}
				memcpy(&vertices[index++ * size + offset], &normVec[0], sizeof(float) * 3);
			}
		}
		pos = lineEnd + 1;
	}
	delete[] line;
}
