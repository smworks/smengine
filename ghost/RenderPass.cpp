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
	GraphicsManager* gm = services_->getGraphicsManager();
	static float lastScaleWidth;
	static float lastScaleHeight;
	gm->useFrameBuffer(renderToScreen_ ? 0 : write->getId());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gm->setViewPort(
		services_->getScreenWidth() * scaleWidth_,
		services_->getScreenHeight() * scaleHeight_);
	if (renderContent_ != 0) {
		gm->renderScene(
			static_cast<GraphicsManager::NodeType>(renderContent_));
	} else if (!renderToScreen_) {
		gm->renderQuad(
			shader_, read->getColorBuffer(), read->getDepthBuffer(),
			lastScaleWidth, lastScaleHeight);
	}
	if (renderToScreen_ && shader_ != 0) {
		gm->renderQuad(
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