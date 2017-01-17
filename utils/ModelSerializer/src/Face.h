#ifndef FACE_H_
#define FACE_H_

#include "../../../ghost/Multiplatform/Ghost.h"

struct Face {
	UINT32 indices_[3];
	UINT32 texIndices_[3];
	UINT32 normIndices_[3];
	static const int FACE_SIZE = 3;
};

#endif
