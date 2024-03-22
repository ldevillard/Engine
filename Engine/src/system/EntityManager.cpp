#include <iostream>

#include "system/EntityManager.h"
#include "component/Model.h"

// singleton instance
EntityManager* EntityManager::instance = nullptr;

#pragma region Public Methods

void EntityManager::CreateInstance()
{
	if (instance == nullptr)
	{
		instance = new EntityManager;
	}
}

void EntityManager::DestroyInstance()
{
	if (instance != nullptr)
	{
		delete instance;
		instance = nullptr;
	}
}

EntityManager* EntityManager::Get()
{
	return instance;
}

void EntityManager::ComputeEntities() const
{
	for (Entity* e : entities)
		e->Compute();
}

const unsigned int EntityManager::GetNumberOfTriangles() const
{
	unsigned int sum = 0;

	for (Entity* e : entities)
	{
		Model* model = nullptr;
		if (e->TryGetComponent<Model>(model))
		{
			sum += model->GetNumberOfTriangles();
		}
	}

	return sum;
}

void EntityManager::RegisterEntity(Entity* e) 
{
	if (e == nullptr)
	{
		std::cerr << "Try to register a null Entity" << std::endl;
		return;
	}

	auto it = std::find(entities.begin(), entities.end(), e);

	if (it == entities.end())
		entities.push_back(e);
	else
		std::cerr << "Entity is already registered!" << std::endl;
}

void EntityManager::UnregisterEntity(Entity* e) 
{
	if (e == nullptr)
	{
		std::cerr << "Try to unregister a null Entity" << std::endl;
		return;
	}

	auto it = std::find(entities.begin(), entities.end(), e);

	if (it != entities.end())
		entities.erase(it);
	else
		std::cerr << "Couldn't not find Entity to unregister!" << std::endl;
}

#pragma endregion