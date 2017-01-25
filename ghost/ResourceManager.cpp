#include "ResourceManager.h"
#include "Multiplatform/ServiceLocator.h"

#define CREATE_KEY(name, type) name + "_" + Resource::typeToString(type)

ResourceManager::ResourceManager(ServiceLocator* services) : ServiceProvider(services)
{
	LOGD("Created resource manager.");
}

ResourceManager::~ResourceManager()
{
	reset();
	LOGD("Deleted resource manager.");
}

void ResourceManager::reset()
{
	auto it = resources.begin();
	while (it != resources.end())
	{
		delete it->second;
		++it;
	}
	resources.clear();
}

void ResourceManager::add(const string& name, Resource* resource)
{
	string key = CREATE_KEY(name, resource->getType());
	auto it = resources.find(key);
	if (resources.end() != it)
	{
		LOGD("Resource \"%s\" is already in resource manager.", name.c_str());
		if (resource != it->second)
		{
			LOGW("Resource in resource manager represents different object, so removing it.");
			remove(resource->getType(), name);
		}
	}
	resource->setName(name);
	resources.insert(pair<string, Resource*>(key, resource));
}

bool ResourceManager::has(Resource::Type type, const string& name)
{
	return resources.end() != resources.find(CREATE_KEY(name, type));
}

Resource* ResourceManager::get(Resource::Type type, const string& name)
{
	auto it = resources.find(CREATE_KEY(name, type));
	return it != resources.end() ? it->second : nullptr;
}

void ResourceManager::remove(Resource::Type type, const string& name)
{
	auto it = resources.begin();
	string key = CREATE_KEY(name, type);
	while (it != resources.end())
	{
		if (it->first == key)
		{
			delete it->second;
			resources.erase(it);
			return;
		}
		++it;
	}
}
