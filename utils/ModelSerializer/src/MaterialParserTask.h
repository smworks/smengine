#ifndef MATERIALPARSER_H_
#define MATERIALPARSER_H_

#include "../../../ghost/Multiplatform/Ghost.h"
#include "../../../ghost/Thread.h"
#include "../../../ghost/ServiceProvider.h"

struct MaterialIndex;
class ModelData;

class MaterialParserTask : public Task, public ServiceProvider
{
public:
	explicit MaterialParserTask(ServiceLocator* sl, string data,
		MaterialIndex& matInd, ModelData& modelData,
		vector<MaterialIndex>& matIndices);

	void run() override;
	bool parseMaterial(string file) const;
private:
	string data;
	MaterialIndex& matInd;
	ModelData& modelData;
	vector<MaterialIndex>& matIndices;
};

#endif
