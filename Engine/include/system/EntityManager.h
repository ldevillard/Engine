#pragma once

#include <vector>

#include "Entity.h"

class EntityManager
{
public:	
	// singleton
	static void CreateInstance();
	static void DestroyInstance();
	static EntityManager* Get();

	void ComputeEntities() const;
	const unsigned int GetNumberOfTriangles() const;

	void RegisterEntity(Entity* e);
	void UnregisterEntity(Entity* e);

private:
	// singleton
	static EntityManager* instance;

	std::vector<Entity*> entities = {};
};