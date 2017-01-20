#ifndef MATERIALPARSER_H_
#define MATERIALPARSER_H_

#include "../../../ghost/Multiplatform/Ghost.h"
#include "../../../ghost/Thread.h"
#include "../../../ghost/ServiceProvider.h"

struct RawObject;
struct MaterialIndex;
class ModelData;

class MaterialParserTask : public Task, public ServiceProvider
{
public:
	explicit MaterialParserTask(ServiceLocator* sl, string data,
		RawObject& rawObject, ModelData& modelData);

	void run() override;
	bool parseMaterial(string file) const;
private:
	string data;
	RawObject& rawObject;
	ModelData& modelData;
};

#endif
