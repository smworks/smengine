﻿/*
 * GraphicsManager.cpp
 *
 *  Created on: 2014.01.16
 *      Author: Martynas Šustavičius
 */

#include "GraphicsManager.h"
#include "ServiceLocator.h"
#include "../Settings.h"
#include "../Camera.h"
#include "../Node.h"
#include "../TextureAtlas.h"
#include "../ResourceManager.h"
#include "../Resources/Resource.h"
#include "../Resources/Renderable.h"
#include "../Resources/StaticObject.h"
#include "../Resources/CubeMap.h"
#include "../Resources/TextureRGBA.h"
#include "../Resources/Symbol.h"
#include "../Resources/Shader.h"
#include "../Resources/GUIText.h"
#include "../TextManager.h"
#include "../BoundingVolume.h"
#include "../RenderPass.h"
#include "../FrameBuffer.h"
#include "../Shapes.h"

#define MAX_ACTIVE_TEXTURES 32

#ifdef SMART_DEBUG
	extern UINT64 g_renderModelTime;
	extern UINT64 g_renderSpriteTime;
	extern UINT64 g_renderTextTime;
#endif

GraphicsManager::GraphicsManager(ServiceLocator* services) :
	services_(services),
	frontBuffer_(0),
	backBuffer_(0),
	windingType_(Renderable::WINDING_TYPE_NONE),
	boundTextures_(MAX_ACTIVE_TEXTURES),
	activeTexture_(0),
	shaderId_(0),
	bufferId_(0)
{
	LOGD("Created graphics manager.");
}


GraphicsManager::~GraphicsManager() {
	LOGD("Deleted graphics manager.");
}

void GraphicsManager::create() {
	// Create plane vertex buffer object with positions that will be used to render all 2D content.
	vector<VertexP>* vbo = static_cast<vector<VertexP>*>(
        Shapes::getShape(Shapes::SHAPE_SCREEN_PLANE, Shapes::VERTEX_POS));
	setVertexBuffer(planeVBO_, &(*vbo)[0], (UINT32) vbo->size() * sizeof(VertexP));
	delete vbo;
	// Create plane vertex buffer object with uv coordinates that will be used to render all 2D content.
	vector<VertexT>* uvbo = static_cast<vector<VertexT>*>(
        Shapes::getShape(Shapes::SHAPE_SCREEN_PLANE, Shapes::VERTEX_TEX));
	setVertexBuffer(planeUVBO_, &(*uvbo)[0], (UINT32) uvbo->size() * sizeof(VertexT));
	delete uvbo;
	settings_ = services_->getSettings();
	camera_ = services_->getCamera();
	resourceManager_ = services_->getRM();
	textManager_ = services_->getTextManager();
	#ifdef SMART_DEBUG
        glClearColor(1.0f, 0.5f, 1.0f, 1.0f);
	#else
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	#endif
	// Disable depth buffer testing by default.
	glDisable(GL_DEPTH_TEST);
	// Choose the type of depth testing.
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	// Counter-clockwise winding.
	setWindingOrder(Renderable::WINDING_TYPE_CCW);
	// Enable face culling.
	glEnable(GL_CULL_FACE);
	// Side of faces to remove with culling.
	glCullFace(GL_BACK);
	// Use dithering to produce better quality for 565 color buffer.
	glEnable(GL_DITHER);
	// Specify blending function.
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Initialize loading screen.
	// Load loading screen shader.
	loadingShader_ = NEW Shader(services_);
	loadingShader_->getAttributes().setString(
		Resource::ATTR_FILE, settings_->getString(
			Settings::LOADING_SCREEN_SHADER));
	loadingShader_->create();
	if (loadingShader_ == 0) {
		LOGE("Unable to load default shader.");
	}
	// Load screen plane.
	StaticObject* obj = NEW StaticObject(services_);
	obj->getAttributes().setString(
		Resource::ATTR_FILE, PLANE);
	obj->getAttributes().setString(
		StaticObject::ATTR_TYPE, StaticObject::VAL_PLANE);
	obj->create();
	screenPlane_ = obj;
	// Load loading screen texture.
	loadingTexture_ = NEW TextureRGBA(services_);
	loadingTexture_->getAttributes().setString(
		Resource::ATTR_FILE, LOADING_BACKGROUND);
	loadingTexture_->create();
	CHECK_GL_ERROR("Error initialising graphics manager.");
	// Load immediate mode shader.
	immediateShader_ = NEW Shader(services_);
	string& immediateShaderName = settings_->getString(
		Settings::IMMEDIATE_MODE_SHADER);
	immediateShader_->getAttributes().setString(
		Resource::ATTR_FILE, immediateShaderName);
	immediateShader_->create();
	resourceManager_->add(immediateShaderName, immediateShader_);
	// Load text shader.
	textShader_ = NEW Shader(services_);
	string& textShaderName = settings_->getString(
		Settings::DEFAULT_TEXT_SHADER);
	textShader_->getAttributes().setString(
		Resource::ATTR_FILE, textShaderName);
	textShader_->create();
	resourceManager_->add(textShaderName, textShader_);

	RenderPass* rp = NEW RenderPass(services_);
	rp->setRenderToScreen(true);
	rp->setRenderContent(ALL);
	rp->setFrameScale(1.0f, 1.0f);
	passes_.push_back(rp);

	//rp = NEW RenderPass(services_);
	//rp->setRenderToScreen(false);
 //   Shader* shader = NEW Shader(services_);
 //   shader->getAttributes().setString(Resource::ATTR_FILE, "horizontal_blur");
 //   shader->create();
 //   resourceManager_->add("horizontal_blur", shader);
	//rp->setFrameScale(1.0f, 1.0f);
	//rp->setShader(shader);
	//passes_.push_back(rp);

	//rp = NEW RenderPass(services_);
	//rp->setRenderToScreen(true);
	//shader = NEW Shader(services_);
 //   shader->getAttributes().setString(Resource::ATTR_FILE, "vertical_blur");
 //   shader->create();
 //   resourceManager_->add("vertical_blur", shader);
	//rp->setFrameScale(1.0f, 1.0f);
	//rp->setShader(shader);
	//passes_.push_back(rp);
}

void GraphicsManager::release() {
	unsetVertexBuffer(planeVBO_);
	unsetVertexBuffer(planeUVBO_);
	delete loadingTexture_;
	delete screenPlane_;
	delete loadingShader_;
	vector<RenderPass*>::iterator it = passes_.begin();
	while (it != passes_.end()) {
		delete (*it);
		it++;
	}
	if (frontBuffer_ != 0) {
		delete frontBuffer_;
	}
	if (backBuffer_ != 0) {
		delete backBuffer_;
	}
}

void GraphicsManager::resize(UINT32 width, UINT32 height) {
	if (width == 0 || height == 0) {
		return;
	}
	// Update screen matrix.
	Mat4 pos, rot, posRot, scale, posScale;
	Matrix::translate(pos,
		(float) services_->getScreenWidth() * 0.5f,
		(float) services_->getScreenHeight() * 0.5f,
		-1.0f);
	Matrix::rotateX(rot, -90.0f);
	Matrix::scale(scale,
		(float) services_->getScreenWidth(),
		1.0f,
		(float) services_->getScreenHeight());
	Matrix::multiply(pos, rot, posRot);
	Matrix::multiply(posRot, scale, posScale);
	Mat4 proj;
	Matrix::projection2D(proj,
		(float) services_->getScreenWidth(),
		(float) services_->getScreenHeight());
	Matrix::multiply(
		proj,
		posScale,
		screenMatrix_);
	// Update frame buffers.
	if (frontBuffer_ != 0) {
		delete frontBuffer_;
	}
	frontBuffer_ = NEW FrameBuffer(this, width, height);
	if (backBuffer_ != 0) {
		delete backBuffer_;
	}
	backBuffer_ = NEW FrameBuffer(this, width, height);
}

void GraphicsManager::renderLoading() {
	services_->updateTimer(1000.0f / 30.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int shaderId = loadingShader_->getId();
	useProgram(shaderId);
	int hTexture = glGetUniformLocation(shaderId,
			SHADER_MAIN_TEXTURE);
	if (!loadingShader_->hasHandle(Shader::WVP)
		||!loadingShader_->hasHandle(Shader::POS)
		|| !loadingShader_->hasHandle(Shader::UV)) {
		LOGE("Loading screen shader does not "
			"contain required variables.");
		return;
	}
	loadingShader_->setMatrix4(Shader::WVP, screenMatrix_);
	loadingShader_->setFloat(Shader::TIMER, getMicroseconds() / 1000000.0f);
	// Screen width.
	loadingShader_->setFloat(Shader::SCREEN_WIDTH,
		(float) services_->getScreenWidth());
	// Screen height.
	loadingShader_->setFloat(Shader::SCREEN_HEIGHT,
		(float) services_->getScreenHeight());
	bindTexture(loadingTexture_->getId());
	glUniform1i(hTexture, 0);
	SIZE stride = screenPlane_->getVertexStride();
	bindBuffer(screenPlane_->getCBO());
	glEnableVertexAttribArray(loadingShader_->getHandle(Shader::POS));
	glVertexAttribPointer(
		loadingShader_->getHandle(Shader::POS), 3, GL_FLOAT, GL_FALSE,
		stride, ((char*) 0) + screenPlane_->getPosOffset());
	glEnableVertexAttribArray(loadingShader_->getHandle(Shader::UV));
	glVertexAttribPointer(
		loadingShader_->getHandle(Shader::UV), 2, GL_FLOAT, GL_FALSE,
		stride, ((char*) 0) + screenPlane_->getUVOffset());
	glDrawArrays(GL_TRIANGLES, 0, (GLint) screenPlane_->getVertexCount());
	glDisableVertexAttribArray(loadingShader_->getHandle(Shader::UV));
	glDisableVertexAttribArray(loadingShader_->getHandle(Shader::POS));
	CHECK_GL_ERROR("Error while rendering loading screen");
}

void GraphicsManager::render() {
	refreshRenderList();
	Matrix::multiply(camera_->getProjection3D(), camera_->getMatrix(), viewMatrix_);
	camera_->extractPlanes(viewMatrix_);
	// Render passes.
	static vector<RenderPass*>::const_iterator passIt;
	passIt = passes_.begin();
	FrameBuffer* a = frontBuffer_;
	FrameBuffer* b = backBuffer_;
	FrameBuffer* tmp = 0;
	while (passIt != passes_.end()) {
	    if ((*passIt)->isEnabled()) {
            (*passIt)->render(a, b);
            tmp = a;
            a = b;
            b = tmp;
	    }
		passIt++;
	}
	// Render wireframe.
	renderVertices(viewMatrix_);
}

void GraphicsManager::renderSprite(Renderable* sprite, float x, float y, float width, float height) {
	Shader* shader = sprite->getShader();
	useProgram(shader->getId());
	shader->setVector4(Shader::AMBIENT, sprite->getAmbient().toArray());
	glEnableVertexAttribArray(shader->getHandle(Shader::POS));
	bindBuffer(planeVBO_);
	glVertexAttribPointer(shader->getHandle(Shader::POS), 3, GL_FLOAT, GL_FALSE,
		sizeof(VertexP), (char*) 0);
	if (shader->hasHandle(Shader::UV)) {
		glEnableVertexAttribArray(shader->getHandle(Shader::UV));
		bindBuffer(planeUVBO_);
		glVertexAttribPointer(shader->getHandle(Shader::UV), 2, GL_FLOAT, GL_FALSE,
			sizeof(VertexT), (char*) 0);
	}
	Matrix::translate(matPos, x, y, 1.0f);
	Matrix::scale(matScale, width, height, 1.0f);
	Matrix::multiply(camera_->getProjection2D(), matPos, matProjPos);
	Matrix::multiply(matProjPos, matScale, matProjPosScale);
	shader->setMatrix4(Shader::WVP, matProjPosScale);
	glDrawArrays(GL_TRIANGLES, 0, 18 / 3);
	glDisableVertexAttribArray(shader->getHandle(Shader::POS));
	if (shader->hasHandle(Shader::UV)) {
		glDisableVertexAttribArray(shader->getHandle(Shader::UV));
	}
	CHECK_GL_ERROR("Rendering sprite");
}

void GraphicsManager::renderGUI(Node* node) {
	GUIText* text = 0;
	if (node->hasResource(Resource::GUI_TEXT)) {
		text = dynamic_cast<GUIText*>(
			node->getResource(Resource::GUI_TEXT));
	}
	else if (node->hasResource(Resource::GUI_BUTTON)) {
		text = dynamic_cast<GUIText*>(
			node->getResource(Resource::GUI_BUTTON));
	}
	else if (node->hasResource(Resource::GUI_INPUT)) {
		text = dynamic_cast<GUIText*>(
			node->getResource(Resource::GUI_INPUT));
	}
	if (text == 0) {
		return;
	}
	GUIText::SymbolData* symbols = text->getSymbolArray();
	SIZE symbolCount = text->getSymbolCount();
	renderSprite(text, 0.0f, (float) services_->getScreenHeight() - text->getHeight(),
		(float) text->getWidth(), (float) text->getHeight());
	useProgram(textShader_->getId());
	int texture = glGetUniformLocation(textShader_->getId(), "texture_0");
	bindTexture(services_->getTextureAtlas()->getId(Texture::MONO));
	glUniform1i(texture, 0);
	glEnableVertexAttribArray(textShader_->getHandle(Shader::POS));
	glEnableVertexAttribArray(textShader_->getHandle(Shader::UV));
	bindBuffer(planeVBO_);
	glVertexAttribPointer(textShader_->getHandle(Shader::POS), 3, GL_FLOAT, GL_FALSE,
		sizeof(VertexP), ((char*) 0));
	textShader_->setVector4(Shader::FOREGROUND, text->getDiffuse().toArray());
	for (SIZE i = 0; i < symbolCount; i++) {
		GUIText::SymbolData& sd = symbols[i];
		if (sd.posX + sd.symbol->getWidth() < 0 || sd.posX > services_->getScreenWidth()) {
			continue;
		}
		textShader_->setMatrix4(Shader::WVP, sd.matProjPosScale);
		if (bindBuffer(sd.symbol->getTexture()->getCBO())) {
			glVertexAttribPointer(textShader_->getHandle(Shader::UV), 2, GL_FLOAT, GL_FALSE,
				sizeof(VertexPT), ((char*) 0) + offsetof(VertexPT, uv));
		}
		glDrawArrays(GL_TRIANGLES, 0, 18 / 3);
	}
	glDisableVertexAttribArray(textShader_->getHandle(Shader::POS));
	glDisableVertexAttribArray(textShader_->getHandle(Shader::UV));
	CHECK_GL_ERROR("Problem with text renderer");
}

void GraphicsManager::refreshRenderList() {
	vector<Node*> renderArray;
	services_->getRootNode()->toRenderArray(renderArray);
	spriteArray_.clear();
	modelArray_.clear();
	textArray_.clear();
	lights_.clear();
	static vector<Node*>::const_iterator it;
	it = renderArray.begin();
	while (it != renderArray.end()) {
		if ((*it)->hasResource(Resource::SPRITE)) {
			spriteArray_.push_back((*it));
		}
		if ((*it)->hasResource(Resource::STATIC_OBJECT)
			|| (*it)->hasResource(Resource::DYNAMIC_OBJECT)) {
			modelArray_.push_back((*it));
		}
		if ((*it)->hasResource(Resource::GUI_TEXT)
			|| (*it)->hasResource(Resource::GUI_CONTAINER)
			|| (*it)->hasResource(Resource::GUI_IMAGE)
			|| (*it)->hasResource(Resource::GUI_BUTTON)
            || (*it)->hasResource(Resource::GUI_INPUT))
		{
			textArray_.push_back((*it));
		}
		if ((*it)->hasResource(Resource::LIGHT)) {
			lights_.push_back((*it));
		}
		it++;
	}
}

void GraphicsManager::vertex(float x, float y, float z) {
	vertices_.push_back(x);
	vertices_.push_back(y);
	vertices_.push_back(z);
	colors_.push_back(vertexColor_.getR());
	colors_.push_back(vertexColor_.getG());
	colors_.push_back(vertexColor_.getB());
	colors_.push_back(vertexColor_.getA());
}

void GraphicsManager::color(float r, float g, float b, float a) {
	vertexColor_.setRGBA(r, g, b, a);
}

void GraphicsManager::renderScene(NodeType type) {
	static vector<Node*>::const_iterator it;
	if (type == ALL || type == MODEL) {
#ifdef SMART_DEBUG
	UINT64 start = getMicroseconds();
#endif
		glEnable(GL_DEPTH_TEST);
		it = modelArray_.begin();
		while (it != modelArray_.end())	{
			renderNode(*it, viewMatrix_, false);
			it++;
		}
		glDisable(GL_DEPTH_TEST);
#ifdef SMART_DEBUG
	g_renderModelTime += (getMicroseconds() - start);
#endif
	}
	if (type == ALL || type == SPRITE || type == SPRITE_TEXT) {
#ifdef SMART_DEBUG
	UINT64 start = getMicroseconds();
#endif
		glEnable(GL_BLEND);
		it = spriteArray_.begin();
		while (it != spriteArray_.end()) {
			renderNode(*it, camera_->getProjection2D(), true);
			it++;
		}
		glDisable(GL_BLEND);
#ifdef SMART_DEBUG
	g_renderModelTime += (getMicroseconds() - start);
#endif
	}
	if (type == ALL || type == TEXT || type == SPRITE_TEXT) {
#ifdef SMART_DEBUG
	UINT64 start = getMicroseconds();
#endif
		glEnable(GL_BLEND);
		it = textArray_.begin();
		while (it != textArray_.end()) {
			if ((*it)->getState(Node::RENDERABLE)) {
				renderGUI(*it);
			}
			it++;
		}
		glDisable(GL_BLEND);
#ifdef SMART_DEBUG
	g_renderTextTime += (getMicroseconds() - start);
#endif
	}
}

void GraphicsManager::renderVertices(Mat4 mat) {
	if (vertices_.size() == 0) {
		return;
	}
	useProgram(immediateShader_->getId());
	immediateShader_->setMatrix4(Shader::WVP, mat);
    bindBuffer(0);
	glEnableVertexAttribArray(immediateShader_->getHandle(Shader::POS));
	glVertexAttribPointer(immediateShader_->getHandle(Shader::POS),
		3, GL_FLOAT, GL_FALSE, 0, &vertices_[0]);
	glEnableVertexAttribArray(immediateShader_->getHandle(Shader::COL));
	glVertexAttribPointer(immediateShader_->getHandle(Shader::COL),
		4, GL_FLOAT, GL_FALSE, 0, &colors_[0]);
    glDrawArrays(GL_LINES, 0, static_cast<GLint>(vertices_.size() / 3));
    glDisableVertexAttribArray(immediateShader_->getHandle(Shader::COL));
	glDisableVertexAttribArray(immediateShader_->getHandle(Shader::POS));
	vertices_.clear();
	colors_.clear();
    CHECK_GL_ERROR("Rendering vertices");
}

void GraphicsManager::renderNode(
	Node* node, Mat4 mat, bool ortho)
{
	if (!node->getState(Node::RENDERABLE)) {
		return;
	}
	// Retrieve model data.
	Renderable* renderable = 0;
	Resource* resource = 0;
	if (node->hasResource(Resource::SPRITE)) {
		resource = node->getResource(Resource::SPRITE);
		renderable = dynamic_cast<Renderable*>(resource);
	}
	else if (node->hasResource(Resource::GUI_CONTAINER)) {
		resource = node->getResource(Resource::GUI_CONTAINER);
		renderable = dynamic_cast<Renderable*>(resource);
	}
	else if (node->hasResource(Resource::GUI_IMAGE)) {
		resource = node->getResource(Resource::GUI_IMAGE);
		renderable = dynamic_cast<Renderable*>(resource);
	}
	else if (node->hasResource(Resource::GUI_TEXT)) {
		resource = node->getResource(Resource::GUI_TEXT);
		renderable = dynamic_cast<Renderable*>(resource);
	}
	else if (node->hasResource(Resource::GUI_BUTTON)) {
		resource = node->getResource(Resource::GUI_BUTTON);
		renderable = dynamic_cast<Renderable*>(resource);
	}
	else if (node->hasResource(Resource::GUI_INPUT)) {
		resource = node->getResource(Resource::GUI_INPUT);
		renderable = dynamic_cast<Renderable*>(resource);
	}
	else if (node->hasResource(Resource::STATIC_OBJECT)) {
		resource = node->getResource(Resource::STATIC_OBJECT);
		renderable = dynamic_cast<Renderable*>(resource);
		StaticObject* so = static_cast<StaticObject*>(resource);
		// Check for frustum culling.
		Vec3 posAbs;
		node->getPosAbs(posAbs);
		if (so->getBoundingVolume() != 0
			&& so->getBoundingVolume()->isInFrustum(
			camera_, posAbs, node->getScale())
			== BoundingVolume::OUTSIDE)
		{
			return;
		}
	}
	else if (node->hasResource(Resource::DYNAMIC_OBJECT)) {
		resource = node->getResource(Resource::DYNAMIC_OBJECT);
		renderable = dynamic_cast<Renderable*>(resource);
	}

	if (renderable == 0) {
		LOGW("Renderable with name \"%s\" is null.",
			node->getName().c_str());
		return;
	}
	Shader* shader = 0;
	if (node->hasResource(Resource::SHADER)) { // TODO: leave only renderable->getShader()
		shader = static_cast<Shader*>(node->getResource(Resource::SHADER));
	}
	else {
		shader = renderable->getShader();
	}
	if (shader == 0) {
		LOGW("Unable to retrieve shader for node: %s.",
			node->getName().c_str());
		return;
	}
	if (!shader->isValid()) {
		LOGW("Shader \"%s\" is not valid.", shader->getName().c_str());
	}
	// Select shader program to use.
	useProgram(shader->getId());
	Mat4 res;
	if (!ortho) {
		Matrix::multiply(mat, node->getMatrix(), res);
	}
	else {
		Mat4 tmp;
		Mat4 pos;
		Mat4 rot;
		Mat4 scale;
		Vec3 p;
		node->getPosAbs(p);
		Quaternion& q = node->getRot();
		Vec3 r(q.getX(), q.getY(), q.getZ());
		Vec3 s = node->getScale();
		float width = s.getX();
		float height = s.getY();
		float x = p.getX();
		float y = p.getY();
		Matrix::translate(pos, x, y, p.getZ());
		Matrix::rotateXYZ(rot, r.getX(), r.getY(), r.getZ());
		Matrix::scale(scale, width, height, 1.0f);
		Matrix::multiply(mat, pos, res);
		Matrix::multiply(res, rot, tmp);
		Matrix::multiply(tmp, scale, res);
	}
	// World * View * Projection matrix.
	shader->setMatrix4(Shader::WVP, res);
	// World matrix.
	shader->setMatrix4(Shader::W, node->getMatrix());
	// Normal matrix.
	if (shader->hasHandle(Shader::N)) {
		Mat3 normalMatrix;
		Matrix::toMat3(node->getMatrix(), normalMatrix);
		shader->setMatrix3(Shader::N, normalMatrix);
	}
	// Light position.
	if (lights_.size() > 0 && shader->hasHandle(Shader::LIGHT_POS)) {
        float* lightPos = NEW float[lights_.size() * 3];
		for (UINT32 i = 0; i < lights_.size(); i++) {
			Vec3& pos = lights_[i]->getPos();
			lightPos[i * 3 + 0] = pos.getX();
			lightPos[i * 3 + 1] = pos.getY();
			lightPos[i * 3 + 2] = pos.getZ();
		}
		shader->setVector3(Shader::LIGHT_POS, lightPos, static_cast<UINT32>(lights_.size()));
        delete lightPos;
	}
	// Light count.
	shader->setInt(Shader::LIGHT_COUNT, static_cast<UINT32>(lights_.size()));
	//shader->setVector3(Shader::LIGHT_POS,
	//	services_->getEnv()->getSunPos()->toArray());
	// Eye position.
	shader->setVector3(Shader::EYE_POS,
		camera_->getPos().toArray());
	// Fog color.
	//if (shader->hasHandle(Shader::FOG_COLOR)) {
	//	shader->setVector3(Shader::FOG_COLOR,
	//		services_->getEnv()->getFogColor());
	//}
	// Fog density.
	//shader->setFloat(Shader::FOG_DENSITY,
	//	services_->getEnv()->getFogDensity());
	// Timer.
	shader->setFloat(Shader::TIMER, getMicroseconds() / 1000000.0f);
	// Bind combined buffer object.
	if (renderable->getCBO() > 0) {
		SIZE stride = renderable->getVertexStride();
		bindBuffer(renderable->getCBO());
		if (shader->hasHandle(Shader::POS)) {
			glEnableVertexAttribArray(shader->getHandle(Shader::POS));
			glVertexAttribPointer(
				shader->getHandle(Shader::POS), 3, GL_FLOAT, GL_FALSE,
				stride, ((char*) 0) + renderable->getPosOffset());
		}
		if (renderable->getNormalOffset() != -1
			&& shader->hasHandle(Shader::NORMAL)) {
			glEnableVertexAttribArray(shader->getHandle(Shader::NORMAL));
			glVertexAttribPointer(
				shader->getHandle(Shader::NORMAL), 3, GL_FLOAT, GL_FALSE,
				stride, ((char*) 0) + renderable->getNormalOffset());
		}
		if (renderable->getUVOffset() != -1 && shader->hasHandle(Shader::UV)) {
			glEnableVertexAttribArray(shader->getHandle(Shader::UV));
			glVertexAttribPointer(
				shader->getHandle(Shader::UV), 2, GL_FLOAT, GL_FALSE,
				stride, ((char*) 0) + renderable->getUVOffset());
		}
	}
	else {
		return;
	}
	// Bind cube map.
	if (node->hasResource(Resource::CUBE_MAP)
		&& shader->hasHandle(Shader::CUBE_MAP)) {
		CubeMap* t = static_cast<CubeMap*>(
			node->getResource(Resource::CUBE_MAP));
		bindTexture(t->getId(), 0, CUBE_MAP);
		glUniform1i(shader->getHandle(Shader::CUBE_MAP), 0);
	}
	int hTextures[8];
	hTextures[0] = glGetUniformLocation(shader->getId(),
		SHADER_MAIN_TEXTURE);
	// Bind the texture.
	vector<Resource*> textures = node->getResources(Resource::TEXTURE_2D);
	UINT32 size = textures.size() < 8 ? static_cast<UINT32>(textures.size()) : 7;
	UINT32 texture = 0;
	for (UINT32 i = 0; i < size; i++) {
		texture = i + 1;
		const string& name = textures[i]->getName();
		TextureRGBA* tex = static_cast<TextureRGBA*>(textures[i]);
		SIZE offset = node->getName().length() + 1;
		string textName = name.substr(offset, name.length() - 4 - offset);
		hTextures[texture] = glGetUniformLocation(shader->getId(),
			textName.c_str());
		if (hTextures[texture] == -1) {
			LOGW("Handle to texture \"%s\" not found in \"%s\" shader.",
				textName.c_str(), shader->getName().c_str());
			continue;
		}
		bindTexture(tex->getId(), i + 1);
		glUniform1i(hTextures[texture], texture);
	}
	int renderType;
	switch (renderable->getRenderType()) {
		case Renderable::RENDER_TYPE_POINTS:
			renderType = GL_POINTS;
			//glPointSize(renderable->getPointSize());
		break;
		case Renderable::RENDER_TYPE_LINES:
			renderType = GL_LINES;
			glLineWidth(renderable->getLineWidth());
		break;
		case Renderable::RENDER_TYPE_TRIANGLE_FAN:
			renderType = GL_TRIANGLE_FAN;
		break;
		case Renderable::RENDER_TYPE_TRIANGLE_STRIP:
			renderType = GL_TRIANGLE_STRIP;
		break;
		case Renderable::RENDER_TYPE_QUADS:
			renderType = GL_TRIANGLES;
			// TODO: renderType = GL_QUADS;
		break;
		default:
			renderType = GL_TRIANGLES;
		break;
	}
	setWindingOrder(renderable->getWindingType());
	if (renderable->getCullFace()) {
		glEnable(GL_CULL_FACE);
	}
	else {
		glDisable(GL_CULL_FACE);
	}
	if (renderable->getBlending()) {
		glEnable(GL_BLEND);
	}
	SIZE renderCount = renderable->getRenderCount();
	SIZE lastTexture = 0;
	for (SIZE i = 0; i < renderCount; i++) {
		renderable->setRenderable(i);
		// Check if this part contains bounding volume.
		//if (renderable->getBV() != 0) {
		//	// Check for frustum culling.
		//	if (renderable->getBV()->isInFrustum(
		//		camera_, renderable->getBV()->getPos(), node->getScale())
		//		== BoundingVolume::OUTSIDE)
		//	{
		//		continue;
		//	}
		//}
		// Ambient material color.
		if (shader->hasHandle(Shader::AMBIENT)) {
			shader->setVector3(Shader::AMBIENT,
				renderable->getAmbient().toArray());
		}
		// Diffuse material color.
		if (shader->hasHandle(Shader::DIFFUSE)) {
			shader->setVector3(Shader::DIFFUSE,
				renderable->getDiffuse().toArray());
		}
		// Specular material color.
		if (shader->hasHandle(Shader::SPECULAR)) {
			shader->setVector3(Shader::SPECULAR,
				renderable->getSpecular().toArray());
		}
		// Specular material color intensity.
		shader->setFloat(Shader::SPECULARITY, renderable->getSpecularity());
		// Model transparency.
		shader->setFloat(Shader::TRANSPARENCY, renderable->getTransparency());
		// Bind main texture.
		renderable->getAmbient();
		if (renderable->getTexture() != lastTexture
			&& hTextures[0] != -1)
        {
			lastTexture = renderable->getTexture();
			if (shader->hasHandle(Shader::MAIN_TEXTURE)) {
				if (lastTexture == 0) {
					shader->setFloat(Shader::MAIN_TEXTURE, 0.0f);
				}
				else {
					shader->setFloat(Shader::MAIN_TEXTURE, 1.0f);
				}
			}
			bindTexture(renderable->getTexture());
			glUniform1i(hTextures[0], 0);
		}
		else if (lastTexture == 0) {
			shader->setFloat(Shader::MAIN_TEXTURE, 0.0f);
		}
		if (renderable->getIBO() > 0) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
				renderable->getIBO());
			if (renderable->getIndexType() == Renderable::INDEX_TYPE_USHORT) {
				glDrawElements(renderType,
					(GLint) renderable->getIndexCount(),
					GL_UNSIGNED_SHORT, ((char*) 0) + renderable->getIndexOffset() * sizeof(short));
				CHECK_GL_ERROR("glDrawElements with short indices");
			}
			else {
				glDrawElements(renderType,
					(GLint) renderable->getIndexCount(),
					GL_UNSIGNED_INT, ((char*) 0) + renderable->getIndexOffset() * sizeof(int));
				CHECK_GL_ERROR("glDrawElements with int indices");
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		else {
			glDrawArrays(renderType, 0, (GLint) renderable->getVertexCount());
			CHECK_GL_ERROR("glDrawArrays");
		}
	}
	if (renderable->getBlending()) {
		glDisable(GL_BLEND);
	}
	if (renderable->getUVOffset() != -1 && shader->hasHandle(Shader::UV)) {
		glDisableVertexAttribArray(shader->getHandle(Shader::UV));
	}
	if (renderable->getNormalOffset() != -1
		&& shader->hasHandle(Shader::NORMAL)) {
		glDisableVertexAttribArray(shader->getHandle(Shader::NORMAL));
	}
	glDisableVertexAttribArray(shader->getHandle(Shader::POS));
}

void GraphicsManager::renderQuad(
	Shader* shader, UINT32 colorBuffer, UINT32 depthBuffer, float widthScale, float heightScale)
{
	if (shader == 0) {
		LOGE("Shader for quad not found!");
		return;
	}
	int shaderId = shader->getId();
	useProgram(shaderId);
	if (!shader->hasHandle(Shader::WVP)
		||!shader->hasHandle(Shader::POS)
		|| !shader->hasHandle(Shader::UV)) {
		LOGE("Shader does not "
			"contain required variables.");
		return;
	}
	// World * View * Projection matrix.
	Mat4 res, pos, rot, posRot, scale, posScale;
	Matrix::translate(pos,
		(float) services_->getScreenWidth() * 0.5f / widthScale,
		(float) services_->getScreenHeight() * 0.5f / heightScale,
		-1.0f);
	Matrix::rotateX(rot, -90.0f);
	Matrix::scale(scale,
		(float) services_->getScreenWidth() / widthScale,
		1.0f,
		(float) services_->getScreenHeight() / heightScale);
	Matrix::multiply(pos, rot, posRot);
	Matrix::multiply(posRot, scale, posScale);
	Mat4 proj;
	Matrix::projection2D(proj,
		(float) services_->getScreenWidth(),
		(float) services_->getScreenHeight());
	Matrix::multiply(
		proj,
		posScale,
		res);
	shader->setMatrix4(Shader::WVP, res);
	// Timer.
	shader->setFloat(Shader::TIMER, getMicroseconds() / 1000000.0f);
	// Screen width.
	shader->setFloat(Shader::SCREEN_WIDTH,
		(float) services_->getScreenWidth());
	// Screen height.
	shader->setFloat(Shader::SCREEN_HEIGHT,
		(float) services_->getScreenHeight());
	bindTexture(colorBuffer);
	glUniform1i(shader->getHandle(Shader::COLOR_BUFFER), 0);
	if (shader->hasHandle(Shader::DEPTH_BUFFER)) {
		bindTexture(depthBuffer, 1);
		glUniform1i(shader->getHandle(Shader::DEPTH_BUFFER), 1);
	}
	SIZE stride = screenPlane_->getVertexStride();
	bindBuffer(screenPlane_->getCBO());
	glEnableVertexAttribArray(shader->getHandle(Shader::POS));
	glVertexAttribPointer(
		shader->getHandle(Shader::POS), 3, GL_FLOAT, GL_FALSE,
		stride, ((char*) 0) + screenPlane_->getPosOffset());
	glEnableVertexAttribArray(shader->getHandle(Shader::UV));
	glVertexAttribPointer(
		shader->getHandle(Shader::UV), 2, GL_FLOAT, GL_FALSE,
		stride, ((char*) 0) + screenPlane_->getUVOffset());
	glDrawArrays(GL_TRIANGLES, 0, (GLint) screenPlane_->getVertexCount());
	glDisableVertexAttribArray(shader->getHandle(Shader::UV));
	glDisableVertexAttribArray(shader->getHandle(Shader::POS));
	// Unbind the textures.
	for (UINT32 i = 0; i < 2; i++) {
		bindTexture(0, i);
	}
	// Disable vertex arrays.
	glDisableVertexAttribArray(shader->getHandle(Shader::UV));
	glDisableVertexAttribArray(shader->getHandle(Shader::POS));
	CHECK_GL_ERROR("Error rendering post processing.");
}

void GraphicsManager::setWindingOrder(int type) {
	if (windingType_ != type) {
		windingType_ = type;
		if (type == Renderable::WINDING_TYPE_CCW) {
			glFrontFace(GL_CCW);
		}
		else {
			glFrontFace(GL_CW);
		}
	}
}

void GraphicsManager::bindTexture(SIZE id, SIZE index, TextureType type) {
	if (index > MAX_ACTIVE_TEXTURES) {
		LOGW("Trying to use more active textures than possible.");
		return;
	}
	if (index != activeTexture_) {
		glActiveTexture(GL_TEXTURE0 + index);
		activeTexture_ = index;
	}
	if (boundTextures_[index] != id) {
		boundTextures_[index] = id;
		if (type == T2D) {
			glBindTexture(GL_TEXTURE_2D, (GLint) id);
		}
		else {
			glBindTexture(GL_TEXTURE_CUBE_MAP, (GLint) id);
		}
	}
}

void GraphicsManager::useProgram(SIZE id) {
	if (id == shaderId_) {
		return;
	}
	shaderId_ = id;
	glUseProgram(id);
}

bool GraphicsManager::bindBuffer(SIZE id) {
	if (bufferId_ == id) {
		return false;
	}
	bufferId_ = id;
	glBindBuffer(GL_ARRAY_BUFFER, id);
	return true;
}