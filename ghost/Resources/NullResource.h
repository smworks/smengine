#ifndef NULLRESOURCE_H_
#define NULLRESOURCE_H_

#include "../Multiplatform/Ghost.h"
#include "Resource.h"

class NullResource : public Resource {
public:
	NullResource(ServiceLocator* services) : Resource(services) {}
	~NullResource() {}

	/**
	 * @see Resource
	 */
	void release() {}
	SIZE getSize() { return sizeof(NullResource); }
	Resource::Type getType() { return Resource::NULL_RESOURCE; }
	bool isValid() { return true; }
protected:

	/**
	 * @see Resource
	 */
	bool create() { return true; }
};

#endif