#ifndef POSITIONPARSERTASK_H_
#define POSITIONPARSERTASK_H_

#include "../../../ghost/Thread.h"
#include "RawObject.h"
class Vec3;

class PositionParserTask : public Task
{
public:
	PositionParserTask(const char* data, RawObject& rawObject);

	void run() override;
private:
	const char* data;
	RawObject& rawObject;
};

#endif
