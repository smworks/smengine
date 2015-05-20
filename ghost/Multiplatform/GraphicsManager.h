/*
 * GraphicsManager.h
 *
 *  Created on: 2013.02.25
 *      Author: Martynas Šustavičius
 */

#ifndef GRAPHICSMANAGER_H_
#define GRAPHICSMANAGER_H_

#include "Ghost.h"
#include "../Matrix.h"
#include "../Color.h"
class GraphicsManager;
class Database;
class Camera;
class ResourceManager;
class TextManager;
class Node;
class Renderable;
class Shader;
class TextureRGBA;
class RenderPass;
class FrameBuffer;
class ServiceLocator;

#define USE_POST_PROCESSING "use_post_processing"
#define PLANE "plane"

class GraphicsManager {
public:
	enum Max {
		MAX_TEXTURE_UNITS, MAX_COMBINED_TEXTURE_UNITS,
		MAX_TEXTURE_SIZE, MAX_CUBE_MAP_SIZE,
		MAX_RENDER_BUFFER_SIZE,
		MAX_VIEWPORT_WIDTH, MAX_VIEWPORT_HEIGHT,
		MAX_COUNT
	};
	enum Support {
		SUPPORT_NPOT_TEXTURES, SUPPORT_UINT_INDEX, SUPPORT_COUNT
	};
public:
	/**
	 * Checks if graphics context is available.
	 * There might be situations where context is no longer
	 * available and resourses on graphics card
	 * are already released.
	 * @return True if graphics context is available.
	 */
	virtual bool isGraphicsContextAvailable() = 0;

	/**
	 * Sets texture.
	 * @param id - id of the texture.
	 * @param image - image byte array.
	 * @param width - width in pixels.
	 * @param height - height in pixels.
	 * @param wrapURepeat - should texture be repeated horizontaly?
	 * @param wrapVRepeat - should texture be repeated verticaly?
	 * @param useMipmaps - should additional smaller images
	 * be generated for this texture?
	 * @param textureType - type of texture. Could be MONO,
	 * RGBA or some other format.
	 * @return True on success, false otherwise.
	 */
	virtual bool setTexture(
		UINT32& id, UINT8* image, UINT32 width, UINT32 height,
		bool wrapURepeat, bool wrapVRepeat, bool useMipmaps,
		int textureType) = 0;

	/**
	 * Updates specified region of specified texture.
	 * @param id - id of the texture.
	 * @param partBuffer - buffer containing updated image region.
	 * @param rowOffset - row on which updated region begins.
	 * @param colOffset - column on which updated region begins.
	 * @param width - width of the updated region.
	 * @param height - height of the updated region.
	 * @param useMipmaps - specify true if texture contains mipmaps.
	 * @param textureType - type of texture. Could be MONO,
	 * RGBA or some other format.
	 * @return True on success, false otherwise.
	 */
	virtual bool updateTexture(
		UINT32 id, UINT8* partBuffer, UINT32 rowOffset, UINT32 colOffset,
		UINT32 width, UINT32 height, bool useMipmaps, int textureType) = 0;

	/**
	 * Sets cube map textures.
	 * @param id - id of the texture.
	 * @param images - image byte array.
	 * @param width - width in pixels.
	 * @param height - height in pixels.
	 * @param wrapURepeat - should texture be repeated horizontaly?
	 * @param wrapVRepeat - should texture be repeated verticaly?
	 * @param useMipmaps - should additional smaller images
	 * be generated for this texture?
	 * @return True on success, false otherwise.
	 */
	virtual bool setCubeMap(
		UINT32& id, UINT8** images, UINT32 width, UINT32 height,
		bool wrapURepeat, bool wrapVRepeat, bool useMipmaps) = 0;

	/**
	 * Removes texture from graphics memory.
	 * @param id - id of the texture.
	 */
	virtual void unsetTexture(UINT32 id) = 0;

	/**
	 * Compiles shader.
	 * @param id - id of the shader.
	 * @param vert - vertex shader code.
	 * @param frag - fragment shader code.
	 * @param handles - array of handles to shader variables.
	 * @return True on success.
	 */
	virtual bool setShader(UINT32& id, string vert, string frag, int handles[]) = 0;

	/**
	 * Removes shader from graphics memory.
	 * @param id - id of the shader.
	 */
	virtual void unsetShader(UINT32 id) = 0;

	/**
	 * Sets value for specified shader parameter.
	 * @param handle - shader parameter handle.
	 * @param type - indicates data type.
	 * @param count - number of data units.
	 * @param pointer to single data unit or unit array.
	 */
	virtual void setShaderValue(
		UINT32 id, int& handle, int type,
		UINT32 count, void* data) = 0;

	/**
	 * Set up frame buffer.
	 * @param id - id of the buffer.
	 * @param color - id of the color buffer.
	 * @param depth - id of the depth buffer.
	 * @param width - width of the buffer.
	 * @param height - height of the buffer.
	 */
	virtual bool setFrameBuffer(
		UINT32& id, UINT32& color, UINT32& depth, UINT32 width, UINT32 height) = 0;

	/**
	 * Removes frame buffer from graphics memory.
	 * @param id - id of the frame buffer.
	 */
	virtual void unsetFrameBuffer(UINT32& id, UINT32& color, UINT32& depth) = 0;

	/**
	 * Use specified frame buffer.
	 * @param id - id of the frame buffer.
	 */
	virtual void useFrameBuffer(UINT32 id) = 0;

	/**
	 * Move vertex buffer into graphics memory.
	 * If vertex buffer was already created with specified id,
	 * its content is updated to avoid deleting old buffer
	 * and generating new.
	 * @param id - id of the vertex buffer.
	 * @param buffer - array of vertex data.
	 * @param size - array size in bytes.
	 */
	virtual bool setVertexBuffer(UINT32& id, void* buffer, int size) = 0;

	/**
	 * Remove vertex buffer from graphics memory.
	 * @param id - id of the vertex buffer.
	 */
	virtual void unsetVertexBuffer(UINT32& id) = 0;

	/**
	 * Remove array of vertex buffers from graphics memory.
	 * @param count - buffer count.
	 * @param buffers - array of buffer ids.
	 */
	virtual void unsetVertexBuffers(UINT32 count, UINT32*& buffers) = 0; 
protected:
	/**
	 * Checks support for specified extension or attribute.
	 */
	virtual bool checkSupport(Support key) = 0;
public:
	enum NodeType {NONE, MODEL, SPRITE, TEXT, SPRITE_TEXT, ALL};
	enum TextureType {T2D, CUBE_MAP};
public:
	GraphicsManager(ServiceLocator* services);
	virtual ~GraphicsManager();

	/** 
	 * Called to setup object.
	 */
	void create();

	/**
	 * Called to release object resources.
	 */
	void release();

	/**
	 * @return Maximum value for specified key.
	 */
	SIZE getMax(Max key);

	/**
	 * Return true if specified key is supported.
	 */
	bool isSupported(Support key);

	/**
	 * Resizes window.
	 * @param width - new width in pixels.
	 * @param height - new height in pixels.
	 */
	void resize(UINT32 width, UINT32 height);

	/**
	 * Render scene.
	 */
	void render();

	/**
	 * Renders graphic user interface element with text.
	 */
	void renderGuiText(Node* node);

	/**
	 * Optimize render list for best performance.
	 */
	void refreshRenderList();

	/**
	 * Adds vertex to array of vertices, that are rendered once
	 * during in current frame.
	 * @param x - coordinate.
	 * @param y - coordinate.
	 * @param z - coordinate.
	 */
	void vertex(float x, float y, float z = 0.0f);

	/**
	 * Specifies the color, the array of vertices should be,
	 * that are specified after this call.
	 * @param r - red.
	 * @param g - green.
	 * @param b - blue.
	 * @param a - alpha.
	 */
	void color(float r, float g, float b, float a = 1.0f);

	/**
	 * Renders nodes used in scene.
	 * @param type - lets to specify what type of nodes to render.
	 */
	void renderScene(NodeType type);

	/**
	 * Renders verticies specified during this frame.
	 * @param mat - view matrix.
	 */
	void renderVertices(Mat4 mat);

	/**
	 * Render single node.
	 * @param node - pointer to node, to be rendered.
	 * @param mat - 3D projection matrix for models,
	 * and 2D projection matrix for sprites.
	 */
	void renderNode(Node* node, Mat4 mat);
	bool isOutsideCameraView(Node* node);
	void prepareShader(Shader* shader, Node* node, Mat4 in);
	void prepareMatrix(Node* node, Mat4 in, Mat4 out);
	void bindCombinedBufferObject(Renderable* renderable);
	void bindCubeMap();
	void bindTextures(Shader* shader, int textures[]);
	void renderParts(Renderable* renderable, int textures[]);
	void drawViaVertices(Renderable* renderable);
	void drawViaIndices(Renderable* renderable);
	int getRenderType(Renderable* renderable);
	void releaseResources(Renderable* renderable);

	/**
	 * Render quad all over the screen.
	 * @param shader - shader to be used for quad rendering.
	 * @param colorBuffer - color texture of last render pass.
	 * @param depthBuffer - depth texture of last render pass.
	 * @param widthScale - width scale of last render frame.
	 * @param heightScale - height scale of last render frame.
	 */
	void renderQuad(Shader* shader, UINT32 colorBuffer, UINT32 depthBuffer,
		float widthScale, float heightScale);

	/**
	 * @param type - winding order.
	 */
	void setWindingOrder(int type);

	/**
	 * @param id - texture id.
	 * @param index - index of the active texture.
	 */
	void bindTexture(SIZE id, SIZE index = 0, TextureType type = T2D);

	/**
	 * @param id - shader id.
	 */
	void useProgram(SIZE id);

	/**
	 * @param id - buffer id.
	 * @return True if new buffer was bound.
	 */
	bool bindBuffer(SIZE id);

protected:
	ServiceLocator* services_;
	Database* database_;
	Camera* camera_;
	ResourceManager* resourceManager_;
	TextManager* textManager_;
	vector<Node*> modelArray_;
	vector<Node*> spriteArray_;
	vector<Node*> guiArray_;
	vector<Node*> lights_;
	Color vertexColor_;
	vector<float> vertices_;
	vector<float> colors_;
	Mat4 screenMatrix_;
	Mat4 viewMatrix_;
	Renderable* screenPlane_;
	Shader* immediateShader_;
	Shader* textShader_;
	vector<RenderPass*> passes_;
	FrameBuffer* frontBuffer_;
	FrameBuffer* backBuffer_;
	UINT32 planeVBO_;
	UINT32 planeUVBO_;
	// Variables below are used to reduce redundant state changes.
	int windingType_;
	vector<SIZE> boundTextures_;
	SIZE activeTexture_;
	SIZE shaderId_;
	SIZE bufferId_;
	Mat4 matPos, matScale, matPosScale, matProjPos, matProjPosScale;
	SIZE maxValues_[MAX_COUNT];
	INT8 supportValues_[SUPPORT_COUNT];
	INT64 startTime_;
};

#endif
