/*
 * AndroidGraphicsManager.h
 *
 *  Created on: 2013.03.31
 *      Author: Martynas Šustavičius
 */

#ifndef ANDROIDGRAPHICSMANAGER_H_
#define ANDROIDGRAPHICSMANAGER_H_

#include "../Ghost.h"
#include "../GraphicsManager.h"
class ServiceLocator;
class Shader;
class TextureRGBA;

class AndroidGraphicsManager : public GraphicsManager {
public:
	AndroidGraphicsManager(ServiceLocator* services);
	~AndroidGraphicsManager();
	
	/**
	 * @see GraphicsManager.
	 */
	bool isGraphicsContextAvailable();
	
	/**
	 * @see GraphicsManager.
	 */
	bool setTexture(
        UINT32& id, UINT8* image, UINT32 width, UINT32 height,
        bool wrapURepeat, bool wrapVRepeat, bool useMipmaps, int textureType);

	/**
	 * @see GraphicsManager.
	 */
	bool updateTexture(
		UINT32 id, UINT8* partBuffer, UINT32 rowOffset, UINT32 colOffset,
        UINT32 width, UINT32 height, bool useMipmaps, int textureType);

	/**
	 * @see GraphicsManager.
	 */
	bool setCubeMap(
		UINT32& id, UINT8** image, UINT32 width, UINT32 height,
		bool wrapURepeat, bool wrapVRepeat, bool useMipmaps);

	/**
	 * @see GraphicsManager.
	 */
	void unsetTexture(UINT32 id);

	/**
	 * @see GraphicsManager.
	 */
	bool setShader(UINT32& id, string vert, string frag, int handles[]);

	/**
	 * @see GraphicsManager.
	 */
	void unsetShader(UINT32 id);

	/**
	 * @see GraphicsManager.
	 */
	void setShaderValue(
		UINT32 id, int& handle, int type,
		UINT32 count, void* data);

	/**
	 * @see GraphicsManager.
	 */
	int getShaderHandle(UINT32 id, const string& name);
	
	/**
	 * @see GraphicsManager.
	 */
	bool setFrameBuffer(
		UINT32& id, UINT32& color, UINT32& depth, UINT32 width, UINT32 height);

	/**
	 * @see GraphicsManager.
	 */
	void unsetFrameBuffer(UINT32& id, UINT32& color, UINT32& depth);
	
	/**
	 * @see GraphicsManager.
	 */
	void useFrameBuffer(UINT32 id);
	
	/**
	 * @see GraphicsManager.
	 */
	bool setVertexBuffer(UINT32& id, void* buffer, int size);

	/**
	 * @see GraphicsManager.
	 */
	void unsetVertexBuffer(UINT32& id);

	/**
	 * @see GraphicsManager.
	 */
	bool checkSupport(Support key);
};

#endif
