#include "stdafx.h"
#include "CppUnitTest.h"
#include "../ghost/Multiplatform/ServiceLocator.h"
#include "../ghost/Multiplatform/Win32/WindowsServiceLocator.h"
#include "../ghost/ResourceManager.h"
#include "../ghost/ThreadManager.h"
#include "../utils/ModelSerializer/src/ObjParser.h"
#include "../ghost/Multiplatform/FileManager.h"
#include "../ghost/Resources/Renderable.h"
#include "../ghost/BoundingVolume.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{		
	TEST_CLASS(ModelSerializer)
	{
	public:
		static bool areColorsEqual(Color left, Color right)
		{
			return left.getA() == right.getA() && left.getR() == right.getR()
				&& left.getG() == right.getG() && left.getB() == right.getB();
		}

		TEST_METHOD(parse)
		{
			auto* wsl = new WindowsServiceLocator();
			wsl->setScreenWidth(800);
			wsl->setScreenHeight(600);
			wsl->provide(NEW ResourceManager(wsl));
			wsl->provide(NEW ThreadManager(wsl));

			ModelData md;
			ObjParser objParser(wsl);
			bool result = objParser.parse(md, "car_body.obj");
			
			Assert::IsTrue(result, L"Unable to parse car_body.obj");

			ModelData md2;
			char* data;
			SIZE size;
			wsl->getFileManager()->loadRaw(data, size, "test.sm");
			md2.deserialize(wsl, data);
			delete[] data;

			Assert::IsTrue(md.getBoundingVolume() != nullptr, L"Bounding volume on original model can not be null");
			Assert::IsTrue(md2.getBoundingVolume() != nullptr, L"Bounding volume on serialized model can not be null");
			Assert::IsTrue(md.getBoundingVolume()->getType() == md2.getBoundingVolume()->getType(), L"Bounding volume does not match");
			Assert::IsTrue(md.getIndexCount() == md2.getIndexCount(), L"Index count does not match");
			Assert::IsTrue(md.getIndexStride() == md2.getIndexStride(), L"Index stride does not match");
			Assert::IsTrue(md.getIndexType() == md2.getIndexType(), L"Index type does not match");
			Assert::IsTrue(md.getMaterials().size() == md2.getMaterials().size(), L"Material size does not match");
			Assert::IsTrue(md.getNormalOffset() == md2.getNormalOffset(), L"Normal offset does not match");
			Assert::IsTrue(md.getParts().size() == md2.getParts().size(), L"Part size does not match");
			Assert::IsTrue(md.getPosOffset() == md2.getPosOffset(), L"Position offset does not match");
			Assert::IsTrue(md.getUVOffset() == md2.getUVOffset(), L"UV offset does not match");
			Assert::IsTrue(md.getVertexCount() == md2.getVertexCount(), L"Vertex count does not match");
			Assert::IsTrue(md.getVertexStride() == md2.getVertexStride(), L"Vertex stride does not match");
			Assert::IsTrue(md.getVertexType() == md2.getVertexType(), L"Vertex type does not match");
			Assert::IsTrue(memcmp(md.getVertices(), md2.getVertices(), md.getVertexCount() * sizeof(float)) == 0, L"Vertex array does not match");
			Assert::IsTrue(md.areFacesCulled() == md2.areFacesCulled(), L"Face culled flag does not match");
			Assert::IsTrue(md.hasNormals() == md2.hasNormals(), L"Has normals flag does not match");
			Assert::IsTrue(md.hasUV() == md2.hasUV(), L"Has UV flag does not match");

			if (md.getIndexType() == Renderable::INDEX_TYPE_UINT)
			{
				Assert::IsTrue(memcmp(md.getIndicesInt(), md2.getIndicesInt(), md.getIndexCount() * sizeof(UINT32)) == 0, L"Indices int array does not match");
			}
			else
			{
				Assert::IsTrue(memcmp(md.getIndicesShort(), md2.getIndicesShort(), md.getIndexCount() * sizeof(UINT16)) == 0, L"Indices short array does not match");
			}

			for (SIZE i = 0; i < md.getMaterials().size(); i++)
			{
				auto& left = md.getMaterials()[i];
				auto& right = md2.getMaterials()[i];
				Assert::IsTrue(areColorsEqual(left.ambient_, right.ambient_), L"Ambient does not match. Iteration: " + i);
				Assert::IsTrue(areColorsEqual(left.diffuse_, right.diffuse_), L"Diffuse does not match. Iteration: " + i);
				Assert::IsTrue(strcmp(left.name, right.name) == 0, L"Name does not match. Iteration: " + i);
				Assert::IsTrue(left.specIntensity_ == right.specIntensity_, L"Specular intensity does not match. Iteration: " + i);
				Assert::IsTrue(areColorsEqual(left.specular_, right.specular_), L"Specular does not match. Iteration: " + i);
				Assert::IsTrue(left.texture_ == right.texture_, L"Texture does not match. Iteration: " + i);
				Assert::IsTrue(left.transparency_ == right.transparency_, L"Transparency does not match. Iteration: " + i);
			}

			for (SIZE i = 0; i < md.getParts().size(); i++)
			{
				auto& left = md.getParts()[i];
				auto& right = md2.getParts()[i];
				Assert::IsTrue(left.indexCount_ == right.indexCount_, L"Index count does not match. Iteration: " + i);
				Assert::IsTrue(left.material_ == right.material_, L"Material index does not match. Iteration: " + i);
				Assert::IsTrue(left.offset_ == right.offset_, L"Material index does not match. Iteration: " + i);
			}

			if (md.getIndexType() == Renderable::INDEX_TYPE_USHORT)
			{
				auto* ind1 = md.getIndicesShort();
				auto* ind2 = md2.getIndicesShort();
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

			wsl->release();
			delete wsl;
		}

	};
}