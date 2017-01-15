#include "../../ghost/Multiplatform/Ghost.h"
#include "../../ghost/Multiplatform/Win32/WindowsServiceLocator.h"
#include "../../ghost/ScriptManager.h"
#include "../../ghost/Input.h"
#include "../../ghost/Environment.h"
#include "../../ghost/Camera.h"
#include "../../ghost/NodeManager.h"
#include "../../ghost/SceneManager.h"
#include "../../ghost/ScenarioManager.h"
#include "../../ghost/PhysicsManager.h"
#include "../../ghost/ResourceManager.h"
#include "../../ghost/TextManager.h"
#include "../../ghost/GUIManager.h"
#include "../../ghost/ThreadManager.h"
#include "../../ghost/Network/NetworkManager.h"
#include "../../ghost/TextureAtlas.h"
#include "../../ghost/Multiplatform/Win32/WindowsFileManager.h"
#include "../../ghost/Resources/Renderable.h"

#include "ObjParser.h"
#include <iostream>

ServiceLocator* getServiceLocator() {
	WindowsServiceLocator* wsl = new WindowsServiceLocator();
	wsl->setScreenWidth(800);
	wsl->setScreenHeight(600);
	wsl->provide(NEW ScriptManager());
	wsl->provide(NEW Input());
	wsl->provide(NEW Environment());
	wsl->provide(NEW Camera(wsl->getDB()));
	wsl->provide(NEW NodeManager(wsl));
	wsl->provide(NEW SceneManager(wsl));
	wsl->provide(NEW ScenarioManager(wsl));
	wsl->provide(NEW PhysicsManager(wsl));
	wsl->provide(NEW ResourceManager(wsl));
	wsl->provide(NEW TextManager(wsl));
	wsl->provide(NEW GUIManager(wsl));
	wsl->provide(NEW ThreadManager(wsl));
	wsl->provide(NEW NetworkManager(wsl));
	PROFILE("Finished creating engine object.");
	return wsl;
}

bool areColorsEqual(Color left, Color right) {
	return left.getA() == right.getA() && left.getR() == right.getR()
		&& left.getG() == right.getG() && left.getB() == right.getB();
}

int main() {
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif

	ServiceLocator* sl = getServiceLocator();
	cout << "Enter model file to convert: ";
	string file;
	getline(cin, file);
	cout << "Loading file: " << file.c_str() << endl;
	ModelData md;
	ObjParser parser;
	if (parser.parse(md, file, sl)) {
		md.serializeToFile(file + ".sm");
		cout << "Successfully serialized model" << endl;

		ModelData md2;
		char* data;
		SIZE size;
		sl->getFileManager()->loadRaw(data, size, (file + ".sm").c_str());
		md2.deserialize(sl, data);
		delete[] data;

		cout << "Starting verification" << endl;

		ASSERT(md.getBoundingVolume() != 0, "Bounding volume on original model can not be null");
		ASSERT(md2.getBoundingVolume() != 0, "Bounding volume on serialized model can not be null");
		ASSERT(md.getBoundingVolume()->getType() == md2.getBoundingVolume()->getType(), "Bounding volume does not match");
		ASSERT(md.getIndexCount() == md2.getIndexCount(), "Index count does not match");
		ASSERT(md.getIndexStride() == md2.getIndexStride(), "Index stride does not match");
		ASSERT(md.getIndexType() == md2.getIndexType(), "Index type does not match");
		ASSERT(md.getMaterials().size() == md2.getMaterials().size(), "Material size does not match");
		ASSERT(md.getNormalOffset() == md2.getNormalOffset(), "Normal offset does not match");
		ASSERT(md.getParts().size() == md2.getParts().size(), "Part size does not match");
		ASSERT(md.getPosOffset() == md2.getPosOffset(), "Position offset does not match");
		ASSERT(md.getUVOffset() == md2.getUVOffset(), "UV offset does not match");
		ASSERT(md.getVertexCount() == md2.getVertexCount(), "Vertex count does not match");
		ASSERT(md.getVertexStride() == md2.getVertexStride(), "Vertex stride does not match");
		ASSERT(md.getVertexType() == md2.getVertexType(), "Vertex type does not match");
		ASSERT(memcmp(md.getVertices(), md2.getVertices(), md.getVertexCount() * sizeof(float)) == 0, "Vertices array does not match");
		ASSERT(md.areFacesCulled() == md2.areFacesCulled(), "Face culled flag does not match");
		ASSERT(md.hasNormals() == md2.hasNormals(), "Has normals flag does not match");
		ASSERT(md.hasUV() == md2.hasUV(), "Has UV flag does not match");

		if (md.getIndexType() == Renderable::INDEX_TYPE_UINT) {
			ASSERT(memcmp(md.getIndicesInt(), md2.getIndicesInt(), md.getIndexCount() * sizeof(UINT32)) == 0, "Indices int array does not match");
		}
		else {
			ASSERT(memcmp(md.getIndicesShort(), md2.getIndicesShort(), md.getIndexCount() * sizeof(UINT16)) == 0, "Indices short array does not match");
		}
		
		for (SIZE i = 0; i < md.getMaterials().size(); i++) {
			ModelData::Material& left = md.getMaterials()[i];
			ModelData::Material& right = md2.getMaterials()[i];
			ASSERT(areColorsEqual(left.ambient_, right.ambient_), "Ambient does not match. Iteration: " + i);
			ASSERT(areColorsEqual(left.diffuse_, right.diffuse_), "Diffuse does not match. Iteration: " + i);
			ASSERT(strcmp(left.name, right.name) == 0, "Name does not match. Iteration: " + i);
			ASSERT(left.specIntensity_ == right.specIntensity_, "Specular intensity does not match. Iteration: " + i);
			ASSERT(areColorsEqual(left.specular_, right.specular_), "Specular does not match. Iteration: " + i);
			ASSERT(left.texture_ == right.texture_, "Texture does not match. Iteration: " + i);
			ASSERT(left.transparency_ == right.transparency_, "Transparency does not match. Iteration: " + i);
		}

		for (SIZE i = 0; i < md.getParts().size(); i++) {
			ModelData::Part& left = md.getParts()[i];
			ModelData::Part& right = md2.getParts()[i];
			ASSERT(left.indexCount_ == right.indexCount_, "Index count does not match. Iteration: " + i);
			ASSERT(left.material_ == right.material_, "Material index does not match. Iteration: " + i);
			ASSERT(left.offset_ == right.offset_, "Material index does not match. Iteration: " + i);
		}

		if (md.getIndexType() == Renderable::INDEX_TYPE_USHORT) {
			UINT16* ind1 = md.getIndicesShort();
			UINT16* ind2 = md2.getIndicesShort();
			for (SIZE i = 0; i < md.getIndexCount(); i++) {
				LOGI("%d = %d", ind1[i], ind2[i]);
			}
		}

		//float* vert1 = (float*) md.getVertices();
		//float* vert2 = (float*) md2.getVertices();
		//for (SIZE i = 0; i < md.getVertexCount() * 3; i += 3) {
		//	LOGI("% 04.2f % 04.2f % 04.2f vs % 04.2f % 04.2f % 04.2f", vert1[i], vert1[i + 1], vert1[i + 2], vert2[i], vert2[i + 1], vert2[i + 2]);
		//}

		cout << "Finished verification" << endl;
	}
	else {
		cout << "Problem converting model. Check warning log." << endl;
	}

	sl->release();
	delete sl;
	return 0;
}