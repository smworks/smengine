#ifndef MODELFACTORY_H_
#define MODELFACTORY_H_

#include "Resource.h"
#include "Renderable.h"
class BoundingVolume;
class TextureRGBA;
class Vec3;
class ModelData;

#define GHOST_TERRAIN_HEIGHT_MAP "heightMap"

class ModelFactory {
public:
	static ModelData* create(string model, Attributes& attr, ServiceLocator* sl);
private:
	static void createModel(ModelData* md, Attributes& attr, ServiceLocator* sl);
	static void createTerrain(ModelData* md, Attributes& attr, ServiceLocator* sl);
	static void createPlane(ModelData* md, Attributes& attr, ServiceLocator* sl);
	static void createSphere(ModelData* md, Attributes& attr);
	static void createShape(ModelData* md, Attributes& attr, ServiceLocator* sl);
	static void createWater(ModelData* md, Attributes& attr, ServiceLocator* sl);
private:
	UINT32 cbo_; // Combined buffer object.
	UINT32 ibo_; // Index buffer object.
	SIZE currentPart_;
};

#endif
