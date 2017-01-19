#ifndef POSITIONPARSERTASK_H_
#define POSITIONPARSERTASK_H_

#include "../../../ghost/Thread.h"
#include "RawObject.h"
class Vec3;

class PositionParserTask : public Task
{
public:
	PositionParserTask(
		const char* data, UINT8* vertices, UINT32 offset, UINT32 size,
		RawObject& rawObject, float& maxVertexPos);

	void run() override;
private:
	const char* data;
	UINT8* vertices;
	RawObject& rawObject;
	UINT32 offset;
	UINT32 size;
	float maxVertexPos;
};

#endif
