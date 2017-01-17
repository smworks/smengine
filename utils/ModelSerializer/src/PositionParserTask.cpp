#include "PositionParserTask.h"
#include "../../../ghost/Utils.h"
#include "../../../ghost/Vec3.h"

PositionParserTask::PositionParserTask(const char* data, UINT8* vertices, UINT32 offset, UINT32 size, Vec3* vmin, Vec3* vmax, Vec3* radius, float& maxVertexPos):
	data(data),
	vertices(vertices),
	vmin(vmin),
	vmax(vmax),
	radius(radius),
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
				SIZE offset = 0;
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
							offset = i;
						}
						else
						{
							posVec[1] = toFloat(line + offset + 1) * adjustRatio;
							offset = i;
							posVec[2] = toFloat(line + offset + 1) * adjustRatio;
							break;
						}
					}
				}
				memcpy(&vertices[index++ * size + offset], &posVec[0], sizeof(float) * 3);
				static Vec3 tmp;
				tmp.setXYZ(posVec[0], posVec[1], posVec[2]);
				if (tmp.length() > radius->length())
				{
					radius->setXYZ(tmp);
				}
				if (vmin->getX() > posVec[0])
				{
					vmin->setX(posVec[0]);
				}
				if (vmin->getY() > posVec[1])
				{
					vmin->setY(posVec[1]);
				}
				if (vmin->getZ() > posVec[2])
				{
					vmin->setZ(posVec[2]);
				}
				if (vmax->getX() < posVec[0])
				{
					vmax->setX(posVec[0]);
				}
				if (vmax->getY() < posVec[1])
				{
					vmax->setY(posVec[1]);
				}
				if (vmax->getZ() < posVec[2])
				{
					vmax->setZ(posVec[2]);
				}
			}
		}
		pos = lineEnd + 1;
	}
	delete[] line;
}
