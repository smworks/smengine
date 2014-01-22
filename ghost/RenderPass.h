/*
 * RenderPass.h
 *
 *  Created on: 2013.05.02
 *      Author: Martynas Šustavičius
 */

#ifndef RENDERPASS_H_
#define RENDERPASS_H_

#include "Multiplatform/Ghost.h"
class FrameBuffer;
class RenderManager;
class Camera;
class ServiceLocator;
class Shader;

class RenderPass {
public:
	RenderPass(ServiceLocator* services);
	~RenderPass();

	/**
	 * @param state - state to be set.
	 */
	void setRenderToScreen(bool state);

	/**
	 * Specify what to render.
	 * @param content - value of type RenderManager::NodeType.
	 */
	void setRenderContent(int content);

	/**
	 * Set shader that will be used for this render pass.
	 * @paramm shader - shader object.
	 */
	void setShader(Shader* shader);

	/**
	 * If set tu true, renders this pass, otherwise
	 * this pass will be ignored.
	 * @param state - state to be set.
	 */
	void setEnabled(bool state);

	/**
	 * @param read - read buffer.
	 * @param write - write buffer.
	 */
	void render(FrameBuffer* read, FrameBuffer* write);

	/**
	 * @return True if pass is enabled.
	 */
	bool isEnabled();

	/**
	 * @param widthScale - scale ratio for frame width.
	 * @param heightScale - scale ratio for frame height.
	 */
	void setFrameScale(float widthScale, float heightScale);
protected:
	RenderManager* renderManager_;
	Camera* camera_;
	ServiceLocator* services_;
    int renderContent_;
	bool renderToScreen_;
	bool enabled_;
	Shader* shader_;
	float scaleWidth_;
	float scaleHeight_;
};

#endif
