#ifndef POSITIONPARSERTASK_H_
#define POSITIONPARSERTASK_H_

#include "../../../ghost/Thread.h"
class Vec3;

class PositionParserTask : public Task
{
public:
	PositionParserTask(
		const char* data, UINT8* vertices, UINT32 offset, UINT32 size,
		Vec3* vmin, Vec3* vmax, Vec3* radius, float& maxVertexPos);

	void run() override;
private:
	const char* data;
	UINT8* vertices;
	Vec3* vmin;
	Vec3* vmax;
	Vec3* radius;
	UINT32 offset;
	UINT32 size;
	float maxVertexPos;
};

#endif
