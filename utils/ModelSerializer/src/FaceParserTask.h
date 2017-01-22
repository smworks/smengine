#ifndef FACEPARSERTASK_H_
#define FACEPARSERTASK_H_

#include "../../../ghost/Multiplatform/Ghost.h"
#include "../../../ghost/Thread.h"
#include "Face.h"

struct RawObject;

class FaceParserTask : public Task
{
public:
	FaceParserTask(const char* data, RawObject& rawObject);

	void run() override;
	bool parseFace(const char* line, SIZE length) const;
private:
	const char* data;
	RawObject& rawObject;
};

#endif
