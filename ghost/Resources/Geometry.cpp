/*
 * Geometry.cpp
 *
 *  Created on: 2013.08.14
 *      Author: Martynas Šustavičius
 */


#include "Geometry.h"
#include "../Multiplatform/ServiceLocator.h"

Geometry::Geometry(ServiceLocator* services) :
	Resource(services),
	id_(0)
{}

Geometry::~Geometry() {}

void Geometry::release() {
	id_ = 0;
}

SIZE Geometry::getSize() {
	return sizeof(Geometry);
}

Resource::Type Geometry::getType() {
	return Resource::SCRIPT;
}

bool Geometry::isValid() {
	return false;
}

void Geometry::reset() {
	release();
	create();
}

bool Geometry::create() {
	return false;
}
