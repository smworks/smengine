/*
 * WindowsGraphicsManager.cpp
 *
 *  Created on: 2012.06.19
 *      Author: Martynas Šustavičius
 */

#include "WindowsGraphicsManager.h"
#include "WindowsFileManager.h"
#include "../ServiceLocator.h"
#include "../../Resources/Shader.h"
#include "../../Resources/Texture.h"

bool isExtensionSupported(string extension) {
	string extensions((const char*) glGetString(GL_EXTENSIONS));
	if (extensions.find(extension) != string::npos) {
		return true;
	}
	return false;
}

const char* getGLString(GLenum id) {
    const char *info = (const char *) glGetString(id);
	return info;
}

WindowsGraphicsManager::WindowsGraphicsManager(ServiceLocator* services) : GraphicsManager(services) {
	LOGD("OpenGL information:");
	LOGD("Version: %s.", getGLString(GL_VERSION));
	LOGD("Vendor: %s.", getGLString(GL_VENDOR));
	LOGD("Renderer: %s.", getGLString(GL_RENDERER));
	LOGD("Extensions: %s.", getGLString(GL_EXTENSIONS));
	LOGD("Created Windows graphics manager.");
}

WindowsGraphicsManager::~WindowsGraphicsManager() {
	LOGD("Deleted Windows graphics manager.");
}

bool WindowsGraphicsManager::isGraphicsContextAvailable() {
	return wglGetCurrentContext() != 0;
}

bool WindowsGraphicsManager::setTexture(
	UINT32& id, UINT8* image, UINT32 width, UINT32 height,
	bool wrapURepeat, bool wrapVRepeat, bool useMipmaps, int textureType)
{
	glGenTextures(1, &id);
	bindTexture(id);
	int type = GL_RGBA;
	if (textureType == Texture::MONO) {
		type = GL_ALPHA;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}
	glTexImage2D(
		GL_TEXTURE_2D, 0, type,
		width, height, 0, type,
		GL_UNSIGNED_BYTE, image);
	int minFilter = GL_LINEAR;
	if (useMipmaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
		minFilter = GL_LINEAR_MIPMAP_LINEAR;
	}
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int wrap_u = wrapURepeat ?
		GL_REPEAT : GL_CLAMP_TO_EDGE;
	int wrap_v = wrapVRepeat ?
		GL_REPEAT : GL_CLAMP_TO_EDGE;
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_S, wrap_u);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_T, wrap_v);
	return !checkGLError("Loading texture.");
}

bool WindowsGraphicsManager::updateTexture(
	UINT32 id, UINT8* partBuffer, UINT32 rowOffset, UINT32 colOffset,
	UINT32 width, UINT32 height, bool useMipmaps, int textureType)
{
	int type = GL_RGBA;
	if (textureType == Texture::MONO) {
		type = GL_ALPHA;
	}
	bindTexture(id);
	glTexSubImage2D(GL_TEXTURE_2D, 0, colOffset, rowOffset,
		width, height, type, GL_UNSIGNED_BYTE, partBuffer);
	if (useMipmaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	return !checkGLError("Updating texture.");
}

bool WindowsGraphicsManager::setCubeMap(
	UINT32& id, UINT8** images, UINT32 width, UINT32 height,
	bool wrapURepeat, bool wrapVRepeat, bool useMipmaps)
{
	glGenTextures(1, &id);
	bindTexture(id, 0, CUBE_MAP);
	for (int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGBA, width, height,
			0, GL_RGBA, GL_UNSIGNED_BYTE, images[i]);
	}
	int minFilter = GL_LINEAR;
	if (useMipmaps) {
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		minFilter = GL_LINEAR_MIPMAP_LINEAR;
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP,
		GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,
	GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int wrap_u = wrapURepeat ?
		GL_REPEAT : GL_CLAMP_TO_EDGE;
	int wrap_v = wrapVRepeat ?
		GL_REPEAT : GL_CLAMP_TO_EDGE;
	glTexParameteri(
		GL_TEXTURE_CUBE_MAP,
		GL_TEXTURE_WRAP_S, wrap_u);
	glTexParameteri(
		GL_TEXTURE_CUBE_MAP,
		GL_TEXTURE_WRAP_T, wrap_v);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP,
	//	GL_TEXTURE_WRAP_R, GL_REPEAT);
	//glTexEnvi(GL_TEXTURE_ENV,
	//	GL_TEXTURE_ENV_MODE, GL_MODULATE);
	return !checkGLError("Loading cube map.");
}

void WindowsGraphicsManager::unsetTexture(UINT32 id) {
	glDeleteTextures(1, &id);
}

/**
 * Loads shader into GPU memory.
 * @param type - type of the shader.
 * @param source - shader content.
 * @return Id of the shader, or
 * zero if method failed.
 */
int loadShader(GLuint type, const char* source) {
	GLuint id;
	GLint compiled;
	if ((id = glCreateShader(type)) == 0) {
		LOGW("Call to glCreateShader failed.");
		return 0;
	}
	glShaderSource(id, 1, (const GLchar**) &source, 0);
	glCompileShader(id);
	glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE) {
		GLint infoLen = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1) {
			char* infoLog = (char*) malloc(sizeof(char) * infoLen);
			glGetShaderInfoLog(id, infoLen, 0, infoLog);
			LOGE("Error compiling shader: %s", infoLog);
			LOGE("Shader content:\n%s", source);
			free(infoLog);
		}
		else {
			LOGW("Unsuccessful shader compilation with no error description.");
		}
		glDeleteShader(id);
		return false;
	}
	return id;
}

bool WindowsGraphicsManager::setShader(
	UINT32& id, const string& name, int handles[])
{
	string vertex, fragment;
	services_->getFileManager()->loadText(vertex,
		(GHOST_SHADERS + name + SHADER_VERTEX_SUFFIX).c_str());
	services_->getFileManager()->loadText(fragment,
		(GHOST_SHADERS + name + SHADER_FRAGMENT_SUFFIX).c_str());
	if (vertex.length() == 0) {
		LOGW("Vertex shader is empty for \"%s\".", name.c_str());
		return false;
	}
	if (fragment.length() == 0) {
		LOGW("Fragment shader is empty for \"%s\".", name.c_str());
		return false;
	}
	GLuint vertexId, fragmentId;
	GLint linked;
	if ((vertexId = loadShader(GL_VERTEX_SHADER, vertex.c_str())) == 0) {
		LOGW("Vertex part of shader not loaded for \"%s\".", name.c_str());
		return false;
	}
	if ((fragmentId = loadShader(GL_FRAGMENT_SHADER, fragment.c_str())) == 0) {
		LOGW("Fragment part of shader not loaded for \"%s\".", name.c_str());
		glDeleteShader(vertexId);
		return false;
	}
	if ((id = glCreateProgram()) == 0) {
		LOGW("Unable to create shader program.");
		glDeleteShader(vertexId);
		glDeleteShader(fragmentId);
		glDeleteProgram(id);
		return false;
	}
	glAttachShader(id, vertexId);
	glAttachShader(id, fragmentId);
	glLinkProgram(id);
	glDeleteShader(vertexId);
	glDeleteShader(fragmentId);
	// Check the link status
	glGetProgramiv(id, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLint infoLen = 0;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1) {
			char* infoLog = (char*) malloc(sizeof(char) * infoLen);
			glGetProgramInfoLog(id, infoLen, NULL, infoLog);
			LOGW("Error linking program: %s.", infoLog);
			free(infoLog);
		}
		else {
			LOGW("Failed to link shader. No error description available.");
		}
		glDeleteProgram(id);
		return false;
	}
	// Use this program to assign handles.
	glUseProgram(id);
	// Clear all handles.
	std::fill(handles, handles + Shader::HANDLE_COUNT, -1);
	// Assign handles.
	handles[Shader::WVP] = glGetUniformLocation(id, SHADER_WVP);
	handles[Shader::W] = glGetUniformLocation(id, SHADER_W);
	handles[Shader::N] = glGetUniformLocation(id, SHADER_N);
	handles[Shader::LIGHT_POS] = glGetUniformLocation(id, SHADER_LIGHT_POS);
	handles[Shader::LIGHT_COUNT] = glGetUniformLocation(
		id, SHADER_LIGHT_COUNT);
	handles[Shader::EYE_POS] = glGetUniformLocation(id, SHADER_EYE_POS);
	handles[Shader::AMBIENT] = glGetUniformLocation(id, SHADER_AMBIENT);
	handles[Shader::DIFFUSE] = glGetUniformLocation(id, SHADER_DIFFUSE);
	handles[Shader::SPECULAR] = glGetUniformLocation(id, SHADER_SPECULAR);
	handles[Shader::SPECULARITY] = glGetUniformLocation(
		id, SHADER_SPEC_INTENSITY);
	handles[Shader::TRANSPARENCY] = glGetUniformLocation(
		id, SHADER_TRANSPARENCY);
	handles[Shader::FOG_COLOR] = glGetUniformLocation(id, SHADER_FOG_COLOR);
	handles[Shader::FOG_DENSITY] = glGetUniformLocation
		(id, SHADER_FOG_DENSITY);
	handles[Shader::TIMER] = glGetUniformLocation(id, SHADER_TIMER);
	handles[Shader::POS] = glGetAttribLocation(id, SHADER_POS);
	handles[Shader::NORMAL] = glGetAttribLocation(id,	SHADER_NORMAL);
	handles[Shader::UV] = glGetAttribLocation(id,	SHADER_UV);
	handles[Shader::COL] = glGetAttribLocation(id, SHADER_COL);
	handles[Shader::FOREGROUND] = glGetUniformLocation(id, SHADER_FOREGROUND);
	handles[Shader::BACKGROUND] = glGetUniformLocation(id, SHADER_BACKGROUND);
	handles[Shader::CUBE_MAP] = glGetUniformLocation(id, SHADER_CUBE_MAP);
	handles[Shader::MAIN_TEXTURE] = glGetUniformLocation(
		id, SHADER_HAS_MAIN_TEXTURE);
	handles[Shader::SCREEN_WIDTH] = glGetUniformLocation(
		id, SHADER_SCREEN_WIDTH);
	handles[Shader::SCREEN_HEIGHT] = glGetUniformLocation(
		id, SHADER_SCREEN_HEIGHT);
	handles[Shader::COLOR_BUFFER] = glGetUniformLocation(
		id, SHADER_COLOR_BUFFER);
	handles[Shader::DEPTH_BUFFER] = glGetUniformLocation(
		id, SHADER_DEPTH_BUFFER);
	// Stop using this program.
	glUseProgram(0);
	return !checkGLError("Loading shader.");
}

void WindowsGraphicsManager::unsetShader(UINT32 id) {
	glDeleteProgram(id);
}

void WindowsGraphicsManager::setShaderValue(
	UINT32 id, int& handle, int type,
	UINT32 count, void* data)
{
	switch (type) {
	case Shader::INT:
		glUniform1i(handle, *(int*) data);
		break;
	case Shader::FLOAT:
		glUniform1fv(handle, count, (float*) data);
		break;
	case Shader::VECTOR3:
		glUniform3fv(handle, count, (float*) data);
		break;
	case Shader::VECTOR4:
		glUniform4fv(handle, count, (float*) data);
		break;
	case Shader::MATRIX3:
		glUniformMatrix3fv(handle, count, false, (float*) data);
		break;
	case Shader::MATRIX4:
		glUniformMatrix4fv(handle, count, false, (float*) data);
		break;
	default:
		LOGW("Unknown shader type.");
		break;
	}
}

bool WindowsGraphicsManager::setFrameBuffer(
	UINT32& id, UINT32& color, UINT32& depth, UINT32 width, UINT32 height)
{
	unsetFrameBuffer(id, color, depth);
	glGenFramebuffers(1, &id);
	glGenTextures(1, &color);
	glGenTextures(1, &depth);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	bindTexture(color);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		width,
		height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, color, 0);
	bindTexture(depth);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_DEPTH24_STENCIL8,
		width,
		height,
		0,
		GL_DEPTH_STENCIL,
		GL_UNSIGNED_INT_24_8,
		NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, depth, 0);
	GLenum status;
	status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	switch (status) {
	case GL_FRAMEBUFFER_COMPLETE:
		// Success.
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		LOGE("Frame buffer format not supported.");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		LOGE("Not all framebuffer attachment points are framebuffer"
			"attachment complete. This means that at least one "
			"attachment point with a renderbuffer or texture "
			"attached has its attached object no longer in existence "
			"or has an attached image with a width or height of "
			"zero, or the color attachment point has a "
			"non-color-renderable image attached, or the "
			"depth attachment point has a non-depth-renderable "
			"image attached, or the stencil attachment point has a "
			"non-stencil-renderable image attached.\n\n"
			"Color-renderable formats include GL_RGBA4, "
			"GL_RGB5_A1, and GL_RGB565. "
			"GL_DEPTH_COMPONENT16 is the only "
			"depth-renderable format. "
			"GL_STENCIL_INDEX8 is the only "
			"stencil-renderable format. ");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		LOGE("No images are attached to the framebuffer.");
		break;
	default:
		LOGE("Unknown frame buffer error: 0x%x.", status);
		break;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return !checkGLError("Setting frame buffer.");
}

void WindowsGraphicsManager::unsetFrameBuffer(UINT32& id, UINT32& color, UINT32& depth) {
	if (id != 0) {
		glDeleteFramebuffers(1, &id);
		id = 0;
	}
	if (color != 0) {
		glDeleteTextures(1, &color);
		color = 0;
	}
	if (depth != 0) {
		glDeleteTextures(1, &depth);
		depth = 0;
	}
}

void WindowsGraphicsManager::useFrameBuffer(UINT32 id) {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

bool WindowsGraphicsManager::setVertexBuffer(UINT32& id, void* buffer, int size) {
	if (id == 0) {
		glGenBuffers(1, &id);
	}
	bindBuffer(id);
	glBufferData(GL_ARRAY_BUFFER, size,	buffer, GL_STATIC_DRAW);
	return !checkGLError("Setting vertex buffer");
}

void WindowsGraphicsManager::unsetVertexBuffer(UINT32& id) {
	if (id != 0) {
		bindBuffer(0);
		glDeleteBuffers(1, &id);
		id = 0;
	}
}

bool WindowsGraphicsManager::checkSupport(Support key) {
	switch (key) {
	case SUPPORT_NPOT_TEXTURES:
		return isExtensionSupported("ARB_texture_non_power_of_two");
		break;
	case SUPPORT_UINT_INDEX:
		return true;
		break;
	default:
		return false;
	}
}