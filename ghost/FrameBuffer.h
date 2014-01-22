/*
 * FrameBuffer.h
 *
 *  Created on: 2013.05.02
 *      Author: Martynas Šustavičius
 */

#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include "Multiplatform/Ghost.h"
class GraphicsManager;

class FrameBuffer {
public:
	FrameBuffer(GraphicsManager* graphicsManager, UINT32 width, UINT32 height);
	~FrameBuffer();

	/**
	 * @return Id of the frame buffer.
	 */
	UINT32 getId();

	/**
	 * @return Id of the color buffer texture.
	 */
	UINT32 getColorBuffer();

	/**
	 * @return Id of the depth buffer texture.
	 */
	UINT32 getDepthBuffer();
protected:
	GraphicsManager* graphicsManager_;
	UINT32 id_;
	UINT32 color_;
	UINT32 depth_;
};

#endif
