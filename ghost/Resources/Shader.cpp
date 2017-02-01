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

const string Shader::ATTR_VERTEX_SHADER = "vertex_shader";
const string Shader::ATTR_FRAGMENT_SHADER = "fragment_shader";

Shader::Shader(ServiceLocator* services) :
	Resource(services),
	id(0)
{
}

Shader::~Shader()
{
	Shader::release();
}

bool Shader::create()
{
	string file = getAttribute(ATTR_FILE);
	string vert, frag, name;

	if (file.length() > 0)
	{
		vert = getFileManager()->loadText(GHOST_SHADERS + file + SHADER_VERTEX_SUFFIX);
		frag = getFileManager()->loadText(GHOST_SHADERS + file + SHADER_FRAGMENT_SUFFIX);
		name = file;
	}
	else
	{
		vert = getAttribute(ATTR_VERTEX_SHADER);
		frag = getAttribute(ATTR_FRAGMENT_SHADER);
		name = getName();
	}

	ASSERT(vert.length() > 0 && frag.length() > 0,
		"Neither shader file nor vertex and fragment codes specified for node: %s.",
		name.c_str());
	ASSERT(getGraphicsManager()->setShader(id, vert, frag, handles),
		"Unable to compile shader: %s.", name.c_str());

	LOGD("Loaded \"%s\" shader.", name.c_str());
	return true;
}

void Shader::release()
{
	if (id != 0)
	{
		getGraphicsManager()->unsetShader(id);
		id = 0;
	}
}

SIZE Shader::getSize()
{
	return sizeof(Shader);
}

Resource::Type Shader::getType()
{
	return SHADER;
}

bool Shader::isValid()
{
	return id > 0;
}

UINT32 Shader::getId() const
{
	return id;
}

void Shader::setVertexAndFragment(string vertex, string fragment)
{
	getAttributes().setString(ATTR_VERTEX_SHADER, vertex);
	getAttributes().setString(ATTR_FRAGMENT_SHADER, fragment);
}

bool Shader::hasHandle(HandleType type) const
{
	return handles[type] != -1;
}

void Shader::setMatrix3(HandleType type, float* data)
{
	if (handles[type] == -1)
	{
		return;
	}
	getGraphicsManager()->setShaderValue(
		id, handles[type], MATRIX3, 1, data);
}

void Shader::setMatrix4(HandleType type, float* data)
{
	if (handles[type] == -1)
	{
		return;
	}
	getGraphicsManager()->setShaderValue(
		id, handles[type], MATRIX4, 1, data);
}

void Shader::setVector3(HandleType type, float* data, UINT32 count)
{
	if (handles[type] == -1)
	{
		return;
	}
	getGraphicsManager()->setShaderValue(
		id, handles[type], VECTOR3, count, data);
}

void Shader::setVector4(HandleType type, float* data, UINT32 count)
{
	if (handles[type] == -1)
	{
		return;
	}
	getGraphicsManager()->setShaderValue(
		id, handles[type], VECTOR4, count, data);
}

void Shader::setFloat(HandleType type, float data)
{
	if (handles[type] == -1)
	{
		return;
	}
	getGraphicsManager()->setShaderValue(
		id, handles[type], FLOAT, 1, &data);
}

void Shader::setFloat(HandleType type, float* data, UINT32 count)
{
	if (handles[type] == -1)
	{
		return;
	}
	getGraphicsManager()->setShaderValue(
		id, handles[type], FLOAT, 1, data);
}

void Shader::setInt(HandleType type, int data)
{
	if (handles[type] == -1)
	{
		return;
	}
	getGraphicsManager()->setShaderValue(
		id, handles[type], INT, 1, &data);
}

int& Shader::getHandle(HandleType type)
{
	return handles[type];
}
