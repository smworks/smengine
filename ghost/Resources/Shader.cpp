/*
 * Shader.cpp
 *
 *  Created on: 2012.06.20
 *      Author: MS
 */

#include "Shader.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Multiplatform/GraphicsManager.h"
#include "../Multiplatform/FileManager.h"
#include "../ResourceManager.h"
#include "../ScriptManager.h"
#include "../Utils.h"

const string Shader::ATTR_VERTEX_SHADER = "vertex_shader";
const string Shader::ATTR_FRAGMENT_SHADER = "fragment_shader";

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
	FileManager* fm = getServiceLocator()->getFileManager();
	string file = getAttribute(ATTR_FILE);
	string vert, frag;
	if (file.length() > 0) {
		fm->loadText(vert, (GHOST_SHADERS + file + SHADER_VERTEX_SUFFIX).c_str());
		fm->loadText(frag, (GHOST_SHADERS + file + SHADER_FRAGMENT_SUFFIX).c_str());
		if (!graphicsManager_->setShader(id_, vert, frag, handles_)) {
			LOGW("Unable to compile shader: %s.", file.c_str());
			return false;
		}
		LOGD("Loaded \"%s\" shader.", file.c_str());
	} else {
		vert = getAttribute(ATTR_VERTEX_SHADER);
		frag = getAttribute(ATTR_FRAGMENT_SHADER);
		if (vert.length() > 0 && frag.length() > 0) {
			if (!graphicsManager_->setShader(id_, vert, frag, handles_)) {
				LOGW("Unable to compile shader: %s.", getName().c_str());
				return false;
			}
		} else {
			LOGW("Neither shader file nor vertex and fragment codes specified for node: %s.",
				getName().c_str());
			return false;
		}
	}
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

Shader* Shader::getDefaultSpriteShader(ServiceLocator* services) {
	string name = "default_sprite_shader";
	string vert = "uniform mat4 uWVP;\
attribute vec4 attrPos;\
attribute vec2 attrUV;\
varying vec2 varTexCoords;\
void main(void) {\
   gl_Position = uWVP * attrPos;\
   varTexCoords = attrUV;\
}";
	string frag = "uniform sampler2D mainTexture;\
varying vec2 varTexCoords;\
void main(void) {\
	vec4 col = texture2D(mainTexture, varTexCoords);\
	gl_FragColor = col;\
}\
";
	Shader* shader = dynamic_cast<Shader*>(services->getRM()->get(SHADER, name));
	if (shader == 0) {
		shader = NEW Shader(services);
		shader->getAttributes().setString(ATTR_VERTEX_SHADER, vert);
		shader->getAttributes().setString(ATTR_FRAGMENT_SHADER, frag);
		shader->create();
		services->getRM()->add(name, shader);
	}
	return shader;
}

Shader* Shader::getDefaultModelShader(ServiceLocator* services) {
	string name = "default_model_shader";
	string vert = "uniform mat4 uWVP;\
attribute vec4 attrPos;\
attribute vec2 attrUV;\
varying vec2 varTexCoords;\
void main(void)\
{\
	varTexCoords = attrUV;\
	gl_Position = uWVP * attrPos;\
}";
	string frag = "uniform vec3 uAmbient;\
uniform vec3 uDiffuse;\
// Texture data.\
uniform sampler2D mainTexture;\
uniform float uMainTexture;\
varying vec2 varTexCoords;\
//---------------------------------------------------------\
// Calculates fog factor for specified pixel.\
// Value 1.442695 is the result of log2.\
//---------------------------------------------------------\
/*float fogFactor() {\
	float z = gl_FragCoord.z / gl_FragCoord.w;\
	float fogFactor = exp2(\
		-uFogDensity * uFogDensity * z * z * 1.442695);\
	return clamp(fogFactor, 0.0, 1.0);\
}*/\
float linearDepth() {\
	float n = 0.1; // camera z near\
	float f = 300.0; // camera z far\
	float zoverw =gl_FragCoord.z;\
	return (2.0 * n) / (f + n - zoverw * (f - n));\
}\
void main(void)\
{\
	float depth = 1.0 - linearDepth();\
	vec4 col = vec4(depth, depth, depth, 1.0);\
	\
	if (uMainTexture > 0.5) {\
		gl_FragColor = col * texture2D(mainTexture, varTexCoords);\
	} else {\
		gl_FragColor = col;\
	}\
}";
	Shader* shader = dynamic_cast<Shader*>(services->getRM()->get(SHADER, name));
	if (shader == 0) {
		shader = NEW Shader(services);
		shader->getAttributes().setString(ATTR_VERTEX_SHADER, vert);
		shader->getAttributes().setString(ATTR_FRAGMENT_SHADER, frag);
		shader->create();
		services->getRM()->add(name, shader);
	}
	return shader;
}