/*
 * AndroidGraphicsManger.cpp
 *
 *  Created on: 2013.03.31
 *      Author: Martynas Šustavičius
 */

#include "AndroidGraphicsManager.h"
#include "../ServiceLocator.h"
#include "../FileManager.h"
#include "../../Resources/Shader.h"
#include "../../Resources/TextureRGBA.h"

#define EXTENSION_DEPTH_TEXTURE "GL_OES_depth_texture"

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

string shaderSupport() {
    GLboolean isSupported;
    glGetBooleanv(GL_SHADER_COMPILER, &isSupported);
    if (!isSupported) {
        return "  Shader compiler is not supported.";
    }
	stringstream ss;
	for (int j = 0; j < 2; j++) {
		INT32 shader;
		if (j % 2 == 0) {
			ss << "  Vertex shader:\n";
			shader = GL_VERTEX_SHADER;
		}
		else {
			ss << "  Fragment shader:\n";
			shader = GL_FRAGMENT_SHADER;
		}
		INT32 range[2], precision;
		glGetShaderPrecisionFormat(shader, GL_LOW_FLOAT, range, &precision);
		ss << "    Float low precision: "
			<< range[0] << " - " << range[1] << " (" << precision << ")\n";
		glGetShaderPrecisionFormat(shader, GL_MEDIUM_FLOAT, range, &precision);
		ss << "    Float medium precision: "
			<< range[0] << " - " << range[1] << " (" << precision << ")\n";
		glGetShaderPrecisionFormat(shader, GL_HIGH_FLOAT, range, &precision);
		ss << "    Float high precision: "
			<< range[0] << " - " << range[1] << " (" << precision << ")\n";
	}
	return ss.str();
}

AndroidGraphicsManager::AndroidGraphicsManager(ServiceLocator* services) : GraphicsManager(services) {
	LOGD("OpenGL information:");
	LOGD("Version: %s.", getGLString(GL_VERSION));
	LOGD("Vendor: %s.", getGLString(GL_VENDOR));
	LOGD("Renderer: %s.", getGLString(GL_RENDERER));
	LOGD("Extensions: %s.", getGLString(GL_EXTENSIONS));
	LOGD("Shader data type precisions (values in log2 format):\n%s", shaderSupport().c_str());
	LOGD("Created Android graphics manager.");
}

AndroidGraphicsManager::~AndroidGraphicsManager() {
	LOGD("Deleted Android graphics manager.");
}

bool AndroidGraphicsManager::isGraphicsContextAvailable() {
	return false; //eglGetCurrentContext() != EGL_NO_CONTEXT;
}

bool AndroidGraphicsManager::setTexture(
	UINT32& id, UINT8* image, UINT32 width, UINT32 height,
	bool wrapURepeat, bool wrapVRepeat, bool useMipmaps,
	int textureType)
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
	int minFilter = GL_NEAREST;
	if (useMipmaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
		minFilter = GL_NEAREST_MIPMAP_NEAREST;
	}
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	int wrap_u = wrapURepeat ?
		GL_REPEAT : GL_CLAMP_TO_EDGE;
	int wrap_v = wrapVRepeat ?
		GL_REPEAT : GL_CLAMP_TO_EDGE;
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_S, wrap_u);
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_T, wrap_v);
	return !checkGLError("Loading texture");
}

bool AndroidGraphicsManager::updateTexture(
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
	return !checkGLError("Updating texture");
}

bool AndroidGraphicsManager::setCubeMap(
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
	return !checkGLError("Loading cube map");
}

void AndroidGraphicsManager::unsetTexture(UINT32 id) {
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

bool AndroidGraphicsManager::setShader(
	UINT32& id, const string& name, int handles[])
{
	string vertex, fragment;
	services_->getFileManager()->loadText(vertex, (GHOST_SHADERS + name + SHADER_VERTEX_SUFFIX).c_str());
	services_->getFileManager()->loadText(fragment, (GHOST_SHADERS + name + SHADER_FRAGMENT_SUFFIX).c_str());
	if (vertex.length() == 0) {
		LOGW("Vertex shader is empty for \"%s\".", name.c_str());
		return false;
	}
	if (fragment.length() == 0) {
		LOGW("Fragment shader is empty for \"%s\".", name.c_str());
		return false;
	}
	fragment = "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
		"precision highp float;\n"
		"#else\n"
		"precision mediump float;\n"
		"#endif\n" + fragment;
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
	return !checkGLError("Loading shader");
}

void AndroidGraphicsManager::unsetShader(UINT32 id) {
	glDeleteProgram(id);
}

void AndroidGraphicsManager::setShaderValue(
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

bool AndroidGraphicsManager::setFrameBuffer(
	UINT32& id, UINT32& color, UINT32& depth, UINT32 width, UINT32 height)
{
	unsetFrameBuffer(id, color, depth);
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glGenTextures(1, &color);
	bindTexture(color);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		width,
		height,
		0,
		GL_RGB,
		GL_UNSIGNED_SHORT_5_6_5,
		NULL);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, color, 0);
	// Depth buffer texture.
	glGenTextures(1, &depth);
	bindTexture(depth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (isExtensionSupported(EXTENSION_DEPTH_TEXTURE)) {
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_DEPTH_COMPONENT,
			width,
			height,
			0,
			GL_DEPTH_COMPONENT,
			GL_UNSIGNED_INT,
			NULL);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, depth, 0);
	}
	else {
		/*glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB,
			width,
			height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			NULL);
		glFramebufferTextureRGBA(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, depth, 0);*/
		GLuint renderBuffer;
		glGenRenderbuffers(1, &renderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
	GLenum status;
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch(status) {
	case GL_FRAMEBUFFER_COMPLETE:
		// Success.
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		LOGE("Incomplete frame buffer attachment.");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
		LOGE("Not all attached images to buffer have same dimensions.");
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		LOGE("No images are attached to the framebuffer.");
	case GL_FRAMEBUFFER_UNSUPPORTED:
		LOGE("Frame buffer format not supported.");
		break;
	default:
		LOGE("Frame buffer error.");
		break;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return !checkGLError("Setting frame buffer");
}

void AndroidGraphicsManager::unsetFrameBuffer(
	UINT32& id, UINT32& color, UINT32& depth)
{
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

void AndroidGraphicsManager::useFrameBuffer(UINT32 id) {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

bool AndroidGraphicsManager::setVertexBuffer(
	UINT32& id, void* buffer, int size)
{
	if (id == 0) {
		glGenBuffers(1, &id);
	}
	bindBuffer(id);
	glBufferData(GL_ARRAY_BUFFER, size,	buffer, GL_STATIC_DRAW);
	return !checkGLError("Setting vertex buffer");
}

void AndroidGraphicsManager::unsetVertexBuffer(UINT32& id) {
	if (id != 0) {
		bindBuffer(0);
		glDeleteBuffers(1, &id);
		id = 0;
	}
}

bool AndroidGraphicsManager::checkSupport(Support key) {
	switch (key) {
	case SUPPORT_NPOT_TEXTURES:
		return isExtensionSupported("ARB_texture_non_power_of_two") || isExtensionSupported("GL_OES_texture_npot");
		break;
	case SUPPORT_UINT_INDEX:
		return isExtensionSupported("GL_OES_element_index_uint");
		break;
	default:
		return false;
	}
}