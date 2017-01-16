/*
 * Model.cpp
 *
 *  Created on: 2012.12.23
 *      Author: Martynas Šustavičius
 */

#include "Model.h"
#include "../Multiplatform/ServiceLocator.h"
#include "../Multiplatform/Database.h"
#include "Vertex.h"
#include "Texture.h"
#include "Shader.h"
#include "../ModelData.h"
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
#include "ModelFactory.h"

Model::Model(ServiceLocator* services) :
	Resource(services),
	modelData(0),
	cbo(0),
	ibo(0),
	currentPart(0)
{}

Model::~Model() {
	release();
}

bool Model::create() {
	GraphicsManager* gm = getServiceLocator()->getGraphicsManager();
	string model = getAttribute(ATTR_TYPE);
	modelData = ModelFactory::create(model, getAttributes(), getServiceLocator());
	setCullFace(modelData->areFacesCulled());
	// Move data to GPU.
	// Create combined position, normal and uv buffer object.
	gm->setVertexBuffer(cbo, modelData->getVertices(), modelData->getVertexCount() * modelData->getVertexStride());
	// Create index buffer object.
	if (modelData->getIndexCount() > 0) {
		if (modelData->getIndexType() == Renderable::INDEX_TYPE_USHORT) {
			setIndexType(INDEX_TYPE_USHORT);
			gm->setVertexBuffer(ibo, modelData->getIndicesShort(), modelData->getIndexCount() * sizeof(UINT16));
		} else if (modelData->getIndexType() == Renderable::INDEX_TYPE_UINT) {
			setIndexType(INDEX_TYPE_UINT);
			gm->setVertexBuffer(ibo, modelData->getIndicesInt(), modelData->getIndexCount() * sizeof(UINT32));
		}
	} else {
		LOGD("Model \"%s\" does not contain indices.", model.c_str());
	}
	if (getAttribute(ATTR_AMBIENT).length() > 0) {
		setAmbient(Color(getAttribute(ATTR_AMBIENT)));
		if (modelData->getMaterials().size() > 0) {
			for (UINT32 i = 0; i < modelData->getMaterials().size(); i++) {
				modelData->getMaterials()[i].ambient_ = getAmbient();
			}
		}
	}
	if (getAttribute(ATTR_DIFFUSE).length() > 0) {
		setDiffuse(Color(getAttribute(ATTR_DIFFUSE)));
		if (modelData->getMaterials().size() > 0) {
			for (UINT32 i = 0; i < modelData->getMaterials().size(); i++) {
				modelData->getMaterials()[i].diffuse_ = getDiffuse();
			}
		}
	}
	if (getAttribute(ATTR_SPECULAR).length() > 0) {
		setSpecular(Color(getAttribute(ATTR_SPECULAR)));
		if (modelData->getMaterials().size() > 0) {
			for (UINT32 i = 0; i < modelData->getMaterials().size(); i++) {
				modelData->getMaterials()[i].specular_ = getSpecular();
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

	return true;
}

void Model::release() {
	if (cbo != 0) {
		getServiceLocator()->getGraphicsManager()->unsetVertexBuffer(cbo);
	}
	if (ibo != 0) {
		getServiceLocator()->getGraphicsManager()->unsetIndexBuffer(ibo);
	}
	if (modelData != 0) {
		for (UINT32 i = 0; i < modelData->getParts().size(); i++) {
			delete modelData->getParts()[i].bv_;
		}
		delete modelData;
		modelData = 0;
	}
}

SIZE Model::getSize() {
	return sizeof(Model);
}

Resource::Type Model::getType() {
	return Resource::MODEL;
}

bool Model::isValid() {
	return cbo != 0 && getShader() != 0;
}

SIZE Model::getVertexCount() {
	return modelData->getVertexCount();
}

SIZE Model::getTexture() {
	if (modelData->getParts().size() == 0) {
		return 0;
	}
	if (modelData->getMaterials().size() <= modelData->getParts()[currentPart].material_) {
		return 0;
	}
	Texture* tex = modelData->getMaterials()[modelData->getParts()[currentPart].material_].texture_;
	return tex != 0 ? tex->getId() : 0;
}

SIZE Model::getIBO() {
	return ibo;
}

SIZE Model::getIndexOffset() {
	return modelData->getParts().size() > currentPart ?
		modelData->getParts()[currentPart].offset_ : 0;
}

BoundingVolume* Model::getBV() {
	return modelData->getParts().size() > currentPart ?
		modelData->getParts()[currentPart].bv_ : 0;
}

SIZE Model::getCBO() {
	return cbo;
}

SIZE Model::getIndexCount() {
	return modelData->getParts().size() > currentPart ?
		modelData->getParts()[currentPart].indexCount_ : 0;
}
SIZE Model::getRenderCount() {
	return (UINT32) (modelData->getParts().size() == 0 ? 1 : modelData->getParts().size());
}

void Model::setRenderable(SIZE i) {
	currentPart = i;
	if (modelData->getParts().size() == 0) {
		return;
	}
	if (modelData->getMaterials().size() <= modelData->getParts()[currentPart].material_) {
		return;
	}
	ModelData::Material& mat = modelData->getMaterials()[modelData->getParts()[currentPart].material_];
	getAmbient().setRGBA(mat.ambient_);
	getDiffuse().setRGBA(mat.diffuse_);
	getSpecular().setRGBA(mat.specular_);
	setSpecularity(mat.specIntensity_);
	setTransparency(mat.transparency_);
}

SIZE Model::getVertexStride() {
	return modelData->getVertexStride();
}

int Model::getPosOffset() {
	return modelData->getPosOffset();
}

int Model::getNormalOffset() {
	return modelData->hasNormals() ? modelData->getNormalOffset() : -1;
}

int Model::getUVOffset() {
	return modelData->hasUV() ? modelData->getUVOffset() : -1;
}

Shader* Model::getDefaultShader() {
	return Shader::getDefaultModelShader(getServiceLocator());
}

ModelData* Model::getData() {
	return modelData;
}

void Model::setTexture(Texture* texture, UINT32 index) {
	vector<ModelData::Part>& parts = modelData->getParts();
	if (parts.size() > index) {
		ModelData::Material m;
		m.ambient_ = getAmbient();
		m.texture_ = texture;
		modelData->getMaterials().push_back(m);
		parts[index].material_ = modelData->getMaterials().size() - 1;
	}
}