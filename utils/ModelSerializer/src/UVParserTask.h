#ifndef UVPARSERTASK_H_
#define UVPARSERTASK_H_

#include "../../../ghost/Multiplatform/Ghost.h"
#include "../../../ghost/Thread.h"

class UVParserTask : public Task
{
public:
	UVParserTask(const char* data, UINT8* vertices, UINT32 offset, UINT32 size);

	void run() override;;
private:
	const char* data;
	UINT8* vertices;
	UINT32 offset;
	UINT32 size;
};

#endif
