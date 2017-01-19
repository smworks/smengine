#ifndef MATERIALINDEX_H_
#define MATERIALINDEX_H_

#include "../../../ghost/Multiplatform/Ghost.h"

struct MaterialIndex
{
	MaterialIndex() :
		name(""),
		offset(0),
		size(0)
	{
	}

	string name;
	SIZE offset;
	SIZE size;
};

#endif
