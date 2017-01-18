#include "ModelFactory.h"
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
#include "../Multiplatform/FileManager.h"

ModelData* ModelFactory::create(string model, Attributes& attr, ServiceLocator* sl) {
	ModelData* md = NEW ModelData();

	if (model == Resource::VAL_TERRAIN) {
		createTerrain(md, attr, sl);
	} else if (model == Resource::VAL_PLANE) {
		createPlane(md, attr, sl);
	} else if (model == Resource::VAL_SHAPE) {
		createShape(md, attr, sl);
	} else if (model == Resource::VAL_SPHERE) {
		createSphere(md, attr);
	} else if (model == Resource::VAL_WATER) {
		createWater(md, attr, sl);
	} else if (model == Resource::VAL_MESH) {
		createModel(md, attr, sl);
	} else {
		if (model.length() > 0) {
			THROWEX("Unknown model type: %s.", model.c_str());
		} else {
			THROWEX("No model type specified. Attempting to load as model type.");
		}
	}

	return md;
}

void ModelFactory::createModel(ModelData* md, Attributes& attr, ServiceLocator* sl) {
	ModelData& model = *md;
	string file = attr.getString(Resource::ATTR_FILE);
	if (file.length() == 0) {
		THROWEX("3D model name not specified.");
	}

	char* bytes;
	SIZE size;
	sl->getFileManager()->loadRaw(bytes, size, (GHOST_MODELS + file).c_str());
	md->deserialize(sl, bytes);
	delete[] bytes;
}

void ModelFactory::createTerrain(ModelData* md, Attributes& attr, ServiceLocator* sl) {
	string file = attr.getString(Resource::ATTR_FILE);
	Texture* map = Texture::load(sl, file);
	if (!map->isValid()) {
		LOGW("Unable to create terrain, because source image \"%s\" is not valid.",
			file.c_str());
	}
	TerrainParser parser(sl, map, 0.2f, -15.0f);
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
	md->getMaterials().push_back(mat);
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
	md->setVertices(ModelData::PNT,
		reinterpret_cast<UINT8*>(vertices), vertexCount);
	UINT32 indCount = (UINT32) tg.indices_->size();
	md->getParts().resize(indCount);
	if (tg.vertices_->size() / 3 - 1 > USHRT_MAX) {
		LOGD("Terrain uses integer indices.");
		vector<UINT32> indices;
		SIZE offset = 0;
		for (UINT32 i = 0; i < indCount; i++)	{
			ModelData::Part& part = md->getParts()[i];
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
		UINT32* indArray = NEW UINT32[indices.size()];
		memcpy(indArray, &indices[0], indices.size() * sizeof(UINT32));
		md->setIndices(Renderable::INDEX_TYPE_UINT,
			reinterpret_cast<UINT8*>(indArray), indices.size());
	} else {
		LOGD("Terrain uses short indices.");
		vector<UINT16> indices;
		SIZE offset = 0;
		for (UINT32 i = 0; i < indCount; i++)	{
			ModelData::Part& part = md->getParts()[i];
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
		UINT16* indArray = NEW UINT16[indices.size()];
		memcpy(indArray, &indices[0], indices.size() * sizeof(UINT16));
		md->setIndices(Renderable::INDEX_TYPE_USHORT,
			reinterpret_cast<UINT8*>(indArray), indices.size());
	}
}

void ModelFactory::createPlane(ModelData* md, Attributes& attr, ServiceLocator* sl) {
	md->setCullFace(false);
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
	md->getParts().resize(1);
	md->setVertices(ModelData::PNT, reinterpret_cast<UINT8*>(vertices), vertexCount);
	md->setBoundingVolume(NEW BoundingBox(Vec3(1.0f, 0.0f, 1.0f)));
	attr.setString(Resource::ATTR_WIDTH, "1.0f");
	attr.setString(Resource::ATTR_HEIGHT, "0.0f");
	attr.setString(Resource::ATTR_DEPTH, "1.0f");
}

void ModelFactory::createSphere(ModelData* md, Attributes& attr) {
	md->setCullFace(false);
	vector<Vec3> vert;
	vector<UINT16> indices;
	INT32 vLod = attr.getInt(Resource::ATTR_VERTICAL_LOD, 12);
	INT32 hLod = attr.getInt(Resource::ATTR_HORIZONTAL_LOD, 12);
	if (vLod < 12 || hLod < 12) {
		THROWEX("Sphere vetical and horizontal LOD levels must be atleast 12.");
	}
	Shapes::getSphere(vLod, hLod, &vert, 0, 0, &indices);
	SIZE vertexCount = vert.size();
	VertexPNT* vertices = NEW VertexPNT[vertexCount];
	VertexPNT v;
	for (SIZE i = 0; i < vertexCount; i++) {
		vert.at(i).getXYZ(v.pos);
		vertices[i] = v;
	}
	md->setVertices(ModelData::PNT, reinterpret_cast<UINT8*>(vertices), vertexCount);
	UINT16* ind = NEW UINT16[indices.size()];
	memcpy(ind, &indices[0], indices.size() * sizeof(UINT16));
	md->setIndices(Renderable::INDEX_TYPE_USHORT, ind, indices.size());
	md->getParts().resize(1);
	md->getParts()[0].indexCount_ = indices.size();
	md->getParts()[0].offset_ = 0;
	md->setBoundingVolume(NEW BoundingSphere(0.5f));
	attr.setString(Resource::ATTR_RADIUS, "0.5f");
}

void ModelFactory::createShape(ModelData* md, Attributes& attr, ServiceLocator* sl) {
	string name = attr.getString(Resource::ATTR_FILE);
	if (name.length() == 0) {
		THROWEX("No map file specified.");
	}
	Texture* map = Texture::load(sl, name);
	vector<Vec3>* vert = NEW vector<Vec3>();
	vector<float>* uv = NEW vector<float>();
	vector<UINT16>* ind = NEW vector<UINT16>();
	float height = attr.getFloat(Resource::ATTR_HEIGHT, 1.0f);
	if (!ShapeParser::parse(vert, uv, ind, map, height)) {
		THROWEX("Unable to parse texture.");
	}
	delete uv;
	
	SIZE vertexCount = vert->size();
	VertexPNT* vertices = NEW VertexPNT[vertexCount];
	VertexPNT v;
	for (SIZE i = 0; i < vertexCount; i++) {
		vert->at(i).getXYZ(v.pos);
		vertices[i] = v;
	}
	md->setVertices(ModelData::PNT, reinterpret_cast<UINT8*>(vertices), vertexCount);
	UINT16* indices = NEW UINT16[ind->size()];
	memcpy(indices, &(*ind)[0], ind->size() * sizeof(UINT16));
	md->setIndices(Renderable::INDEX_TYPE_USHORT, indices, ind->size());
	md->getParts().resize(1);
	md->getParts()[0].indexCount_ = ind->size();
	md->getParts()[0].offset_ = 0;
	md->setBoundingVolume(NEW BoundingBox(Vec3(
		(float) map->getWidth(), height, (float) map->getHeight())));
	md->setCullFace(false);
	delete ind;
	delete vert;
}

void ModelFactory::createWater(ModelData* md, Attributes& attr, ServiceLocator* sl) {
	int rows = toInt(attr.getString(Resource::ATTR_ROWS).c_str());
	int cols = toInt(attr.getString(Resource::ATTR_COLS).c_str());
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
	md->setVertices(ModelData::PT, reinterpret_cast<UINT8*>(vertices), vertexCount);
	md->getParts().resize(1);
	md->getParts()[0].indexCount_ = (UINT32) ind.size();
}