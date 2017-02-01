/*
 * WindowsGraphicsManager.h
 *
 *  Created on: 2012.06.19
 *      Author: Martynas Šustavičius
 */

#ifndef WINDOWSGRAPHICSMANAGER_H_
#define WINDOWSGRAPHICSMANAGER_H_

#include "../Ghost.h"
#include "../GraphicsManager.h"
class ServiceLocator;
class Shader;
class TextureRGBA;

class WindowsGraphicsManager : public GraphicsManager {
public:
	WindowsGraphicsManager(ServiceLocator* services);
	~WindowsGraphicsManager();

	bool isGraphicsContextAvailable() override;
	bool setTexture(
		UINT32& id, UINT8* image, UINT32 width, UINT32 height,
		bool wrapURepeat, bool wrapVRepeat, bool useMipmaps,
		int textureType) override;
	bool updateTexture(
		UINT32 id, UINT8* partBuffer, UINT32 rowOffset, UINT32 colOffset,
		UINT32 width, UINT32 height, bool useMipmaps, int textureType) override;
	bool setCubeMap(
		UINT32& id, UINT8** image, UINT32 width, UINT32 height,
		bool wrapURepeat, bool wrapVRepeat, bool useMipmaps) override;
	void unsetTexture(UINT32 id) override;
	bool setShader(UINT32& id, string vert, string frag, int handles[]) override;
	void unsetShader(UINT32 id) override;
	void setShaderValue(
		UINT32 id, int& handle, int type,
		UINT32 count, void* data) override;
	bool setFrameBuffer(
		UINT32& id, UINT32& color, UINT32& depth, UINT32 width, UINT32 height) override;
	void unsetFrameBuffer(UINT32& id, UINT32& color, UINT32& depth) override;
	void useFrameBuffer(UINT32 id) override;
	bool setVertexBuffer(UINT32& id, void* buffer, int size) override;
	void unsetVertexBuffer(UINT32& id) override;
	void unsetVertexBuffers(UINT32 count, UINT32*& buffers) override;
	bool setIndexBuffer(UINT32& id, void* buffer, int size) override;
	void unsetIndexBuffer(UINT32& id) override;
	void clearColorAndDepthBuffers() override;
	void setViewPort(float width, float height) override;
	bool checkSupport(Support key) override;
	pair<string, string> getDefaultSpriteShader() override;
	pair<string, string> getDefaultModelShader() override;
	pair<string, string> getDefaultTextShader() override;
	pair<string, string> getDefaultSurfaceShader() override;
};

#endif
