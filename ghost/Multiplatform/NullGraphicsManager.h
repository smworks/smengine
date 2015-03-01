#ifndef NULLGRAPHICSMANAGER_H_
#define NULLGRAPHICSMANAGER_H_

#include "Ghost.h"
#include "GraphicsManager.h"
class ServiceLocator;
class Shader;
class TextureRGBA;

class NullGraphicsManager : public GraphicsManager {
public:
	NullGraphicsManager(ServiceLocator* services);
	~NullGraphicsManager();

	bool isGraphicsContextAvailable();
	bool setTexture(
		UINT32& id, UINT8* image, UINT32 width, UINT32 height,
		bool wrapURepeat, bool wrapVRepeat, bool useMipmaps,
		int textureType);
	bool updateTexture(
		UINT32 id, UINT8* partBuffer, UINT32 rowOffset, UINT32 colOffset,
		UINT32 width, UINT32 height, bool useMipmaps, int textureType);
	bool setCubeMap(
		UINT32& id, UINT8** image, UINT32 width, UINT32 height,
		bool wrapURepeat, bool wrapVRepeat, bool useMipmaps);
	void unsetTexture(UINT32 id);
	bool setShader(UINT32& id, string vert, string frag, int handles[]);
	void unsetShader(UINT32 id);
	void setShaderValue(
		UINT32 id, int& handle, int type,
		UINT32 count, void* data);
	int getShaderHandle(UINT32 id, const string& name);
	bool setFrameBuffer(
		UINT32& id, UINT32& color, UINT32& depth, UINT32 width, UINT32 height);
	void unsetFrameBuffer(UINT32& id, UINT32& color, UINT32& depth);
	void useFrameBuffer(UINT32 id);
	bool setVertexBuffer(UINT32& id, void* buffer, int size);
	void unsetVertexBuffer(UINT32& id);
	void unsetVertexBuffers(UINT32 count, UINT32*& buffers);
	bool checkSupport(Support key);
};

#endif
