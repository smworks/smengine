/*
 * ObjParser.cpp
 *
 *  Created on: 2012.06.25
 *      Author: MS
 */

#include "ObjParser.h"
#include "Multiplatform/Ghost.h"
#include "Multiplatform/ServiceLocator.h"
#include "Thread.h"
#include "Multiplatform/FileManager.h"
#include "Multiplatform/GraphicsManager.h"
#include "ResourceManager.h"
#include "BoundingSphere.h"
#include "BoundingBox.h"
#include "Utils.h"
#include "Resources/Texture.h"
#include "Resources/Renderable.h"
#include "HashMap.h"

struct UniqueKey {
	UINT32 buffer[3];
	bool operator==(const UniqueKey& r) const {
		return buffer[0] == r.buffer[0] &&
			buffer[1] == r.buffer[1] &&
			buffer[2] == r.buffer[2];
	}
	bool operator<(const UniqueKey& r) const {
		if (buffer[0] < r.buffer[0]) {
			return true;
		}
		else if (buffer[0] == r.buffer[0]) {
			if (buffer[1] < r.buffer[1]) {
				return true;
			}
			else if (buffer[1] == r.buffer[1]) {
				return buffer[2] < r.buffer[2];
			}
			return false;
		}
		return false;

	}
};

struct UniqueKeyHash {
	size_t operator()(const UniqueKey& key) const {
		size_t h = 0;
		for (UINT32 i = 0; i < 3; i++) {
			h = 5 * h + key.buffer[i];
		}
		return size_t(h);
	}
};

class PositionParser : public Task {
public:
	PositionParser(
		const char* data, UINT8* vertices, UINT32 offset, UINT32 size,
		Vec3* vmin, Vec3* vmax, Vec3* radius, float& maxVertexPos) :
		data_(data),
		vertices_(vertices),
		vmin_(vmin),
		vmax_(vmax),
		radius_(radius),
		offset_(offset),
		size_(size),
		maxVertexPos_(maxVertexPos)
	{}

	void run() {
		SIZE lineEnd = 0;
		SIZE pos = 0;
		SIZE lineLength = 255;
		char* line = NEW char[lineLength];
		UINT32 index = 0;
		float posVec[3];
		float adjustRatio = 1.0f / maxVertexPos_;
		while (true) {
			const char* found = strchr(data_ + pos, GHOST_NEWLINE);
			if (!found) {
				break;
			}
			lineEnd = found - data_;
			if (data_[pos] == 'v') {
				if (data_[pos + 1] == ' ') {
					bool one = true;
					SIZE offset = 0;
					if (lineEnd - pos > lineLength) {
						lineLength = lineEnd - pos;
						delete [] line;
						line = NEW char[lineLength];
					}
					memcpy(line, data_ + pos, lineEnd - pos);
					for (SIZE i = 2; i < lineEnd; i++) {
						if (line[i] == ' ') {
							if (one) {
								//posVec[0] = toFloat(line + 2, i - 2) * adjustRatio;
								posVec[0] = toFloat(line + 2) * adjustRatio;
								one = false;
								offset = i;
							}
							else {
								//posVec[1] = toFloat(line + offset + 1, i - offset - 1) * adjustRatio;
								posVec[1] = toFloat(line + offset + 1) * adjustRatio;
								offset = i;
								//posVec[2] = toFloat(line + offset + 1, lineEnd - pos - offset - 1) * adjustRatio;
								posVec[2] = toFloat(line + offset + 1) * adjustRatio;
								break;
							}
						}
					}
					memcpy(&vertices_[index++ * size_ + offset_], &posVec[0], sizeof(float) * 3);
					static Vec3 tmp;
					tmp.setXYZ(posVec[0], posVec[1], posVec[2]);
					if (tmp.length() > radius_->length()) {
						radius_->setXYZ(tmp);
					}
					if (vmin_->getX() > posVec[0]) {
						vmin_->setX(posVec[0]);
					}
					if (vmin_->getY() > posVec[1]) {
						vmin_->setY(posVec[1]);
					}
					if (vmin_->getZ() > posVec[2]) {
						vmin_->setZ(posVec[2]);
					}
					if (vmax_->getX() < posVec[0]) {
						vmax_->setX(posVec[0]);
					}
					if (vmax_->getY() < posVec[1]) {
						vmax_->setY(posVec[1]);
					}
					if (vmax_->getZ() < posVec[2]) {
						vmax_->setZ(posVec[2]);
					}
				}
			}
			pos = lineEnd + 1;
		}
		delete [] line;
	};
private:
	const char* data_;
	UINT8* vertices_;
	Vec3* vmin_;
	Vec3* vmax_;
	Vec3* radius_;
	UINT32 offset_, size_;
	float maxVertexPos_;
};

class NormalParser : public Task {
public:
	NormalParser(const char* data, UINT8* vertices, UINT32 offset, UINT32 size) :
		data_(data),
		vertices_(vertices),
		offset_(offset),
		size_(size)
	{}

	void run() {
		SIZE lineEnd = 0;
		SIZE pos = 0;
		SIZE lineLength = 255;
		char* line = NEW char[lineLength];
		UINT32 index = 0;
		float normVec[3];
		while (true) {
			const char* found = strchr(data_ + pos, GHOST_NEWLINE);
			if (!found) {
				break;
			}
			lineEnd = found - data_;
			if (data_[pos] == 'v') {
				if (data_[pos + 1] == 'n') {
					if (lineEnd - pos > lineLength) {
						lineLength = lineEnd - pos;
						delete [] line;
						line = NEW char[lineLength];
					}
					bool one = true;
					SIZE offset = 0;
					memcpy(line, data_ + pos, lineEnd - pos);
					for (SIZE i = 3; i < lineEnd; i++) {
						if (line[i] == ' ') {
							if (one) {
								normVec[0] = toFloat(line + 2); //, i - 2);
								one = false;
								offset = i;
							}
							else {
								normVec[1] = toFloat(line + offset + 1);//, i - offset - 1);
								offset = i;
								normVec[2] = toFloat(line + offset + 1);//, lineEnd - pos - offset - 1);
								break;
							}
						}
					}
					memcpy(&vertices_[index++ * size_ + offset_], &normVec[0], sizeof(float) * 3);
				}
			}
			pos = lineEnd + 1;
		}
		delete [] line;
	};
private:
	const char* data_;
	UINT8* vertices_;
	UINT32 offset_, size_;
};

class UVParser : public Task {
public:
	UVParser(const char* data, UINT8* vertices, UINT32 offset, UINT32 size) :
		data_(data),
		vertices_(vertices),
		offset_(offset),
		size_(size)
	{}

	void run() {
		SIZE lineEnd = 0;
		SIZE pos = 0;
		SIZE lineLength = 255;
		char* line = NEW char[lineLength];
		UINT32 index = 0;
		float uvVec[2];
		while (true) {
			const char* found = strchr(data_ + pos, GHOST_NEWLINE);
			if (!found) {
				break;
			}
			lineEnd = found - data_;
			if (data_[pos] == 'v') {
				if (data_[pos + 1] == 't') {
					if (lineEnd - pos > lineLength) {
						lineLength = lineEnd - pos;
						delete [] line;
						line = NEW char[lineLength];
					}
					memcpy(line, data_ + pos, lineEnd - pos);
					for (SIZE i = 3; i < lineEnd; i++) {
						if (line[i] == ' ') {
							uvVec[0] = toFloat(line + 2);//, i - 2);
							uvVec[1] = toFloat(line + i + 1);//, lineEnd - pos - i - 1);
							break;
						}
					}
					memcpy(&vertices_[index++ * size_ + offset_], &uvVec[0], sizeof(float) * 2);
				}
			}
			pos = lineEnd + 1;
		}
		delete [] line;
	};
private:
	const char* data_;
	UINT8* vertices_;
	UINT32 offset_, size_;
};


class FaceParser : public Task {
public:
	FaceParser(const char* data, vector<ObjParser::Face>* faces) :
		data_(data),
		faces_(faces) {}

	void run() {
		SIZE lineEnd = 0;
		SIZE pos = 0;
		SIZE faceCount = 0;
		while (true) {
			const char* found = strchr(data_ + pos, GHOST_NEWLINE);
			if (!found) {
				break;
			}
			lineEnd = found - data_;
			if (data_[pos] == 'f') {
				if (!ObjParser::parseFace(faces_->at(faceCount++), data_ + pos + 2, lineEnd - pos - 2)) {
					LOGW("Error on line: %s", string(data_ + pos, lineEnd - pos - 2).c_str());
				}
			}
			pos = lineEnd + 1;
		}
	};
private:
	const char* data_;
	vector<ObjParser::Face>* faces_;
};

bool ObjParser::parse(ModelData& model, const string& file, ServiceLocator* services) {
	PROFILE("Started loading object: %s.", file.c_str());
	ThreadManager* tm = services->getThreadManager();
	GraphicsManager* gm = services->getGraphicsManager();
	Vec3 vmin(FLT_MAX), vmax(FLT_MIN), radius(0.0f);
    //vector<Material> materials;
	vector<MaterialIndex> matIndices;
	vector<ModelData::Material>& materials = model.getMaterials();
	string obj;
	services->getFileManager()->loadText(obj, (GHOST_MODELS + file).c_str());
	if (obj.length() == 0) {
		LOGW("Unable to load file %s, "
			"or its content is empty.",
			file.c_str());
		return false;
	}
	PROFILE("Loaded raw file byte array.");
	float maxVertexPos;
	UINT32 nVertices = 0,
		nNormals = 0,
		nUvCoordinates = 0,
		nFaces = 0;
	countComponents(
		obj,
		nVertices,
		nNormals,
		nUvCoordinates,
		nFaces,
		maxVertexPos);
	LOGD("Size adjust ratio: %f.", 1.0f / maxVertexPos);
	bool hasUV = nUvCoordinates > 0;
	bool hasNormals = nNormals > 0;
	if (nFaces * 3 > UINT_MAX) {
		LOGW("3D model cannot have more indices than: %d.", UINT_MAX);
		return false;
	}
	if (nFaces * 3 >= USHRT_MAX && !gm->isSupported(GraphicsManager::SUPPORT_UINT_INDEX)) {
		LOGW("3D model has more indices, than device supports. Supported index count: %d.", USHRT_MAX);
		return false;
	}
	UINT8* vertices;
	ModelData::VertexType vertexType;
	UINT32 posOffset = 0, normOffset = 0, uvOffset = 0, vertexSize;
	if (hasUV && hasNormals) {
		vertices = reinterpret_cast<UINT8*>(NEW VertexPNT[nVertices]);
		vertexSize = sizeof(VertexPNT);
		posOffset = offsetof(VertexPNT, pos);
		normOffset = offsetof(VertexPNT, normals);
		uvOffset = offsetof(VertexPNT, uv);
		vertexType = ModelData::PNT;
	}
	else if (hasUV && !hasNormals) {
		vertices = reinterpret_cast<UINT8*>(NEW VertexPT[nVertices]);
		vertexSize = sizeof(VertexPT);
		posOffset = offsetof(VertexPT, pos);
		uvOffset = offsetof(VertexPT, uv);
		vertexType = ModelData::PT;
	}
	else if (!hasUV && hasNormals) {
		vertices = reinterpret_cast<UINT8*>(NEW VertexPN[nVertices]);
		vertexSize = sizeof(VertexPN);
		posOffset = offsetof(VertexPN, pos);
		normOffset = offsetof(VertexPN, normals);
		vertexType = ModelData::PN;
	}
	else {
		vertices = reinterpret_cast<UINT8*>(NEW VertexP[nVertices]);
		vertexSize = sizeof(VertexP);
		posOffset = offsetof(VertexP, pos);
		vertexType = ModelData::P;
	}
	vector<Face> faces;
	faces.resize(nFaces);
	string line;
	SIZE pos = 0;
	SIZE lineEnd = 0;
	SIZE params = 0;
	MaterialIndex matInd;
	SIZE matOff = 0;
	SIZE matSize = 0;
	vector<string> arr;
	const char* data = obj.c_str();
	UINT64 posThread = 0, normalThread = 0, uvThread = 0, faceThread = 0;
	posThread = tm->execute(NEW PositionParser(
		data, vertices, posOffset, vertexSize, &vmin, &vmax, &radius, maxVertexPos));
	if (hasNormals) {
		normalThread = tm->execute(NEW NormalParser(data, vertices, normOffset, vertexSize));
	}
	if (hasUV) {
		uvThread = tm->execute(NEW UVParser(data, vertices, uvOffset, vertexSize));
	}
	faceThread = tm->execute(NEW FaceParser(data, &faces));
	ModelData::Material defMat;
	defMat.setName("default");
	materials.push_back(defMat);
	while (true) {
		lineEnd = obj.find(GHOST_NEWLINE, pos);
		if (lineEnd == string::npos) {
			break;
		}
		switch (obj[pos]) {
		case 'f': // Faces.
			line = obj.substr(pos, lineEnd - pos);
			stringSplit(line, arr, ' ');
			params = arr.size();
			if (params == 4) {
				matSize++;
			}
			break;
		case 'm': // Material file.
			line = obj.substr(pos, lineEnd - pos);
			stringSplit(line, arr, ' ');
			if (!parseMaterial(model, arr[1], services)) {
				return false;
			}
			break;
		case 'u': // Use material.
			line = obj.substr(pos, lineEnd - pos);
			stringSplit(line, arr, ' ');
			if (materials.size() == 0) {
				LOGW("Material file must be defined first");
			}
			else if (arr.size() < 2) {
				LOGE("usemtl in .obj file has no name.");
			}
			else {
				SIZE size = materials.size();
				bool found = false;
				for (SIZE i = 0; i < size; i++) {
					//if (materials[i].name_ == arr[1]) {
					//	found = true;
					//}
				}
				if (found) {
					if (matSize == 0) { // First element.
						matInd.name_ = arr[1];
						matInd.offset_ = matOff;
					}
					else {
						matInd.size_ = matSize;
						matOff += matSize;
						matIndices.push_back(matInd);
						matInd.name_ = arr[1];
						matInd.offset_ = matOff;
						matSize = 0;
					}
				}
				else {
					LOGW("Material %s not found.", arr[1].c_str());
					if (matSize == 0) { // First element.
						matInd.name_ = "default";
						matInd.offset_ = matOff;
					}
					else {
						matInd.size_ = matSize;
						matOff += matSize;
						matIndices.push_back(matInd);
						matInd.name_ = "default";
						matInd.offset_ = matOff;
						matSize = 0;
					}
				}
			}
			break;
		case 'o': // Object groups.
			break;
		case '#': // Comments.
			break;
		}
		pos = lineEnd + 1;
	}
	tm->join(posThread);
	if (hasNormals) {
		tm->join(normalThread);
	}
	if (hasUV) {
		tm->join(uvThread);
	}
	tm->join(faceThread);
    PROFILE("Finished reading basic data for object: %s.", file.c_str());
	matInd.size_ = matSize;
	matIndices.push_back(matInd);
	// Rearrange faces and material indices, to have as little
	// material changes as possible during render stage.
	if (matIndices.size() > materials.size()) {
		vector<vector<Face> > arrFaces;
		for (UINT32 i = 0; i < materials.size(); i++) {
			vector<Face> fac;
			arrFaces.push_back(fac);
		}
		SIZE size = matIndices.size();
		for (UINT32 i = 0; i < size; i++) {
			for (UINT32 j = 0; j < materials.size(); j++) {
				MaterialIndex& mi = matIndices[i];
			/*	if (mi.name_ == materials[j].name_) {
					for (SIZE k = mi.offset_; k < mi.offset_ + mi.size_; k++) {
						arrFaces[j].push_back(faces[k]);
					}
				}*/
			}
		}
		faces.clear();
		UINT32 offset = 0;
		matIndices.clear();
		for (UINT32 i = 0; i < arrFaces.size(); i++) {
			MaterialIndex mi;
			/*mi.name_ = materials[i].name_;*/
			mi.offset_ = offset;
			mi.size_ = arrFaces[i].size();
			matIndices.push_back(mi);
			for (UINT32 j = 0; j < arrFaces[i].size(); j++) {
				faces.push_back(arrFaces[i][j]);
			}
			offset += mi.size_;
		}
	}
	vector<UINT16>* indexArray = 0;
	vector<UINT32>* indexArrayInt = 0;
	UINT32 indexReserve = 0;
	for (UINT32 i = 0; i < matIndices.size(); i++) {
		indexReserve += matIndices[i].size_ * 3;
	}
	bool useShort = true;
	if (indexReserve >= USHRT_MAX) {
		useShort = false;
		indexArrayInt = NEW vector<UINT32>();
		indexArrayInt->reserve(indexReserve);
	}
	else {
		indexArray = NEW vector<UINT16>();
		indexArray->reserve(indexReserve);
	}
	PROFILE("Started optimizations.");
	vector<float>* vs = NEW vector<float>();
	unordered_map<UniqueKey, int, UniqueKeyHash>* hm = NEW unordered_map<UniqueKey, int, UniqueKeyHash>();
	unordered_map<UniqueKey, int, UniqueKeyHash>::const_iterator it;
	UINT32 floatsInVertex = vertexSize / sizeof(float);
	float* vertex = NEW float[floatsInVertex];
	for (SIZE i = 0; i < nFaces; i++) {
		Face& face = faces[i];
		for (int j = 0; j < Face::FACE_SIZE; j++) {
			UINT32 index = face.indices_[j];
			UniqueKey key;
			key.buffer[0] = index;
			key.buffer[1] = face.normIndices_[j];
			key.buffer[2] = face.texIndices_[j];
			it = hm->find(key);
			if (it != hm->end()) {
				if (useShort) {
					indexArray->push_back(it->second);
				}
				else {
					indexArrayInt->push_back(it->second);
				}
			}
			else {
				UINT32 vInd = 0;
				UINT32 offset = index * vertexSize + posOffset;
				memcpy(&vertex[vInd++], &vertices[offset + 0], sizeof(float));
				memcpy(&vertex[vInd++], &vertices[offset + sizeof(float)], sizeof(float));
				memcpy(&vertex[vInd++], &vertices[offset + 2 * sizeof(float)], sizeof(float));
				if (nNormals > 0) {
					index = face.normIndices_[j];
					offset = index * vertexSize + normOffset;
					memcpy(&vertex[vInd++], &vertices[offset + 0], sizeof(float));
					memcpy(&vertex[vInd++], &vertices[offset + sizeof(float)], sizeof(float));
					memcpy(&vertex[vInd++], &vertices[offset + 2 * sizeof(float)], sizeof(float));
				}
				if (nUvCoordinates > 0) {
					index = face.texIndices_[j];
					offset = index * vertexSize + uvOffset;
					memcpy(&vertex[vInd++], &vertices[offset + 0], sizeof(float));
					memcpy(&vertex[vInd++], &vertices[offset + sizeof(float)], sizeof(float));
				}
				if (useShort) {
					indexArray->push_back(vs->size() / floatsInVertex);
				}
				else {
					indexArrayInt->push_back((UINT32) vs->size() / floatsInVertex);
				}
				hm->insert(pair<UniqueKey, int>(key, vs->size() / floatsInVertex));
				for (SIZE i = 0; i < floatsInVertex; i++) {
					vs->push_back(vertex[i]);
				}
			}
		}
	}
	PROFILE("Finished optimizing. Hash map size: %u.", (UINT32) hm->size());
	delete hm;
	PROFILE("Hash map deleted.");
	delete [] vertex;
	if (useShort) {
		UINT16* indexArr = NEW UINT16[indexArray->size()];
		memcpy(indexArr, &(*indexArray)[0], indexArray->size() * sizeof(UINT16));
		model.setIndices(Renderable::INDEX_TYPE_USHORT, indexArr, indexArray->size());
		LOGD("Indices: %u", (UINT32) indexArray->size());
	}
	else {
		UINT32* indexArr = NEW UINT32[indexArrayInt->size()];
		memcpy(indexArr, &(*indexArrayInt)[0], indexArrayInt->size() * sizeof(UINT32));
		model.setIndices(Renderable::INDEX_TYPE_UINT, indexArr, indexArrayInt->size());
		LOGW("3D model is too large for some mobile devices.");
		LOGD("Indices: %u", (UINT32) indexArrayInt->size());
	}
	delete indexArray;
	delete indexArrayInt;
	void* vertexArray = 0;
	SIZE vertexCount = vs->size() / floatsInVertex;
	if (hasUV && hasNormals) {
		vertexArray = NEW VertexPNT[vertexCount];
	}
	else if (hasUV && !hasNormals) {
		vertexArray = NEW VertexPT[vertexCount];
	}
	else if (!hasUV && hasNormals) {
		vertexArray = NEW VertexPN[vertexCount];
	}
	else {
		vertexArray = NEW VertexP[vertexCount];
	}
	memcpy(vertexArray, &(*vs)[0], vs->size() * sizeof(float));
	model.setVertices(vertexType, reinterpret_cast<UINT8*>(vertexArray), vertexCount);
	LOGD("Vertex count: %u.", (UINT32) vertexCount);
	delete vs;
	delete [] vertices;
	// Create separate model part array.
	vector<ModelData::Part>& parts = model.getParts();
	parts.resize(matIndices.size());
	UINT32 offset = 0;
	for (UINT32 i = 0; i < matIndices.size(); i++) {
		MaterialIndex mi = matIndices[i];
		if (mi.name_.length() > 0) {
			for (UINT32 j = 0; j < materials.size(); j++) {
				/*if (mi.name_ == materials[j].name_) {
					parts[i].material_ = j;
				}*/
			}
		}
		parts[i].offset_ = offset;
		parts[i].indexCount_ = (UINT32) mi.size_ * 3;
		offset += parts[i].indexCount_;
	}
	// Calculate bounding volume.
	float width = vmax.getX() - vmin.getX();
	float height = vmax.getY() - vmin.getY();
	float depth = vmax.getZ() - vmin.getZ();
	float r = radius.length();
	float volumeBox = width * height * depth;
	float volumeSphere = 4.0f / 3.0f * (float) SMART_PI * r * r * r;
	LOGD("Sphere radius: %f. Box width: %f, height: %f, depth: %f.", r, width, height, depth);
	LOGD("Calculating optimal bounding volume for mesh. "
		"Box volume: %f, Sphere volume: %f. Optimal is %s.",
		volumeBox, volumeSphere, volumeSphere < volumeBox ? "sphere" : "box");
	BoundingVolume* bounds = 0;
	if (volumeSphere < volumeBox) {
		bounds = NEW BoundingSphere(radius.length());
	}
	else {
		bounds = NEW BoundingBox(Vec3(width, height, depth));
	}
	model.setBoundingVolume(bounds);
    PROFILE("Finished parsing object: %s.", file.c_str());
	return true;
}

/*
 * Initializes specified Face structure.
 */
bool ObjParser::parseFace(Face& face, const char* line, SIZE length) {
	const char* arr[3];
	arr[0] = line;
	arr[1] = arr[2] = 0;
	SIZE sizeArray[3];
	sizeArray[0] = sizeArray[1] = sizeArray[2] = 0;
	for (SIZE i = 0; i < length; i++) {
		if (line[i] == ' ') {
			if (arr[1] == 0) {
				arr[1] = line + i + 1;
				sizeArray[0] = i;
			}
			else {
				sizeArray[1] = i - sizeArray[0] - 1;
				arr[2] = line + i + 1;
				break;
			}
		}
	}
	if (arr[2] == 0) {
		return false;
	}
	sizeArray[2] = length - (arr[2] - line);
	for (int i = 0; i < Face::FACE_SIZE; i++) {
		SIZE firstSlash = 0, secondSlash = 0;
		for (SIZE j = 0; j < sizeArray[i]; j++) {
			if (arr[i][j] == '/') {
				if (firstSlash == 0) {
					firstSlash = j;
				}
				else {
					secondSlash = j;
					break;
				}
			}
		}
		if (secondSlash != 0) { // Has normals.
			face.normIndices_[i] = toUint(
				arr[i] + secondSlash + 1, sizeArray[i] - secondSlash - 1) - 1;
			if (secondSlash - firstSlash > 1) { // Has UV.
				face.texIndices_[i] = toUint(
					arr[i] + firstSlash + 1, secondSlash - firstSlash - 1) - 1;
			}
		}
		else if (firstSlash != 0) { // Has UV, but no normals.
			face.texIndices_[i] = toUint(
				arr[i] + firstSlash + 1, sizeArray[i] - firstSlash - 1) - 1;
		}
		SIZE count = firstSlash == 0 ? sizeArray[i] : firstSlash;
		face.indices_[i] = toUint(arr[i], count) - 1;
	}
	return true;
}

/*
 * Scans through model content and
 * counts the number of vertices and faces.
 */
void ObjParser::countComponents(
		const string& obj,
		UINT32& vertices,
		UINT32& normals,
		UINT32& uvCoordinates,
		UINT32& faces,
		float& maxVertexPos)
{
	size_t pos = 0;
	const char* data = obj.c_str();
	maxVertexPos = 0.0f;
	float vertexPos;
	while (true) {
		switch (obj[pos]) {
		case 'v':
			switch (obj[pos + 1]) {
			case ' ':
				vertices += 3;
				{
				const char* found = strchr(data + pos + 1, GHOST_NEWLINE);
				if (found) {
					SIZE lineEnd = found - data;
					bool one = true;
					SIZE offset = 0;
					const char* start = data + pos + 2;
					for (SIZE i = 2; i < lineEnd; i++) {
						if (start[i] == ' ') {
							if (one) {
								vertexPos = fabs(toFloat(start));
								if (vertexPos > maxVertexPos) {
									maxVertexPos = vertexPos;
								}
								one = false;
								offset = i;
							}
							else {
								vertexPos = fabs(toFloat(start + offset + 1));
								if (vertexPos > maxVertexPos) {
									maxVertexPos = vertexPos;
								}
								offset = i;
								vertexPos = fabs(toFloat(start + offset + 1));
								if (vertexPos > maxVertexPos) {
									maxVertexPos = vertexPos;
								}
								break;
							}
						}
					}
				}
				}
				break;
			case 'n':
				normals += 3;
				break;
			case 't':
				uvCoordinates += 2;
				break;
			}
			break;
		case 'f': // Faces.
			faces++;
			break;
		case 'o': // Object groups.
			break;
		case '#': // Comments.
			break;
		}
		pos = obj.find(GHOST_NEWLINE, pos);
		if (pos == string::npos) {
			break;
		}
		pos++;
	}
}

/*
 * Loads material data into entity model.
 */
bool ObjParser::parseMaterial(ModelData& model, const string& file, ServiceLocator* services) {
	string obj;
	services->getFileManager()->loadText(obj, (GHOST_MODELS + file).c_str());
	if (obj.length() == 0) {
		LOGW("Material %s not found.", file.c_str());
	}
	model.getMaterials().push_back(ModelData::Material());
	ModelData::Material& mat = model.getMaterials()[model.getMaterials().size() - 1];
	string line;
	vector<string> arr;
	size_t lineEnd = 0, pos = 0;
	while (true) {
		lineEnd = obj.find(GHOST_NEWLINE, pos);
		if (lineEnd == string::npos) {
			break;
		}
		line = obj.substr(pos, lineEnd - pos);
		if (line.length() == 0) {
			pos = lineEnd + 1;
			continue;
		}
		stringSplit(line, arr, ' ');
		if (arr.size() < 2) {
			pos = lineEnd + 1;
			continue;
		}

		switch (obj[pos]) {
		case 'K':
			switch (obj[pos + 1]) {
			case 'a': // Ambient color.
				mat.ambient_.setRGB(
					toFloat(arr[1].c_str()),
					toFloat(arr[2].c_str()),
					toFloat(arr[3].c_str()));
				break;
			case 'd': // Diffuse color.
				mat.diffuse_.setRGB(
					toFloat(arr[1].c_str()),
					toFloat(arr[2].c_str()),
					toFloat(arr[3].c_str()));
				break;
			case 's': // Specular color.
				mat.specular_.setRGB(
					toFloat(arr[1].c_str()),
					toFloat(arr[2].c_str()),
					toFloat(arr[3].c_str()));
				break;
			}
			break;
		case 'N': // Specular color intensity.
			mat.specIntensity_ = toFloat(arr[1].c_str());
			break;
		case 'd': // Transparency.
			mat.transparency_ = toFloat(arr[1].c_str());
			break;
		case 'm': // Texture.
			{
				SIZE index = arr[1].find_last_of("/\\");
				string texture;
				if (index != string::npos) {
					texture = arr[1].substr(index + 1);
				}
				else {
					texture = arr[1];
				}
				if (texture.length() > 0) {
					mat.texture_ = Texture::load(services, texture);
				}
			}
			break;
		case 'n':
			if (arr.size() < 2) {
				LOGE("Used materials has no name.");
				return false;
			}
			else {
				mat.setName(arr[1]);
			}
			break;
		}
		pos = lineEnd + 1;
	}
	return true;
}
