#ifndef FACE_H_
#define FACE_H_

#include "../../../ghost/Multiplatform/Ghost.h"

struct Face {
	UINT32 posIndices[3];
	UINT32 texIndices[3];
	UINT32 normIndices[3];
	static const int FACE_SIZE = 3;
};

#endif
