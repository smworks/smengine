/*
 * Shader.cpp
 *
 *  Created on: 2012.06.20
 *      Author: MS
 */

#include "Shader.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Multiplatform/GraphicsManager.h"
#include "../ResourceManager.h"
#include "../ScriptManager.h"
#include "../Utils.h"

Shader::Shader(ServiceLocator* services) :
	Resource(services),
	id_(0),
	graphicsManager_(0)
{}

Shader::~Shader() {
	release();
}

bool Shader::create() {
	graphicsManager_ = getServiceLocator()->getGraphicsManager();
	if (getAttribute(ATTR_FILE).length() == 0) {
		LOGW("Shader name not specified for node: %s.", getName().c_str());
		return false;
	}
	if (!graphicsManager_->setShader(
		id_, getAttribute(ATTR_FILE), handles_))
	{
		LOGW("Unable to compile shader: %s.", getAttribute(ATTR_FILE).c_str());
		return false;
	}
    LOGD("Loaded \"%s\" shader.", getAttribute(ATTR_FILE).c_str());
	return true;
}

void Shader::release() {
	if (id_ != 0) {
		getServiceLocator()->getGraphicsManager()->unsetShader(id_);
		id_ = 0;
	}
}

SIZE Shader::getSize() {
	return sizeof(Shader);
}

Resource::Type Shader::getType() {
	return Resource::SHADER;
}

bool Shader::isValid() {
	return id_ > 0;
}

UINT32 Shader::getId() {
	return id_;
}

bool Shader::hasHandle(HandleType type) {
	return handles_[type] != -1;
}

void Shader::setMatrix3(HandleType type, float* data) {
	if (handles_[type] == -1) {
		return;
	}
	graphicsManager_->setShaderValue(
		id_, handles_[type], MATRIX3, 1, data);
}

void Shader::setMatrix4(HandleType type, float* data) {
	if (handles_[type] == -1) {
		return;
	}
	graphicsManager_->setShaderValue(
		id_, handles_[type], MATRIX4, 1, data);
}

void Shader::setVector3(HandleType type, float* data, UINT32 count) {
	if (handles_[type] == -1) {
		return;
	}
	graphicsManager_->setShaderValue(
		id_, handles_[type], VECTOR3, count, data);
}

void Shader::setVector4(HandleType type, float* data, UINT32 count) {
	if (handles_[type] == -1) {
		return;
	}
	graphicsManager_->setShaderValue(
		id_, handles_[type], VECTOR4, count, data);
}

void Shader::setFloat(HandleType type, float data) {
	if (handles_[type] == -1) {
		return;
	}
	graphicsManager_->setShaderValue(
		id_, handles_[type], FLOAT, 1, &data);
}

void Shader::setFloat(HandleType type, float* data, UINT32 count) {
	if (handles_[type] == -1) {
		return;
	}
	graphicsManager_->setShaderValue(
		id_, handles_[type], FLOAT, 1, data);
}

void Shader::setInt(HandleType type, int data) {
	if (handles_[type] == -1) {
		return;
	}
	graphicsManager_->setShaderValue(
		id_, handles_[type], INT, 1, &data);
}

int& Shader::getHandle(HandleType type) {
	return handles_[type];
}