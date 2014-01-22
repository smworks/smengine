/*
 * RenderPass.cpp
 *
 *  Created on: 2013.05.02
 *      Author: Martynas Šustavičius
 */

#include "RenderPass.h"
#include "Multiplatform/GraphicsManager.h"
#include "FrameBuffer.h"
#include "Camera.h"
#include "Node.h"
#include "Multiplatform/GraphicsManager.h"
#include "Multiplatform/ServiceLocator.h"

RenderPass::RenderPass(ServiceLocator* services) :
	services_(services),
	renderContent_(0),\
	renderToScreen_(false),
	enabled_(true),
	shader_(0),
	scaleWidth_(1.0f),
	scaleHeight_(1.0f)
{
}

RenderPass::~RenderPass() {}

void RenderPass::setRenderToScreen(bool state) {
	renderToScreen_ = state;
}

void RenderPass::setRenderContent(int content) {
    renderContent_ = content;
}

void RenderPass::setShader(Shader* shader) {
	shader_ = shader;
}

void RenderPass::setEnabled(bool state) {
    enabled_ = state;
}

void RenderPass::render(FrameBuffer* read, FrameBuffer* write) {
	static float lastScaleWidth;
	static float lastScaleHeight;
	if (!renderToScreen_) {
		services_->getGraphicsManager()->useFrameBuffer(write->getId());
	}
	else {
		services_->getGraphicsManager()->useFrameBuffer(0);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0,
		int(float(services_->getScreenWidth()) * scaleWidth_),
		int(float(services_->getScreenHeight()) * scaleHeight_));
	if (renderContent_ != 0) {
		services_->getGraphicsManager()->renderScene(
			static_cast<GraphicsManager::NodeType>(renderContent_));
	} else if (!renderToScreen_) {
		services_->getGraphicsManager()->renderQuad(
			shader_, read->getColorBuffer(), read->getDepthBuffer(),
			lastScaleWidth, lastScaleHeight);
	}
	if (renderToScreen_ && shader_ != 0) {
		services_->getGraphicsManager()->renderQuad(
			shader_, read->getColorBuffer(), read->getDepthBuffer(),
			lastScaleWidth, lastScaleHeight);
	}
	lastScaleWidth = scaleWidth_;
	lastScaleHeight = scaleHeight_;
}

bool RenderPass::isEnabled() {
    return enabled_;
}

void RenderPass::setFrameScale(float widthScale, float heightScale) {
	scaleWidth_ = widthScale;
	scaleHeight_ = heightScale;
}