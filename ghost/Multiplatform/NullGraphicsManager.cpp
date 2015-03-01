#include "NullGraphicsManager.h"

NullGraphicsManager::NullGraphicsManager(ServiceLocator* services) : GraphicsManager(services) {
	LOGD("Created null graphics manager.");
}

NullGraphicsManager::~NullGraphicsManager() {
	LOGD("Deleted null graphics manager.");
}

bool NullGraphicsManager::isGraphicsContextAvailable() {
	return true;
}

bool NullGraphicsManager::setTexture(
	UINT32& id, UINT8* image, UINT32 width, UINT32 height,
	bool wrapURepeat, bool wrapVRepeat, bool useMipmaps, int textureType)
{
	id = 0;
	return true;
}

bool NullGraphicsManager::updateTexture(
	UINT32 id, UINT8* partBuffer, UINT32 rowOffset, UINT32 colOffset,
	UINT32 width, UINT32 height, bool useMipmaps, int textureType)
{
	return true;
}

bool NullGraphicsManager::setCubeMap(
	UINT32& id, UINT8** images, UINT32 width, UINT32 height,
	bool wrapURepeat, bool wrapVRepeat, bool useMipmaps)
{
	return true;
}

void NullGraphicsManager::unsetTexture(UINT32 id) {}

bool NullGraphicsManager::setShader(
	UINT32& id, string vert, string frag, int handles[])
{
	id = 0;
	return true;
}

void NullGraphicsManager::unsetShader(UINT32 id) {}

void NullGraphicsManager::setShaderValue(
	UINT32 id, int& handle, int type,
	UINT32 count, void* data)
{
	handle = 0;
}

bool NullGraphicsManager::setFrameBuffer(
	UINT32& id, UINT32& color, UINT32& depth, UINT32 width, UINT32 height)
{
	id = 0;
	color = 0;
	depth = 0;
	return true;
}

void NullGraphicsManager::unsetFrameBuffer(UINT32& id, UINT32& color, UINT32& depth) {
	id = 0;
}

void NullGraphicsManager::useFrameBuffer(UINT32 id) {}

bool NullGraphicsManager::setVertexBuffer(UINT32& id, void* buffer, int size) {
	id = 0;
	return true;
}

void NullGraphicsManager::unsetVertexBuffer(UINT32& id) {
	id = 0;
}

void NullGraphicsManager::unsetVertexBuffers(UINT32 count, UINT32*& buffers) {
	delete [] buffers;
	buffers = 0;
}

bool NullGraphicsManager::checkSupport(Support key) {
	return true;
}