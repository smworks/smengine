#ifndef NORMALPARSERTASK_H_
#define NORMALPARSERTASK_H_

#include "../../../ghost/Multiplatform/Ghost.h"
#include "../../../ghost/Thread.h"

struct RawObject;

class NormalParserTask : public Task
{
public:
	NormalParserTask(const char* data, RawObject& rawObject);

	void run() override;
private:
	const char* data;
	RawObject& rawObject;
};

#endif
