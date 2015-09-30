/*
 * Light.cpp
 *
 *  Created on: 2013 02 24
 *      Author: Martynas Šustavičius
 */

#include "Light.h"
#include "../Multiplatform/ServiceLocator.h"

Light::Light(ServiceLocator* services) :
	Resource(services),
	lightType(NONE)
{}

Light::~Light() {
	release();
}

void Light::release() {
}

SIZE Light::getSize() {
	return sizeof(Light);
}

Resource::Type Light::getType() {
	return Resource::LIGHT;
}

bool Light::isValid() {
	return lightType != NONE;
}

bool Light::create() {
	string type = getAttribute(ATTR_TYPE);
	if (type == VAL_POINT) {
		lightType = POINT_LIGHT;
	} else if (type == VAL_SPOT) {
		lightType = SPOT_LIGHT;
	} else if (type == VAL_GLOBAL) {
		lightType = GLOBAL_LIGHT;
	}
	color.setRGBA(getAttribute(ATTR_COLOR));
	return true;
}