#ifndef UVPARSERTASK_H_
#define UVPARSERTASK_H_

#include "../../../ghost/Multiplatform/Ghost.h"
#include "../../../ghost/Thread.h"

struct RawObject;

class UVParserTask : public Task
{
public:
	UVParserTask(const char* data, RawObject& rawObject);

	void run() override;;
private:
	const char* data;
	RawObject& rawObject;
};

#endif
