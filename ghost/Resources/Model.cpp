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
#include "ModelFactory.h"

Model::Model(ServiceLocator* services) :
	Resource(services),
	modelData_(0),
	cbo_(0),
	ibo_(0),
	currentPart_(0)
{}

Model::~Model() {
	release();
}

bool Model::create() {
	string model = getAttribute(ATTR_TYPE);
	modelData_ = ModelFactory::create(model, getAttributes(), getServiceLocator());
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
		} else if (modelData_->getIndexType() == Renderable::INDEX_TYPE_UINT) {
			setIndexType(INDEX_TYPE_UINT);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelData_->getIndexCount() * sizeof(UINT32),
				modelData_->getIndicesInt(), GL_STATIC_DRAW);
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	} else {
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

	return true;
}

void Model::release() {
	if (cbo_ != 0) {
		glDeleteBuffers(1, &cbo_);
		cbo_ = 0;
	}
	if (ibo_ != 0) {
		glDeleteBuffers(1, &ibo_);
		ibo_ = 0;
	}
	if (modelData_ != 0) {
		for (UINT32 i = 0; i < modelData_->getParts().size(); i++) {
			delete modelData_->getParts()[i].bv_;
		}
		delete modelData_;
		modelData_ = 0;
	}
}

SIZE Model::getSize() {
	return sizeof(Model);
}

Resource::Type Model::getType() {
	return Resource::MODEL;
}

bool Model::isValid() {
	return cbo_ != 0 && getShader() != 0;
}

SIZE Model::getVertexCount() {
	return modelData_->getVertexCount();
}

SIZE Model::getTexture() {
	if (modelData_->getParts().size() == 0) {
		return 0;
	}
	if (modelData_->getMaterials().size() <= modelData_->getParts()[currentPart_].material_) {
		return 0;
	}
	Texture* tex = modelData_->getMaterials()[modelData_->getParts()[currentPart_].material_].texture_;
	return tex != 0 ? tex->getId() : 0;
}

SIZE Model::getIBO() {
	return ibo_;
}

SIZE Model::getIndexOffset() {
	return modelData_->getParts().size() > currentPart_ ?
		modelData_->getParts()[currentPart_].offset_ : 0;
}

BoundingVolume* Model::getBV() {
	return modelData_->getParts().size() > currentPart_ ?
		modelData_->getParts()[currentPart_].bv_ : 0;
}

SIZE Model::getCBO() {
	return cbo_;
}

SIZE Model::getIndexCount() {
	return modelData_->getParts().size() > currentPart_ ?
		modelData_->getParts()[currentPart_].indexCount_ : 0;
}
SIZE Model::getRenderCount() {
	return (UINT32) (modelData_->getParts().size() == 0 ? 1 : modelData_->getParts().size());
}

void Model::setRenderable(SIZE i) {
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

SIZE Model::getVertexStride() {
	return modelData_->getVertexStride();
}

int Model::getPosOffset() {
	return modelData_->getPosOffset();
}

int Model::getNormalOffset() {
	return modelData_->hasNormals() ? modelData_->getNormalOffset() : -1;
}

int Model::getUVOffset() {
	return modelData_->hasUV() ? modelData_->getUVOffset() : -1;
}

Shader* Model::getDefaultShader() {
	return Shader::getDefaultModelShader(getServiceLocator());
}

ModelData* Model::getData() {
	return modelData_;
}

void Model::addTexture(Texture* texture) {
	vector<ModelData::Part>& parts = modelData_->getParts();
	if (parts.size() > 0) {
		ModelData::Material m;
		m.ambient_ = getAmbient();
		m.texture_ = texture;
		modelData_->getMaterials().push_back(m);
		parts[0].material_ = modelData_->getMaterials().size() - 1;
	}
}