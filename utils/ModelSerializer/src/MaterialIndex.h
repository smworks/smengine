#ifndef MATERIALINDEX_H_
#define MATERIALINDEX_H_

#include "../../../ghost/Multiplatform/Ghost.h"

struct MaterialIndex
{
	MaterialIndex() :
		name_(""),
		offset_(0),
		size_(0)
	{
	}

	string name_;
	SIZE offset_;
	SIZE size_;
};

#endif
