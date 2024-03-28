#pragma once

#include <vector>

#include "Entity.h"

#define MAX_LIGHTS 8

class EntityManager
{
public:	
	~EntityManager();

	// singleton
	static void CreateInstance(Shader* shader);
	static void DestroyInstance();
	static EntityManager* Get();

	void ComputeEntities() const;
	const unsigned int GetNumberOfTriangles() const;

	void RegisterEntity(Entity* e);
	void UnregisterEntity(Entity* e);

	unsigned int GetLightIndex(Transform* transform) const;
	void UpdateLightsIndex();

	const std::vector<Entity*>& GetEntities() const;

private:
	// singleton
	static EntityManager* instance;

	Shader* shader = nullptr;

	std::vector<Entity*> entities = {};

	int lightsCount = 0;
};