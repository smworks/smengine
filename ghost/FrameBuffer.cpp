/*
 * FrameBuffer.cpp
 *
 *  Created on: 2013.05.02
 *      Author: Martynas Šustavičius
 */

#include "FrameBuffer.h"
#include "Multiplatform/GraphicsManager.h"

FrameBuffer::FrameBuffer(GraphicsManager* graphicsManager, UINT32 width, UINT32 height) :
	graphicsManager_(graphicsManager),
	id_(0),
	color_(0),
	depth_(0)
{
	graphicsManager_->setFrameBuffer(id_, color_, depth_, width, height);
}

FrameBuffer::~FrameBuffer() {
	graphicsManager_->unsetFrameBuffer(id_, color_, depth_);
}

UINT32 FrameBuffer::getId() {
	return id_;
}

UINT32 FrameBuffer::getColorBuffer() {
	return color_;
}

UINT32 FrameBuffer::getDepthBuffer() {
	return depth_;
}
