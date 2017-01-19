#ifndef NORMALPARSERTASK_H_
#define NORMALPARSERTASK_H_

#include "../../../ghost/Multiplatform/Ghost.h"
#include "../../../ghost/Thread.h"

class NormalParserTask : public Task
{
public:
	NormalParserTask(const char* data, UINT8* vertices, UINT32 offset, UINT32 size);

	void run() override;
private:
	const char* data;
	UINT8* vertices;
	UINT32 offset;
	UINT32 size;
};

#endif
