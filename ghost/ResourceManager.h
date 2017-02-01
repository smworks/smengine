#ifndef SCENELOADER_H_
#define SCENELOADER_H_

#include "Multiplatform/Ghost.h"
#include "Resources/Resource.h"
#include "ServiceProvider.h"

class ServiceLocator;

class ResourceManager : public ServiceProvider
{
public:
	explicit ResourceManager(ServiceLocator* services);
	~ResourceManager();

	/**
	 * Deletes all resources and sets resource
	 * manager into default state.
	 */
	void reset();

	/**
	 * Adds resouce to memory.
	 * @param name - name of the resource.
	 * @param resource - pointer to resource
	 * object.
	 */
	void add(const string& name, Resource* resource);

	/**
	 * Checks if required resource is
	 * available.
	 * @param type - type of resource.
	 * @param name - name of the resource.
	 * @return True if resource is available.
	 */
	bool has(Resource::Type type, const string& name);

	/**
	 * Returns pointer to resource.
	 * @param type - type of resource.
	 * @param name - name of the resource.
	 * @return Pointer to required resource or
	 * 0 if resource not found.
	 */
	Resource* get(Resource::Type type, const string& name);

	/**
	 * Removes resource that is
	 * no longer required.
	 * @param type - type of resource.
	 * @param name - name of the resource.
	 */
	void remove(Resource::Type type, const string& name);
private:
	unordered_map<string, Resource*> resources;
};

#endif
