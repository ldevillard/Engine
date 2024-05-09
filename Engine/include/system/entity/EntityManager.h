#pragma once

#include <vector>

#include "Entity.h"
#include "component/Model.h"

#define MAX_LIGHTS 8

class EntityManager
{
public:	
	~EntityManager();

	// singleton
	static void CreateInstance(Shader* shader);
	static void DestroyInstance();
	static EntityManager* Get();

	Entity* CreateEntity(const std::string& name);
	void DestroyEntity(Entity* entity);
	Entity* DuplicateEntity(Entity* entity);

	void ComputeEntities() const;
	void ComputeSelectedEntity() const;
	const unsigned int GetNumberOfTriangles() const;

	unsigned int GetLightIndex(Transform* transform) const;
	void UpdateLightsIndex();

	const std::vector<Entity*>& GetEntities() const;
	const std::vector<Model*> GetModels() const;
	const std::string GenerateNewEntityName(const std::string& prefix) const;

private:
	// singleton
	static EntityManager* instance;

	void registerEntity(Entity* e);
	std::vector<Entity*>::iterator unregisterEntity(Entity* e);

	Shader* shader = nullptr;

	std::vector<Entity*> entities = {};

	int lightsCount = 0;
};