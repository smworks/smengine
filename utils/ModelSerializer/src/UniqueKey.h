#ifndef UNIQUEKEY_H_
#define UNIQUEKEY_H_

#include "../../../ghost/Multiplatform/Ghost.h"

struct UniqueKey
{
	UINT32 buffer[3];

	bool operator==(const UniqueKey& r) const
	{
		return buffer[0] == r.buffer[0] &&
			buffer[1] == r.buffer[1] &&
			buffer[2] == r.buffer[2];
	}

	bool operator<(const UniqueKey& r) const
	{
		if (buffer[0] < r.buffer[0])
		{
			return true;
		}
		else if (buffer[0] == r.buffer[0])
		{
			if (buffer[1] < r.buffer[1])
			{
				return true;
			}
			else if (buffer[1] == r.buffer[1])
			{
				return buffer[2] < r.buffer[2];
			}
			return false;
		}
		return false;
	}
};

struct UniqueKeyHash
{
	SIZE operator()(const UniqueKey& key) const
	{
		size_t h = 0;
		for (UINT32 i = 0; i < 3; i++)
		{
			h = 5 * h + key.buffer[i];
		}
		return size_t(h);
	}
};

#endif
