﻿#include "../ghost/Multiplatform/Ghost.h"
#include "../ghost/Multiplatform/Win32/WindowsServiceLocator.h"
#include "../ghost/ResourceManager.h"
#include "../ghost/ThreadManager.h"
#include "../ghost/Multiplatform/Win32/WindowsFileManager.h"
#include "../ghost/Resources/Renderable.h"
#include "../ghost/BoundingVolume.h"

#include "../utils/ModelSerializer/src/ObjParser.h"
#include <iostream>
#include <corecrt_io.h>
#include <fcntl.h>

#define ASSERT_EQUAL(first, second, ...) \
		if (first != second) { \
			LOGE(__VA_ARGS__); \
			LOGE("First value: %d, second value: %d", first, second); \
			assert(false); \
		} 

ServiceLocator* getServiceLocator()
{
	auto* wsl = new WindowsServiceLocator();
	wsl->setScreenWidth(800);
	wsl->setScreenHeight(600);
	wsl->provide(NEW ResourceManager(wsl));
	wsl->provide(NEW ThreadManager(wsl));
	PROFILE("Finished creating engine object.");
	return wsl;
}

bool areColorsEqual(Color left, Color right)
{
	return left.getA() == right.getA() && left.getR() == right.getR()
		&& left.getG() == right.getG() && left.getB() == right.getB();
}

bool testModelSerializer()
{
	auto* sl = getServiceLocator();
	ModelData md;
	if (ObjParser(sl).parse(md, "car_body.obj"))
	{
		ModelData md2;
		char* data;
		SIZE size;
		sl->getFileManager()->loadRaw(data, size, "test.sm");
		md2.deserialize(sl, data);
		delete[] data;

		ASSERT(md.getBoundingVolume() != nullptr, "Bounding volume on original model can not be null");
		ASSERT(md2.getBoundingVolume() != nullptr, "Bounding volume on serialized model can not be null");
		ASSERT(md.getBoundingVolume()->getType() == md2.getBoundingVolume()->getType(), "Bounding volume does not match");
		ASSERT_EQUAL(md.getIndexCount(), md2.getIndexCount(), "Index count does not match");
		ASSERT(md.getIndexSize() == md2.getIndexSize(), "Index stride does not match");
		ASSERT(md.getIndexType() == md2.getIndexType(), "Index type does not match");
		ASSERT(md.getMaterials().size() == md2.getMaterials().size(), "Material size does not match");
		ASSERT(md.getNormalOffset() == md2.getNormalOffset(), "Normal offset does not match");
		ASSERT(md.getParts().size() == md2.getParts().size(), "Part size does not match");
		ASSERT(md.getPosOffset() == md2.getPosOffset(), "Position offset does not match");
		ASSERT(md.getUVOffset() == md2.getUVOffset(), "UV offset does not match");
		ASSERT(md.getVertexCount() == md2.getVertexCount(), "Vertex count does not match");
		ASSERT(md.getVertexStride() == md2.getVertexStride(), "Vertex stride does not match");
		ASSERT(md.getVertexType() == md2.getVertexType(), "Vertex type does not match");
		ASSERT(memcmp(md.getVertices(), md2.getVertices(), md.getVertexCount() * sizeof(float)) == 0, "Vertex array does not match");
		ASSERT(md.areFacesCulled() == md2.areFacesCulled(), "Face culled flag does not match");
		ASSERT(md.hasNormals() == md2.hasNormals(), "Has normals flag does not match");
		ASSERT(md.hasUV() == md2.hasUV(), "Has UV flag does not match");

		SIZE sizeOfIndicesInBytes = md.getIndexType() == Renderable::INDEX_TYPE_UINT ? sizeof(UINT32) : sizeof(UINT16);
		ASSERT(memcmp(md.getIndices(), md2.getIndices(), md.getIndexCount() * sizeOfIndicesInBytes) == 0, "Indices int array does not match");

		for (SIZE i = 0; i < md.getMaterials().size(); i++)
		{
			auto& left = md.getMaterials()[i];
			auto& right = md2.getMaterials()[i];
			ASSERT(areColorsEqual(left.ambient_, right.ambient_), "Ambient does not match. Iteration: " + i);
			ASSERT(areColorsEqual(left.diffuse_, right.diffuse_), "Diffuse does not match. Iteration: " + i);
			ASSERT(strcmp(left.name, right.name) == 0, "Name does not match. Iteration: " + i);
			ASSERT(left.specIntensity_ == right.specIntensity_, "Specular intensity does not match. Iteration: " + i);
			ASSERT(areColorsEqual(left.specular_, right.specular_), "Specular does not match. Iteration: " + i);
			ASSERT(left.texture_ == right.texture_, "Texture does not match. Iteration: " + i);
			ASSERT(left.transparency_ == right.transparency_, "Transparency does not match. Iteration: " + i);
		}

		for (SIZE i = 0; i < md.getParts().size(); i++)
		{
			auto& left = md.getParts()[i];
			auto& right = md2.getParts()[i];
			ASSERT(left.indexCount_ == right.indexCount_, "Index count does not match. Iteration: " + i);
			ASSERT(left.material_ == right.material_, "Material index does not match. Iteration: " + i);
			ASSERT(left.offset_ == right.offset_, "Material index does not match. Iteration: " + i);
		}

		if (md.getIndexType() == Renderable::INDEX_TYPE_USHORT)
		{
			auto ind1 = reinterpret_cast<UINT16*>(md.getIndices());
			auto ind2 = reinterpret_cast<UINT16*>(md2.getIndices());
			for (SIZE i = 0; i < md.getIndexCount(); i++)
			{
				LOGI("%d = %d", ind1[i], ind2[i]);
			}
		}

		//float* vert1 = (float*) md.getVertices();
		//float* vert2 = (float*) md2.getVertices();
		//for (SIZE i = 0; i < md.getVertexCount() * 3; i += 3) {
		//	LOGI("% 04.2f % 04.2f % 04.2f vs % 04.2f % 04.2f % 04.2f", vert1[i], vert1[i + 1], vert1[i + 2], vert2[i], vert2[i + 1], vert2[i + 2]);
		//}
	}
	else
	{
		cout << "Problem converting model. Check warning log." << endl;
		return false;
	}

	sl->release();
	delete sl;
	return true;
}

#define TEST(method) \
	wcout << #method << ": " << (method() ? L"☑" : L"☐") << endl

int main()
{
	_setmode(_fileno(stdout), _O_U16TEXT);

	TEST(testModelSerializer);

	system("pause");
	return 0;
}