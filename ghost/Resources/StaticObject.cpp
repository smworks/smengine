/*
 * StaticObject.cpp
 *
 *  Created on: 2012.12.23
 *      Author: Martynas Šustavičius
 */

#include "StaticObject.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Multiplatform/Database.h"
#include "Vertex.h"
#include "TextureRGBA.h"
#include "Shader.h"
#include "../ObjParser.h"
#include "../TerrainParser.h"
#include "../ShapeParser.h"
#include "../ResourceManager.h"
#include "../Shapes.h"
#include "../ScriptManager.h"
#include "../BoundingSphere.h"
#include "../BoundingBox.h"
#include "../Utils.h"
#include "../Node.h"
#include "../Multiplatform/GraphicsManager.h"

StaticObject::StaticObject(ServiceLocator* services) :
	Resource(services),
	modelData_(NEW ModelData()),
	cbo_(0),
	ibo_(0),
	currentPart_(0)
{}

StaticObject::~StaticObject() {
	release();
}

bool StaticObject::create() {
	string model = getAttribute(ATTR_TYPE);
	bool ret = false;
	if (model == VAL_TERRAIN) {
		ret = createTerrain();
	}
	else if (model == VAL_PLANE) {
		ret = createPlane();
	}
	else if (model == VAL_SHAPE) {
		ret = createShape();
	}
	else if (model == VAL_SPHERE) {
		ret = createSphere();
	}
	else if (model == VAL_WATER) {
		ret = createWater();
	}
	else if (model == VAL_MESH) {
		ret = createModel();
	}
	else {
        if (model.length() > 0) {
            LOGW("Unknown model type: %s.", model.c_str());
        }
        else {
            LOGD("No model type specified. Attempting to load as model type.");
        }
		ret = createModel();
	}
	if (!ret) {
		LOGW("3D model not created.");
		return false;
	}
	// Move data to GPU.
	// Create combined position, normal and uv buffer object.
	glGenBuffers(1, &cbo_);
	getServiceLocator()->getGraphicsManager()->bindBuffer(cbo_);
	glBufferData(GL_ARRAY_BUFFER,
		modelData_->getVertexCount() * modelData_->getVertexStride(),
		modelData_->getVertices(), GL_STATIC_DRAW);
	// Create index buffer object.
	if (modelData_->getIndexCount() > 0) {
		glGenBuffers(1, &ibo_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
		if (modelData_->getIndexType() == Renderable::INDEX_TYPE_USHORT) {
			setIndexType(INDEX_TYPE_USHORT);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelData_->getIndexCount() * sizeof(UINT16),
				modelData_->getIndicesShort(), GL_STATIC_DRAW);
		}
		else if (modelData_->getIndexType() == Renderable::INDEX_TYPE_UINT) {
			setIndexType(INDEX_TYPE_UINT);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelData_->getIndexCount() * sizeof(UINT32),
				modelData_->getIndicesInt(), GL_STATIC_DRAW);
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else {
		LOGD("Model \"%s\" does not contain indices.", getAttribute(ATTR_FILE).c_str());
	}
	if (getAttribute(ATTR_AMBIENT).length() > 0) {
		setAmbient(Color(getAttribute(ATTR_AMBIENT)));
		if (modelData_->getMaterials().size() > 0) {
			for (UINT32 i = 0; i < modelData_->getMaterials().size(); i++) {
				modelData_->getMaterials()[i].ambient_ = getAmbient();
			}
		}
	}
	if (getAttribute(ATTR_DIFFUSE).length() > 0) {
		setDiffuse(Color(getAttribute(ATTR_DIFFUSE)));
		if (modelData_->getMaterials().size() > 0) {
			for (UINT32 i = 0; i < modelData_->getMaterials().size(); i++) {
				modelData_->getMaterials()[i].diffuse_ = getDiffuse();
			}
		}
	}
	if (getAttribute(ATTR_SPECULAR).length() > 0) {
		setSpecular(Color(getAttribute(ATTR_SPECULAR)));
		if (modelData_->getMaterials().size() > 0) {
			for (UINT32 i = 0; i < modelData_->getMaterials().size(); i++) {
				modelData_->getMaterials()[i].specular_ = getSpecular();
			}
		}
	}
	if (getAttribute(ATTR_CULL_FACE).length() > 0) {
		setCullFace(toBool(getAttribute(ATTR_CULL_FACE)));
	}
	if (getAttribute(ATTR_BLENDING).length() > 0) {
		setBlending(toBool(getAttribute(ATTR_BLENDING)));
	}
	if (checkGLError("Compiling static object data.")) {
		release();
		return false;
	}

	return ret;
}

void StaticObject::release() {
	if (cbo_ != 0) {
		glDeleteBuffers(1, &cbo_);
		cbo_ = 0;
	}
	if (ibo_ != 0) {
		glDeleteBuffers(1, &ibo_);
		ibo_ = 0;
	}
	for (UINT32 i = 0; i < modelData_->getParts().size(); i++) {
		delete modelData_->getParts()[i].bv_;
	}
	delete modelData_;
	modelData_ = 0;
}

SIZE StaticObject::getSize() {
	return sizeof(StaticObject);
}

Resource::Type StaticObject::getType() {
	return Resource::STATIC_OBJECT;
}

bool StaticObject::isValid() {
	return cbo_ != 0 && getShader() != 0;
}

SIZE StaticObject::getVertexCount() {
	return modelData_->getVertexCount();
}

SIZE StaticObject::getTexture() {
	if (modelData_->getParts().size() == 0) {
		return 0;
	}
	if (modelData_->getMaterials().size() <= modelData_->getParts()[currentPart_].material_) {
		return 0;
	}
	Texture* tex = modelData_->getMaterials()[modelData_->getParts()[currentPart_].material_].texture_;
	return tex != 0 ? tex->getId() : 0;
}

SIZE StaticObject::getIBO() {
	return ibo_;
}

SIZE StaticObject::getIndexOffset() {
	return modelData_->getParts().size() > currentPart_ ?
		modelData_->getParts()[currentPart_].offset_ : 0;
}

BoundingVolume* StaticObject::getBV() {
	return modelData_->getParts().size() > currentPart_ ?
		modelData_->getParts()[currentPart_].bv_ : 0;
}

SIZE StaticObject::getCBO() {
	return cbo_;
}

SIZE StaticObject::getIndexCount() {
	return modelData_->getParts().size() > currentPart_ ?
		modelData_->getParts()[currentPart_].indexCount_ : 0;
}
SIZE StaticObject::getRenderCount() {
	return (UINT32) (modelData_->getParts().size() == 0 ? 1 : modelData_->getParts().size());
}

void StaticObject::setRenderable(SIZE i) {
	currentPart_ = i;
	if (modelData_->getParts().size() == 0) {
		return;
	}
	if (modelData_->getMaterials().size() <= modelData_->getParts()[currentPart_].material_) {
		return;
	}
	ModelData::Material& mat = modelData_->getMaterials()[modelData_->getParts()[currentPart_].material_];
	getAmbient().setRGBA(mat.ambient_);
	getDiffuse().setRGBA(mat.diffuse_);
	getSpecular().setRGBA(mat.specular_);
	setSpecularity(mat.specIntensity_);
	setTransparency(mat.transparency_);
}

SIZE StaticObject::getVertexStride() {
	return modelData_->getVertexStride();
}

int StaticObject::getPosOffset() {
	return modelData_->getPosOffset();
}

int StaticObject::getNormalOffset() {
	return modelData_->hasNormals() ? modelData_->getNormalOffset() : -1;
}

int StaticObject::getUVOffset() {
	return modelData_->hasUV() ? modelData_->getUVOffset() : -1;
}

Shader* StaticObject::getDefaultShader() {
	return Shader::getDefaultModelShader(getServiceLocator());
}

BoundingVolume* StaticObject::getBoundingVolume() {
	return modelData_->getBoundingVolume();
}

ModelData* StaticObject::getModel() {
	return modelData_;
}

bool StaticObject::createModel() {
	ModelData& model = *modelData_;
	string file = getAttribute(ATTR_FILE);
	if (file.length() == 0) {
		LOGW("3D model name not specified.");
		if (getNode() != 0) {
			LOGW("Node that contains model: \"%s\".", getNode()->getName().c_str());
		}
	}
	if (!ObjParser::parse(model, file, getServiceLocator())) {
		LOGW("Failed to parse \"%s\" 3D model.", file.c_str());
		return false;
	}
	return true;
}

bool StaticObject::createTerrain() {
	string file = getAttribute(ATTR_FILE);
	TextureRGBA* map = static_cast<TextureRGBA*>(getServiceLocator()->getRM()->get(
		Resource::TEXTURE_2D, file));
	if (map == 0) {
		map = NEW TextureRGBA(getServiceLocator());
		map->getAttributes().setString(ATTR_FILE, getAttribute(ATTR_FILE));
		map->create();
		getServiceLocator()->getRM()->add(file, map);
	}
	if (!map->isValid()) {
		LOGW("Unable to create terrain, because source image \"%s\" is not valid.",
			file.c_str());
		return false;
	}
	TerrainParser parser(getServiceLocator(), map, 0.2f, -15.0f);
	TerrainGeometry tg;
	BoundingVolume* bv = 0;
	parser.load(tg, bv);
	ModelData::Material mat;
	mat.texture_ = 0;
	mat.ambient_ = Color(0.2f, 0.2f, 0.2f);
	mat.diffuse_ = Color(0.8f, 0.8f, 0.8f);
	mat.specular_ = Color(0.0f, 0.0f, 0.0f);
	mat.specIntensity_ = 0.0f;
	mat.transparency_ = 1.0f;
	modelData_->getMaterials().push_back(mat);
	SIZE vertexCount = tg.vertices_->size() / 3;
	VertexPNT* vertices = NEW VertexPNT[vertexCount];
	VertexPNT v;
	for (SIZE i = 0; i < vertexCount; i++) {
		v.pos[0] = tg.vertices_->at(i * 3 + 0);
		v.pos[1] = tg.vertices_->at(i * 3 + 1);
		v.pos[2] = tg.vertices_->at(i * 3 + 2);
		v.normals[0] = tg.normals_->at(i * 3 + 0);
		v.normals[1] = tg.normals_->at(i * 3 + 1);
		v.normals[2] = tg.normals_->at(i * 3 + 2);
		v.uv[0] = tg.uv_->at(i * 2 + 0);
		v.uv[1] = tg.uv_->at(i * 2 + 1);
		vertices[i] = v;
	}
	modelData_->setVertices(ModelData::PNT,
		reinterpret_cast<UINT8*>(vertices), vertexCount);
	UINT32 indCount = (UINT32) tg.indices_->size();
	modelData_->getParts().resize(indCount);
	if (tg.vertices_->size() / 3 - 1 > USHRT_MAX) {
		LOGD("Terrain uses integer indices.");
		vector<UINT32> indices;
		SIZE offset = 0;
		for (UINT32 i = 0; i < indCount; i++)	{
			ModelData::Part& part = modelData_->getParts()[i];
			vector<UINT16>* ind = tg.indices_->at(i);
			SIZE size = ind->size();
			part.indexCount_ = (UINT32) size;
			part.offset_ = offset;
			offset += size;
			for (SIZE j = 0; j < size; j++) {
				indices.push_back(tg.indices_->at(i)->at(j));
			}
			if (tg.bounds_ != 0 && tg.bounds_->size() > i) {
				part.bv_ = tg.bounds_->at(i)->copy();
			}
		}
		UINT32* indArray = new UINT32[indices.size()];
		memcpy(indArray, &indices[0], indices.size() * sizeof(UINT32));
		modelData_->setIndices(Renderable::INDEX_TYPE_UINT,
			reinterpret_cast<UINT8*>(indArray), indices.size());
	}
	else {
		LOGD("Terrain uses short indices.");
		vector<UINT16> indices;
		SIZE offset = 0;
		for (UINT32 i = 0; i < indCount; i++)	{
			ModelData::Part& part = modelData_->getParts()[i];
			vector<UINT16>* ind = tg.indices_->at(i);
			SIZE size = ind->size();
			part.indexCount_ = (UINT32) size;
			part.offset_ = offset;
			offset += size;
			for (SIZE j = 0; j < size; j++) {
				indices.push_back(tg.indices_->at(i)->at(j));
			}
			if (tg.bounds_ != 0 && tg.bounds_->size() > i) {
				part.bv_ = tg.bounds_->at(i)->copy();
			}
		}
		UINT16* indArray = new UINT16[indices.size()];
		memcpy(indArray, &indices[0], indices.size() * sizeof(UINT16));
		modelData_->setIndices(Renderable::INDEX_TYPE_USHORT,
			reinterpret_cast<UINT8*>(indArray), indices.size());
	}
	return true;
}

bool StaticObject::createPlane() {
	setCullFace(false);
	static float vert[] = {
			-0.5f, 0.0f, 0.5f,
			0.5f, 0.0f, 0.5f,
			0.5f, 0.0f, -0.5f,
			0.5f, 0.0f, -0.5f,
			-0.5f, 0.0f, -0.5f,
			-0.5f, 0.0f, 0.5f
	};
	static float normals[] = {
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
	};
	UINT32 vertexCount = sizeof(vert) / sizeof(float) / 3;
	VertexPNT* vertices = NEW VertexPNT[vertexCount];
	VertexPNT v;
	for (UINT32 i = 0; i < vertexCount; i++) {
		v.pos[0] = vert[i * 3 + 0];
		v.pos[1] = vert[i * 3 + 1];
		v.pos[2] = vert[i * 3 + 2];
		v.normals[0] = normals[i * 3 + 0];
		v.normals[1] = normals[i * 3 + 1];
		v.normals[2] = normals[i * 3 + 2];
		v.uv[0] = g_planeUV[i * 2 + 0];
		v.uv[1] = g_planeUV[i * 2 + 1];
		vertices[i] = v;
	}
	// Create bounding volume.
	modelData_->setBoundingVolume(NEW BoundingBox(Vec3(1.0f, 0.0f, 1.0f)));
	modelData_->setVertices(ModelData::PNT, reinterpret_cast<UINT8*>(vertices), vertexCount);
	setAttribute(Resource::ATTR_WIDTH, "1.0f");
	setAttribute(Resource::ATTR_HEIGHT, "0.0f");
	setAttribute(Resource::ATTR_DEPTH, "1.0f");
	return true;
}

bool StaticObject::createSphere() {
	setCullFace(false);
	vector<Vec3> vert;
	vector<UINT16> indices;
	INT32 vLod = getAttributes().getInt(ATTR_VERTICAL_LOD, 2);
	INT32 hLod = getAttributes().getInt(ATTR_HORIZONTAL_LOD, 2);
	Shapes::getSphere(vLod, hLod, &vert, 0, 0, &indices);
	SIZE vertexCount = vert.size();
	VertexPNT* vertices = NEW VertexPNT[vertexCount];
	VertexPNT v;
	for (SIZE i = 0; i < vertexCount; i++) {
		vert.at(i).getXYZ(v.pos);
		vertices[i] = v;
	}
	modelData_->setVertices(ModelData::PNT, reinterpret_cast<UINT8*>(vertices), vertexCount);
	UINT16* ind = NEW UINT16[indices.size()];
	memcpy(ind, &indices[0], indices.size() * sizeof(UINT16));
	modelData_->setIndices(Renderable::INDEX_TYPE_USHORT, ind, indices.size());
	modelData_->getParts().resize(1);
	modelData_->getParts()[0].indexCount_ = indices.size();
	modelData_->getParts()[0].offset_ = 0;
	modelData_->setBoundingVolume(NEW BoundingSphere(0.5f));
	setAttribute(Resource::ATTR_RADIUS, "0.5f");
	return true;
}

bool StaticObject::createShape() {
	string name = getAttribute(ATTR_FILE);
	if (name.length() == 0) {
		LOGE("No map file specified.");
		return false;
	}
	TextureRGBA* map = static_cast<TextureRGBA*>(
		getServiceLocator()->getRM()->get(TEXTURE_2D, name));
	if (map == 0) {
		map = NEW TextureRGBA(getServiceLocator());
		map->getAttributes().setString(ATTR_FILE, name);
		map->create();
		getServiceLocator()->getRM()->add(name, map);
	}
	vector<Vec3>* vert = NEW vector<Vec3>();
	vector<float>* uv = NEW vector<float>();
	vector<UINT16>* ind = NEW vector<UINT16>();
	float height = getAttributes().getFloat(ATTR_HEIGHT, 1.0f);
	if (!ShapeParser::parse(vert, uv, ind, map, height)) {
		LOGE("Unable to parse texture.");
		return false;
	}
	delete uv;
	
	SIZE vertexCount = vert->size();
	VertexPNT* vertices = NEW VertexPNT[vertexCount];
	VertexPNT v;
	for (SIZE i = 0; i < vertexCount; i++) {
		vert->at(i).getXYZ(v.pos);
		vertices[i] = v;
	}
	modelData_->setVertices(ModelData::PNT, reinterpret_cast<UINT8*>(vertices), vertexCount);
	UINT16* indices = NEW UINT16[ind->size()];
	memcpy(indices, &(*ind)[0], ind->size() * sizeof(UINT16));
	modelData_->setIndices(Renderable::INDEX_TYPE_USHORT, indices, ind->size());
	modelData_->getParts().resize(1);
	modelData_->getParts()[0].indexCount_ = ind->size();
	modelData_->getParts()[0].offset_ = 0;
	modelData_->setBoundingVolume(NEW BoundingBox(Vec3(
		(float) map->getWidth(), height, (float) map->getHeight())));
	setCullFace(false);
	delete ind;
	delete vert;
	return true;
}

bool StaticObject::createWater() {
	int rows = toInt(getAttribute(ATTR_ROWS).c_str());
	int cols = toInt(getAttribute(ATTR_COLS).c_str());
	vector<float> vert;
	vector<UINT16> ind;
	Shapes::grid(rows, cols, 1.0f, 1.0f, &vert, &ind);
	SIZE vertexCount = vert.size() / 3;
	VertexPT* vertices = NEW VertexPT[vertexCount];
	VertexPT v;
    SIZE row = vertexCount / cols;
    SIZE col = vertexCount % cols;
	for (SIZE i = 0; i < vertexCount; i++) {
		v.pos[0] = vert[i * 3 + 0];
		v.pos[1] = vert[i * 3 + 1];
		v.pos[2] = vert[i * 3 + 2];
		v.uv[0] = (float) cols / (float) col;
		v.uv[1] = (float) rows / (float) row;
		vertices[i] = v;
	}
	modelData_->setVertices(ModelData::PT, reinterpret_cast<UINT8*>(vertices), vertexCount);
	modelData_->getParts().resize(1);
	modelData_->getParts()[0].indexCount_ = (UINT32) ind.size();
	return true;
}