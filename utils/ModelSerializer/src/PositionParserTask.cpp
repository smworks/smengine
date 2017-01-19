#include "PositionParserTask.h"
#include "../../../ghost/Utils.h"
#include "../../../ghost/Vec3.h"

PositionParserTask::PositionParserTask(const char* data, UINT8* vertices, UINT32 offset, UINT32 size, RawObject& rawObject, float& maxVertexPos):
	data(data),
	vertices(vertices),
	rawObject(rawObject),
	offset(offset),
	size(size),
	maxVertexPos(maxVertexPos)
{
}

void PositionParserTask::run()
{
	SIZE lineEnd = 0;
	SIZE pos = 0;
	SIZE lineLength = 255;
	char* line = NEW char[lineLength];
	UINT32 index = 0;
	float posVec[3];
	float adjustRatio = 1.0f / maxVertexPos;
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
			if (data[pos + 1] == ' ')
			{
				bool one = true;
				SIZE off = 0;
				if (lineEnd - pos > lineLength)
				{
					lineLength = lineEnd - pos;
					delete[] line;
					line = NEW char[lineLength];
				}
				memcpy(line, data + pos, lineEnd - pos);
				for (SIZE i = 2; i < lineEnd; i++)
				{
					if (line[i] == ' ')
					{
						if (one)
						{
							posVec[0] = toFloat(line + 2) * adjustRatio;
							one = false;
							off = i;
						}
						else
						{
							posVec[1] = toFloat(line + off + 1) * adjustRatio;
							off = i;
							posVec[2] = toFloat(line + off + 1) * adjustRatio;
							break;
						}
					}
				}
				memcpy(&vertices[index++ * size + offset], &posVec[0], sizeof(float) * 3);
				static Vec3 tmp;
				tmp.setXYZ(posVec[0], posVec[1], posVec[2]);
				if (tmp.length() > rawObject.radius)
				{
					rawObject.radius = tmp.length();
				}
				if (rawObject.minVertex.getX() > posVec[0])
				{
					rawObject.minVertex.setX(posVec[0]);
				}
				if (rawObject.minVertex.getY() > posVec[1])
				{
					rawObject.minVertex.setY(posVec[1]);
				}
				if (rawObject.minVertex.getZ() > posVec[2])
				{
					rawObject.minVertex.setZ(posVec[2]);
				}
				if (rawObject.maxVertex.getX() < posVec[0])
				{
					rawObject.maxVertex.setX(posVec[0]);
				}
				if (rawObject.maxVertex.getY() < posVec[1])
				{
					rawObject.maxVertex.setY(posVec[1]);
				}
				if (rawObject.maxVertex.getZ() < posVec[2])
				{
				rawObject.maxVertex.setZ(posVec[2]);
				}
			}
		}
		pos = lineEnd + 1;
	}
	delete[] line;
}
