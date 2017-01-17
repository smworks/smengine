#ifndef FACEPARSERTASK_H_
#define FACEPARSERTASK_H_

#include "../../../ghost/Multiplatform/Ghost.h"
#include "../../../ghost/Thread.h"
#include "Face.h"

class FaceParserTask : public Task
{
public:
	FaceParserTask(const char* data, vector<Face>* faces);

	void run() override;
	bool parseFace(Face& face, const char* line, SIZE length);
private:
	const char* data;
	vector<Face>* faces;
};

#endif
