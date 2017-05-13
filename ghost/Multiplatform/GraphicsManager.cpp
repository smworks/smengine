/*
 * GraphicsManager.cpp
 *
 *  Created on: 2014.01.16
 *      Author: Martynas Šustavičius
 */

#include "GraphicsManager.h"
#ifdef ENABLE_GRAPHICS

#include "ServiceLocator.h"
#include "../Multiplatform/Database.h"
#include "../Camera.h"
#include "../Node.h"
#include "../TextureAtlas.h"
#include "../ResourceManager.h"
#include "../Resources/Resource.h"
#include "../Resources/Renderable.h"
#include "../Resources/Model.h"
#include "../Resources/TextureRGBA.h"
#include "../Resources/Shader.h"
#include "../Resources/GUIText.h"
#include "../TextManager.h"
#include "../BoundingVolume.h"
#include "../RenderPass.h"
#include "../FrameBuffer.h"
#include "../Shapes.h"
#include "../ModelData.h"

#define MAX_ACTIVE_TEXTURES 32

GraphicsManager::GraphicsManager(ServiceLocator* services) :
	services(services),
	frontBuffer_(0),
	backBuffer_(0),
	windingType_(Renderable::WINDING_TYPE_NONE),
	boundTextures_(MAX_ACTIVE_TEXTURES),
	activeTexture_(0),
	shaderId_(0),
	bufferId_(0),
	planeVBO_(0),
	planeUVBO_(0),
	startTime(0),
	database_(0)
{
	fill(maxValues_, maxValues_ + MAX_COUNT, 0);
	fill(supportValues_, supportValues_ + SUPPORT_COUNT, 2);
	LOGD("Created graphics manager.");
}

GraphicsManager::~GraphicsManager()
{
	LOGD("Deleted graphics manager.");
}

void GraphicsManager::create()
{
	if (!services->isGuiAvailable())
	{
		LOGD("Graphics manager not properly created.");
		return;
	}
	LOGD("Max texture units: %lu (%lu combined).",
		getMax(MAX_TEXTURE_UNITS), getMax(MAX_COMBINED_TEXTURE_UNITS));
	LOGD("Max texture width and height: %upx (%upx for cube map).",
		getMax(MAX_TEXTURE_SIZE), getMax(MAX_CUBE_MAP_SIZE));
	LOGD("Max render buffer width and height: %upx.", getMax(MAX_RENDER_BUFFER_SIZE));
	LOGD("Max viewport dimensions: %ux%upx.", getMax(MAX_VIEWPORT_WIDTH), getMax(MAX_VIEWPORT_HEIGHT));
	LOGD("NPOT support: %s.", isSupported(SUPPORT_NPOT_TEXTURES) ? "true" : "false");
	LOGD("UINT index support: %s.", isSupported(SUPPORT_UINT_INDEX) ? "true" : "false");
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
	database_ = services->getDB();
	camera = services->getCamera();
	resourceManager_ = services->getRM();
	textManager_ = services->getTextManager();
#ifdef SMART_DEBUG
	glClearColor(1.0f, 0.5f, 1.0f, 1.0f);
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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
	// Load screen plane.
	Model* obj = NEW Model(services);
	obj->getAttributes().setString(
		Resource::ATTR_FILE, PLANE);
	obj->getAttributes().setString(
		Model::ATTR_TYPE, Model::VAL_PLANE);
	obj->create();
	screenPlane_ = obj;
	// Load immediate mode shader.
	immediateShader = NEW Shader(services);
	string immediateShaderName = database_->getString(
		Database::IMMEDIATE_MODE_SHADER);
	immediateShader->getAttributes().setString(
		Resource::ATTR_FILE, immediateShaderName);
	immediateShader->create();
	resourceManager_->add(immediateShaderName, immediateShader);
	// Load text shader.
	textShader = NEW Shader(services);
	string textShaderName = database_->getString(Database::DEFAULT_TEXT_SHADER);
	textShader->getAttributes().setString(Resource::ATTR_FILE, textShaderName);
	textShader->create();
	resourceManager_->add(textShaderName, textShader);

	RenderPass* rp = NEW RenderPass(services);
	rp->setRenderToScreen(true);
	rp->setRenderContent(ALL);
	rp->setFrameScale(1.0f, 1.0f);
	passes_.push_back(rp);

	//rp = NEW RenderPass(services);
	//rp->setRenderToScreen(false);
	//   Shader* shader = NEW Shader(services);
	//   shader->getAttributes().setString(Resource::ATTR_FILE, "horizontal_blur");
	//   shader->create();
	//   resourceManager_->add("horizontal_blur", shader);
	//rp->setFrameScale(1.0f, 1.0f);
	//rp->setShader(shader);
	//passes_.push_back(rp);

	//rp = NEW RenderPass(services);
	//rp->setRenderToScreen(true);
	//shader = NEW Shader(services);
	//   shader->getAttributes().setString(Resource::ATTR_FILE, "vertical_blur");
	//   shader->create();
	//   resourceManager_->add("vertical_blur", shader);
	//rp->setFrameScale(1.0f, 1.0f);
	//rp->setShader(shader);
	//passes_.push_back(rp);
	startTime = getMicroseconds();
	CHECK_GL_ERROR("Error initialising graphics manager.");
}

void GraphicsManager::release()
{
	if (!services->isGuiAvailable())
	{
		return;
	}
	unsetVertexBuffer(planeVBO_);
	unsetVertexBuffer(planeUVBO_);
	delete screenPlane_;
	for (auto it = passes_.begin(); it != passes_.end(); ++it)
	{
		delete (*it);
	}
	if (frontBuffer_ != 0)
	{
		delete frontBuffer_;
	}
	if (backBuffer_ != 0)
	{
		delete backBuffer_;
	}
}

SIZE GraphicsManager::getMax(Max key)
{
	if (maxValues_[key] != 0)
	{
		return maxValues_[key];
	}
	int val[2];
	switch (key)
	{
	case MAX_TEXTURE_UNITS:
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, val);
		break;
	case MAX_COMBINED_TEXTURE_UNITS:
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, val);
		break;
	case MAX_TEXTURE_SIZE:
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, val);
		break;
	case MAX_CUBE_MAP_SIZE:
		glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, val);
		break;
	case MAX_RENDER_BUFFER_SIZE:
		glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, val);
		break;
	case MAX_VIEWPORT_WIDTH:
		glGetIntegerv(GL_MAX_VIEWPORT_DIMS, val);
		break;
	case MAX_VIEWPORT_HEIGHT:
		glGetIntegerv(GL_MAX_VIEWPORT_DIMS, val);
		val[0] = val[1];
		break;
	case MAX_COUNT:
	default:
		return 0;
	}
	return (maxValues_[key] = val[0]);
}

bool GraphicsManager::isSupported(Support key)
{
	if (supportValues_[key] != 2)
	{
		return supportValues_[key] == 1;
	}
	return (supportValues_[key] = checkSupport(key) ? 1 : 0) == 1;
}

void GraphicsManager::resize(UINT32 width, UINT32 height)
{
	if (width == 0 || height == 0)
	{
		return;
	}
	// Update screen matrix.
	Mat4 pos, rot, posRot, scale, posScale;
	Matrix::translate(pos,
	                  (float) services->getScreenWidth() * 0.5f,
	                  (float) services->getScreenHeight() * 0.5f,
	                  -1.0f);
	Matrix::rotateX(rot, -90.0f);
	Matrix::scale(scale,
	              (float) services->getScreenWidth(),
	              1.0f,
	              (float) services->getScreenHeight());
	Matrix::multiply(pos, rot, posRot);
	Matrix::multiply(posRot, scale, posScale);
	Mat4 proj;
	Matrix::projection2D(proj,
	                     (float) services->getScreenWidth(),
	                     (float) services->getScreenHeight(),
	                     database_->getFloat(Database::FAR_PLANE_DISTANCE));
	Matrix::multiply(
		proj,
		posScale,
		screenMatrix_);
	// Update frame buffers.
	if (frontBuffer_ != 0)
	{
		delete frontBuffer_;
	}
	frontBuffer_ = NEW FrameBuffer(this, width, height);
	if (backBuffer_ != 0)
	{
		delete backBuffer_;
	}
	backBuffer_ = NEW FrameBuffer(this, width, height);
}

void GraphicsManager::render()
{
	refreshRenderList();
	Matrix::multiply(camera->getProjection3D(), camera->getMatrix(), viewMatrix);
	camera->extractPlanes(viewMatrix);
	// Render passes.
	FrameBuffer* a = frontBuffer_;
	FrameBuffer* b = backBuffer_;
	FrameBuffer* tmp = nullptr;
	for (RenderPass* rp : passes_)
	{
		if (rp->isEnabled())
		{
			rp->render(a, b);
			tmp = a;
			a = b;
			b = tmp;
		}
	}
	// Render wireframe.
	renderVertices(viewMatrix);
}

void GraphicsManager::renderGuiText(Node* node)
{
	
	renderNode(node, camera->getProjection2D());
	GUIText* text = dynamic_cast<GUIText*>(node->getResource());
	if (text == nullptr)
	{
		return;
	}
	LOGI("x=%f, y=%f", node->getPos().getX(), node->getPos().getY());
	LOGI("tx=%f, ty=%f", text->getPosX(), text->getPosY() + text->getHeight() * 0.5f);
	useProgram(textShader->getId());
	int texture = glGetUniformLocation(textShader->getId(), "texture_0");
	textShader->setVector4(Shader::FOREGROUND, text->getDiffuse().toArray());
	textShader->setVector4(Shader::BACKGROUND, text->getAmbient().toArray());
	bindTexture(services->getTextureAtlas()->getId(Texture::MONO));
	glUniform1i(texture, 0);
	glEnableVertexAttribArray(textShader->getHandle(Shader::POS));
	glEnableVertexAttribArray(textShader->getHandle(Shader::UV));
	Matrix::translate(matPosScale, text->getPosX(), text->getPosY() + text->getHeight() * 0.5f, 0.0);
	Matrix::multiply(camera->getProjection2D(), matPosScale, matProjPosScale);
	textShader->setMatrix4(Shader::WVP, matProjPosScale);
	bindBuffer(text->getTextVBO());
	glVertexAttribPointer(textShader->getHandle(Shader::POS), 3, GL_FLOAT, GL_FALSE,
	                      sizeof(VertexPT), ((char*) 0));
	glVertexAttribPointer(textShader->getHandle(Shader::UV), 2, GL_FLOAT, GL_FALSE,
	                      sizeof(VertexPT), ((char*) 0) + offsetof(VertexPT, uv));
	glDrawArrays(GL_TRIANGLES, 0, (GLint) text->getTextVertexCount());
	glDisableVertexAttribArray(textShader->getHandle(Shader::POS));
	glDisableVertexAttribArray(textShader->getHandle(Shader::UV));
	CHECK_GL_ERROR("Problem with text renderer");
}

struct zComparator
{
	bool operator()(Node* left, Node* right) const
	{
		return left->getPos().getZ() < right->getPos().getZ();
	}
};

void GraphicsManager::refreshRenderList()
{
	vector<Node*> renderArray;
	services->getRootNode()->toRenderArray(renderArray);
	sprites.clear();
	models.clear();
	guiElements.clear();
	lights.clear();
	for (auto it = renderArray.begin(); it != renderArray.end(); ++it)
	{
		Resource::Type type = (*it)->getResource()->getType();
		if (type == Resource::SPRITE)
		{
			sprites.push_back((*it));
		}
		if (type == Resource::MODEL || type == Resource::DYNAMIC_OBJECT)
		{
			models.push_back((*it));
		}
		if (type == Resource::GUI_SURFACE)
		{
			guiElements.push_back((*it));
		}
		if (type == Resource::LIGHT)
		{
			lights.push_back((*it));
		}
	}
	zComparator cmp;
	sort(sprites.begin(), sprites.end(), cmp);
}

void GraphicsManager::vertex(float x, float y, float z)
{
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);
	colors_.push_back(vertexColor_.getR());
	colors_.push_back(vertexColor_.getG());
	colors_.push_back(vertexColor_.getB());
	colors_.push_back(vertexColor_.getA());
}

void GraphicsManager::color(float r, float g, float b, float a)
{
	vertexColor_.setRGBA(r, g, b, a);
}

void GraphicsManager::renderScene(NodeType type)
{
	if (type == ALL || type == MODEL)
	{
		glEnable(GL_DEPTH_TEST);
		for (Node* node : models)
		{
			renderNode(node, camera->getProjection2D());
		}
		glDisable(GL_DEPTH_TEST);
	}
	if (type == ALL || type == SPRITE || type == SPRITE_TEXT)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		for (Node* node : sprites)
		{
			renderNode(node, camera->getProjection2D());
		}
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
	}
	if (type == ALL || type == TEXT || type == SPRITE_TEXT)
	{
		//glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		for (Node* node : guiElements)
		{
			if (node->getState(Node::RENDERABLE))
			{
				renderGuiText(node);
			}
		}
		glDisable(GL_BLEND);
		//glDisable(GL_DEPTH_TEST);
	}
}

void GraphicsManager::renderVertices(Mat4 mat)
{
	if (vertices.size() == 0)
	{
		return;
	}
	useProgram(immediateShader->getId());
	immediateShader->setMatrix4(Shader::WVP, mat);
	bindBuffer(0);
	glEnableVertexAttribArray(immediateShader->getHandle(Shader::POS));
	glVertexAttribPointer(immediateShader->getHandle(Shader::POS),
	                      3, GL_FLOAT, GL_FALSE, 0, &vertices[0]);
	glEnableVertexAttribArray(immediateShader->getHandle(Shader::COL));
	glVertexAttribPointer(immediateShader->getHandle(Shader::COL),
	                      4, GL_FLOAT, GL_FALSE, 0, &colors_[0]);
	glDrawArrays(GL_LINES, 0, static_cast<GLint>(vertices.size() / 3));
	glDisableVertexAttribArray(immediateShader->getHandle(Shader::COL));
	glDisableVertexAttribArray(immediateShader->getHandle(Shader::POS));
	vertices.clear();
	colors_.clear();
	CHECK_GL_ERROR("Rendering vertices");
}

void GraphicsManager::renderNode(Node* node, Mat4 in)
{
	if (!node->getState(Node::RENDERABLE)) return;
	Renderable* renderable = dynamic_cast<Renderable*>(node->getResource());
	ASSERT(renderable != 0, "Node %s does not contain renderable resource", node->getName().c_str());
	if (isOutsideCameraView(node)) return;
	ASSERT(renderable->getShader() != 0, "Renderable from %s has no shader", node->getName().c_str());
	prepareShader(renderable->getShader(), node, in);
	ASSERT(renderable->getCBO() > 0, "Renderable from %s has no CBO", node->getName().c_str());
	bindCombinedBufferObject(renderable);
	bindCubeMap();
	int hTextures[8];
	bindTextures(renderable->getShader(), hTextures);
	setWindingOrder(renderable->getWindingType());
	renderable->getCullFace() ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
	if (renderable->getBlending()) glEnable(GL_BLEND);
	renderParts(renderable, hTextures);
	releaseResources(renderable);
}

bool GraphicsManager::isOutsideCameraView(Node* node) const
{
	Model* model = dynamic_cast<Model*>(node->getResource());
	if (model != nullptr)
	{ // Check for frustum culling.
		Vec3 posAbs;
		node->getPosAbs(posAbs);
		auto bv = model->getData()->getBoundingVolume();
		if (bv != nullptr && bv->isInFrustum(camera, posAbs, node->getScale()) == BoundingVolume::OUTSIDE)
		{
			return true;
		}
	}
	return false;
}

void GraphicsManager::prepareShader(Shader* shader, Node* node, Mat4 in)
{
	useProgram(shader->getId());
	Mat4 mat;
	prepareMatrix(node, in, mat);
	// World * View * Projection matrix.
	shader->setMatrix4(Shader::WVP, mat);
	// World matrix.
	shader->setMatrix4(Shader::W, node->getMatrix());
	// Normal matrix.
	if (shader->hasHandle(Shader::N))
	{
		Mat3 normalMatrix;
		Matrix::toMat3(node->getMatrix(), normalMatrix);
		shader->setMatrix3(Shader::N, normalMatrix);
	}
	// Light position.
	if (lights.size() > 0 && shader->hasHandle(Shader::LIGHT_POS))
	{
		float* lightPos = NEW float[lights.size() * 3];
		for (UINT32 i = 0; i < lights.size(); i++)
		{
			Vec3& pos = lights[i]->getPos();
			lightPos[i * 3 + 0] = pos.getX();
			lightPos[i * 3 + 1] = pos.getY();
			lightPos[i * 3 + 2] = pos.getZ();
		}
		shader->setVector3(Shader::LIGHT_POS, lightPos, static_cast<UINT32>(lights.size()));
		delete[] lightPos;
	}
	// Light count.
	shader->setInt(Shader::LIGHT_COUNT, static_cast<UINT32>(lights.size()));
	//shader->setVector3(Shader::LIGHT_POS,
	//	services->getEnv()->getSunPos()->toArray());
	// Eye position.
	shader->setVector3(Shader::EYE_POS, camera->getPos().toArray());
	// Fog color.
	//if (shader->hasHandle(Shader::FOG_COLOR)) {
	//	shader->setVector3(Shader::FOG_COLOR,
	//		services->getEnv()->getFogColor());
	//}
	// Fog density.
	//shader->setFloat(Shader::FOG_DENSITY,
	//	services->getEnv()->getFogDensity());
	// Timer.
	shader->setFloat(Shader::TIMER, (getMicroseconds() - startTime) * 0.000001f);
}

void GraphicsManager::prepareMatrix(Node* node, Mat4 in, Mat4 out)
{
	if (node->getResource()->getType() == Resource::GUI_SURFACE)
	{
		GUISurface* surface = dynamic_cast<GUISurface*>(node->getResource());
		Mat4 pos, scale, posScale;
		float posY = surface->getPosY();
		Matrix::translate(pos, surface->getPosX(), posY, 0.0f);
		Matrix::scale(scale, surface->getWidth(), surface->getHeight(), 1.0);
		Matrix::multiply(pos, scale, posScale);
		Matrix::multiply(in, posScale, out);
	}
	else if (node->getResource()->getType() == Resource::SPRITE)
	{
		Matrix::translate(matPos, camera->getPos().getX(),
		                  camera->getPos().getY(), camera->getPos().getZ());
		Matrix::multiply(matPos, node->getMatrix(), matPosScale);
		Matrix::multiply(in, matPosScale, out);
		//Matrix::multiply(matProjPosScale, matPos, res);
	}
	else
	{
		Matrix::multiply(in, node->getMatrix(), out);
	}
}

void GraphicsManager::bindCombinedBufferObject(Renderable* renderable)
{
	Shader* shader = renderable->getShader();
	SIZE stride = renderable->getVertexStride();
	bindBuffer(renderable->getCBO());
	if (shader->hasHandle(Shader::POS))
	{
		glEnableVertexAttribArray(shader->getHandle(Shader::POS));
		glVertexAttribPointer(
			shader->getHandle(Shader::POS), 3, GL_FLOAT, GL_FALSE,
			stride, ((char*) 0) + renderable->getPosOffset());
	}
	if (renderable->getNormalOffset() != -1
		&& shader->hasHandle(Shader::NORMAL))
	{
		glEnableVertexAttribArray(shader->getHandle(Shader::NORMAL));
		glVertexAttribPointer(
			shader->getHandle(Shader::NORMAL), 3, GL_FLOAT, GL_FALSE,
			stride, ((char*) 0) + renderable->getNormalOffset());
	}
	if (renderable->getUVOffset() != -1 && shader->hasHandle(Shader::UV))
	{
		glEnableVertexAttribArray(shader->getHandle(Shader::UV));
		glVertexAttribPointer(
			shader->getHandle(Shader::UV), 2, GL_FLOAT, GL_FALSE,
			stride, ((char*) 0) + renderable->getUVOffset());
	}
}

void GraphicsManager::bindCubeMap()
{
	//if (node->hasResource(Resource::CUBE_MAP)
	//	&& shader->hasHandle(Shader::CUBE_MAP)) {
	//	CubeMap* t = static_cast<CubeMap*>(
	//		node->getResource(Resource::CUBE_MAP));
	//	bindTexture(t->getId(), 0, CUBE_MAP);
	//	glUniform1i(shader->getHandle(Shader::CUBE_MAP), 0);
	//}
}

void GraphicsManager::bindTextures(Shader* shader, int textures[])
{
	textures[0] = glGetUniformLocation(shader->getId(), SHADER_MAIN_TEXTURE);
	//// Bind the texture.
	//vector<Resource*> textures = node->getResources(Resource::TEXTURE_2D);
	//UINT32 size = textures.size() < 8 ? textures.size() : 7;
	//UINT32 texture = 0;
	//for (UINT32 i = 0; i < size; i++) {
	//	texture = i + 1;
	//	const string& name = textures[i]->getName();
	//	TextureRGBA* tex = static_cast<TextureRGBA*>(textures[i]);
	//	SIZE offset = node->getName().length() + 1;
	//	string textName = name.substr(offset, name.length() - 4 - offset);
	//	hTextures[texture] = glGetUniformLocation(shader->getId(), textName.c_str());
	//	if (hTextures[texture] == -1) {
	//		LOGW("Handle to texture \"%s\" not found in \"%s\" shader.",
	//			textName.c_str(), shader->getName().c_str());
	//		continue;
	//	}
	//	bindTexture(tex->getId(), i + 1);
	//	glUniform1i(hTextures[texture], texture);
	//}
}

void GraphicsManager::renderParts(Renderable* renderable, int textures[])
{
	Shader* shader = renderable->getShader();
	SIZE renderCount = renderable->getRenderCount();
	SIZE lastTexture = 0;
	for (SIZE i = 0; i < renderCount; i++)
	{
		renderable->setRenderable(i);
		// Check if this part contains bounding volume.
		//if (renderable->getBV() != 0) {
		//	// Check for frustum culling.
		//	if (renderable->getBV()->isInFrustum(
		//		camera, renderable->getBV()->getPos(), node->getScale())
		//		== BoundingVolume::OUTSIDE)
		//	{
		//		continue;
		//	}
		//}
		shader->setFloat(Shader::SCREEN_WIDTH, (float) services->getScreenWidth());
		shader->setFloat(Shader::SCREEN_HEIGHT, (float) services->getScreenHeight());
		shader->setVector3(Shader::AMBIENT, renderable->getAmbient().toArray());
		shader->setVector3(Shader::DIFFUSE, renderable->getDiffuse().toArray());
		shader->setVector3(Shader::SPECULAR, renderable->getSpecular().toArray());
		shader->setFloat(Shader::SPECULARITY, renderable->getSpecularity());
		shader->setFloat(Shader::TRANSPARENCY, renderable->getTransparency());
		// Bind main texture.
		if (renderable->getTexture() != lastTexture && textures[0] != -1)
		{
			lastTexture = renderable->getTexture();
			if (shader->hasHandle(Shader::MAIN_TEXTURE))
			{
				if (lastTexture == 0)
				{
					shader->setFloat(Shader::MAIN_TEXTURE, 0.0f);
				}
				else
				{
					shader->setFloat(Shader::MAIN_TEXTURE, 1.0f);
				}
			}
			bindTexture(renderable->getTexture());
			glUniform1i(textures[0], 0);
		}
		else if (lastTexture == 0)
		{
			shader->setFloat(Shader::MAIN_TEXTURE, 0.0f);
		}
		renderable->getIBO() == 0 ? drawViaVertices(renderable) : drawViaIndices(renderable);
	}
}

void GraphicsManager::drawViaVertices(Renderable* renderable)
{
	if (renderable->getRenderCount() == 1)
	{
		glDrawArrays(getRenderType(renderable), 0, renderable->getVertexCount());
	}
	else
	{
		glDrawArrays(getRenderType(renderable), renderable->getIndexOffset(), renderable->getIndexCount());
	}
	CHECK_GL_ERROR("glDrawArrays in drawViaVertices()");
}

void GraphicsManager::drawViaIndices(Renderable* renderable)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable->getIBO());
	if (renderable->getIndexType() == Renderable::INDEX_TYPE_USHORT)
	{
		glDrawElements(getRenderType(renderable), (GLint) renderable->getIndexCount(),
		               GL_UNSIGNED_SHORT, ((char*) 0) + renderable->getIndexOffset() * sizeof(short));
		CHECK_GL_ERROR("glDrawElements with short indices");
	}
	else
	{
		glDrawElements(getRenderType(renderable), (GLint) renderable->getIndexCount(),
		               GL_UNSIGNED_INT, ((char*) 0) + renderable->getIndexOffset() * sizeof(int));
		CHECK_GL_ERROR("glDrawElements with int indices in renderNode()");
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GraphicsManager::releaseResources(Renderable* renderable)
{
	Shader* shader = renderable->getShader();
	if (renderable->getBlending()) glDisable(GL_BLEND);
	if (renderable->getUVOffset() != -1 && shader->hasHandle(Shader::UV))
	{
		glDisableVertexAttribArray(shader->getHandle(Shader::UV));
	}
	if (renderable->getNormalOffset() != -1
		&& shader->hasHandle(Shader::NORMAL))
	{
		glDisableVertexAttribArray(shader->getHandle(Shader::NORMAL));
	}
	glDisableVertexAttribArray(shader->getHandle(Shader::POS));
}

int GraphicsManager::getRenderType(Renderable* renderable) const
{
	switch (renderable->getRenderType())
	{
	case Renderable::RENDER_TYPE_POINTS:
		return GL_POINTS;
		//glPointSize(renderable->getPointSize());
	case Renderable::RENDER_TYPE_LINES:
		glLineWidth(renderable->getLineWidth());
		return GL_LINES;
	case Renderable::RENDER_TYPE_TRIANGLES:
		return GL_TRIANGLES;
	case Renderable::RENDER_TYPE_TRIANGLE_FAN:
		return GL_TRIANGLE_FAN;
	case Renderable::RENDER_TYPE_TRIANGLE_STRIP:
		return GL_TRIANGLE_STRIP;
	case Renderable::RENDER_TYPE_QUADS:
		return GL_TRIANGLES;
	default:
		THROWEX("Unknown render type: %d.", renderable->getRenderType());
	}
}

void GraphicsManager::renderQuad(
	Shader* shader, UINT32 colorBuffer, UINT32 depthBuffer, float widthScale, float heightScale)
{
	if (shader == 0)
	{
		LOGE("Shader for quad not found!");
		return;
	}
	int shaderId = shader->getId();
	useProgram(shaderId);
	if (!shader->hasHandle(Shader::WVP)
		|| !shader->hasHandle(Shader::POS)
		|| !shader->hasHandle(Shader::UV))
	{
		LOGE("Shader does not "
			"contain required variables.");
		return;
	}
	// World * View * Projection matrix.
	Mat4 res, pos, rot, posRot, scale, posScale;
	Matrix::translate(pos,
	                  (float) services->getScreenWidth() * 0.5f / widthScale,
	                  (float) services->getScreenHeight() * 0.5f / heightScale,
	                  -1.0f);
	Matrix::rotateX(rot, -90.0f);
	Matrix::scale(scale,
	              (float) services->getScreenWidth() / widthScale,
	              1.0f,
	              (float) services->getScreenHeight() / heightScale);
	Matrix::multiply(pos, rot, posRot);
	Matrix::multiply(posRot, scale, posScale);
	Mat4 proj;
	Matrix::projection2D(proj,
	                     (float) services->getScreenWidth(),
	                     (float) services->getScreenHeight(),
	                     database_->getFloat(Database::FAR_PLANE_DISTANCE));
	Matrix::multiply(
		proj,
		posScale,
		res);
	shader->setMatrix4(Shader::WVP, res);
	// Timer.
	shader->setFloat(Shader::TIMER, getMicroseconds() / 1000000.0f);
	// Screen width.
	shader->setFloat(Shader::SCREEN_WIDTH,
	                 (float) services->getScreenWidth());
	// Screen height.
	shader->setFloat(Shader::SCREEN_HEIGHT,
	                 (float) services->getScreenHeight());
	bindTexture(colorBuffer);
	glUniform1i(shader->getHandle(Shader::COLOR_BUFFER), 0);
	if (shader->hasHandle(Shader::DEPTH_BUFFER))
	{
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
	for (UINT32 i = 0; i < 2; i++)
	{
		bindTexture(0, i);
	}
	// Disable vertex arrays.
	glDisableVertexAttribArray(shader->getHandle(Shader::UV));
	glDisableVertexAttribArray(shader->getHandle(Shader::POS));
	CHECK_GL_ERROR("Error rendering post processing.");
}

void GraphicsManager::setWindingOrder(int type)
{
	if (windingType_ != type)
	{
		windingType_ = type;
		if (type == Renderable::WINDING_TYPE_CCW)
		{
			glFrontFace(GL_CCW);
		}
		else
		{
			glFrontFace(GL_CW);
		}
	}
}

void GraphicsManager::bindTexture(SIZE id, SIZE index, TextureType type)
{
	if (index > MAX_ACTIVE_TEXTURES)
	{
		LOGW("Trying to use more active textures than possible.");
		return;
	}
	if (index != activeTexture_)
	{
		glActiveTexture(GL_TEXTURE0 + index);
		activeTexture_ = index;
	}
	if (boundTextures_[index] != id)
	{
		boundTextures_[index] = id;
		if (type == T2D)
		{
			glBindTexture(GL_TEXTURE_2D, (GLint) id);
		}
		else
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, (GLint) id);
		}
	}
}

void GraphicsManager::useProgram(SIZE id)
{
	if (id == shaderId_)
	{
		return;
	}
	shaderId_ = id;
	glUseProgram(id);
}

bool GraphicsManager::bindBuffer(SIZE id)
{
	if (bufferId_ == id)
	{
		return false;
	}
	bufferId_ = id;
	glBindBuffer(GL_ARRAY_BUFFER, id);
	return true;
}

#endif
