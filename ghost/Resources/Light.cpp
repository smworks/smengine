/*
 * Light.cpp
 *
 *  Created on: 2013 02 24
 *      Author: Martynas Šustavičius
 */

#include "Light.h"
#include "../Multiplatform/ServiceLocator.h"

const string Light::ATTR_TYPE = "type";
const string Light::VAL_POINT = "point";
const string Light::VAL_SPOT = "spot";
const string Light::VAL_GLOBAL = "global";
const string Light::ATTR_COLOR = "color";

Light::Light(ServiceLocator* services) :
	Resource(services),
	lightType_(NONE)
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
	return lightType_ != NONE;
}

bool Light::create() {
	string type = getAttribute(ATTR_TYPE);
	if (type == VAL_POINT) {
		lightType_ = POINT_LIGHT;
	}
	else if (type == VAL_SPOT) {
		lightType_ = SPOT_LIGHT;
	}
	else if (type == VAL_GLOBAL) {
		lightType_ = GLOBAL_LIGHT;
	}
	color_.setRGBA(getAttribute(ATTR_COLOR));
	return true;
}