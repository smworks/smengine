/*
 * Shader.h
 *
 *  Created on: 2012.06.20
 *      Author: MS
 */

#ifndef SHADER_H_
#define SHADER_H_

#include "../Multiplatform/Ghost.h"
#include "Resource.h"
class GraphicsManager;

#define SHADER_MAIN_TEXTURE "mainTexture"
#define SHADER_HAS_MAIN_TEXTURE "uMainTexture"
#define SHADER_VERTEX_SUFFIX ".vert"
#define SHADER_FRAGMENT_SUFFIX ".frag"
#define SHADER_WVP "uWVP"
#define SHADER_W "uW"
#define SHADER_N "uN"
#define SHADER_COLOR_BUFFER "colorBuffer"
#define SHADER_DEPTH_BUFFER "depthBuffer"
#define SHADER_SCREEN_WIDTH "uScreenWidth"
#define SHADER_SCREEN_HEIGHT "uScreenHeight"
#define SHADER_LIGHT_POS "uLightPos"
#define SHADER_LIGHT_COUNT "uLightCount"
#define SHADER_EYE_POS "uEyePos"
#define SHADER_AMBIENT "uAmbient"
#define SHADER_DIFFUSE "uDiffuse"
#define SHADER_SPECULAR "uSpecular"
#define SHADER_SPEC_INTENSITY "uSpecIntensity"
#define SHADER_TRANSPARENCY "uTransparency"
#define SHADER_FOG_COLOR "uFogColor"
#define SHADER_FOG_DENSITY "uFogDensity"
#define SHADER_TIMER "uTimer"
#define SHADER_POS "attrPos"
#define SHADER_NORMAL "attrNorm"
#define SHADER_UV "attrUV"
#define SHADER_COL "attrCol"
#define SHADER_FOREGROUND "uForeground"
#define SHADER_BACKGROUND "uBackground"
#define SHADER_TEXTURE_2D "texture_"
#define SHADER_CUBE_MAP "cubeMap_0"

class Shader : public Resource {
public:
	enum VariableType {
		MATRIX3, MATRIX4,
		VECTOR3, VECTOR4,
		FLOAT, INT
	};
	enum HandleType {
		WVP, W, N,
		LIGHT_POS, LIGHT_COUNT, EYE_POS,
		AMBIENT, DIFFUSE, SPECULAR,
		SPECULARITY, TRANSPARENCY,
		FOG_COLOR, FOG_DENSITY,
		TIMER,
		POS, NORMAL, UV, COL,
		FOREGROUND, BACKGROUND,
		CUBE_MAP,
		MAIN_TEXTURE, // Used to indicate whether renderable has texture.
		TEXTURE_0, TEXTURE_1, TEXTURE_2, TEXTURE_3,
		TEXTURE_4, TEXTURE_5, TEXTURE_6, TEXTURE_7,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		COLOR_BUFFER, DEPTH_BUFFER,
		HANDLE_COUNT
	};
	static const string ATTR_VERTEX_SHADER;
	static const string ATTR_FRAGMENT_SHADER;
public:
	Shader(ServiceLocator* services);
	~Shader();

	/**
	 * @see Resource
	 */
	bool create();

	/**
	 * @see Resource
	 */
	void release();

	/**
	 * @see Resource
	 */
	SIZE getSize();

	/**
	 * @see Resource
	 */
	Resource::Type getType();

	/**
	 * @see Resource
	 */
	bool isValid();

	/**
	 * @return Shader id.
	 */
	UINT32 getId();

	/**
	 * @param type - handle type.
	 * @return True if handle is used by shader.
	 */
	bool hasHandle(HandleType type);

	/**
	 * @param type - handle type.
	 * @param data - pointer to 3x3 matrix array.
	 */
	void setMatrix3(HandleType type, float* data);

	/**
	 * @param type - handle type.
	 * @param data - pointer to 4x4 matrix array.
	 */
	void setMatrix4(HandleType type, float* data);

	/**
	 * @param type - handle type.
	 * @param data - pointer to 3x1 vector array.
	 * @param count - number of vectors.
	 */
	void setVector3(HandleType type, float* data, UINT32 count = 1);

	/**
	 * @param type - handle type.
	 * @param data - pointer to 4x1 vector array.
	 * @param count - number of vectors.
	 */
	void setVector4(HandleType type, float* data, UINT32 count = 1);

	/**
	 * @param type - handle type.
	 * @param data - float value.
	 */
	void setFloat(HandleType type, float data);

	/**
	 * @param type - handle type.
	 * @param data - pointer to float array.
	 * @param count - number of floats.
	 */
	void setFloat(HandleType type, float* data, UINT32 count = 1);

	/**
	 * @param type - handle type.
	 * @param data - int value.
	 */
	void setInt(HandleType type, int data);

	/**
	 * @param type - handle type.
	 * @return Reference to specified handle.
	 */
	int& getHandle(HandleType type);

	/**
	 * @param services - service locator object.
	 * @return Default sprite shader.
	 */
	static Shader* getDefaultSpriteShader(ServiceLocator* services);

	/**
	 * @param services - service locator object.
	 * @return Default model shader.
	 */
	static Shader* getDefaultModelShader(ServiceLocator* services);
protected:
	UINT32 id_;
	int handles_[HANDLE_COUNT];
	GraphicsManager* graphicsManager_;
};

#endif /* SHADER_H_ */
