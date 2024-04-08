#include <iostream>
#include <algorithm>

#include "system/entity/EntityManager.h"
#include "system/editor/Editor.h"
#include "component/Model.h"
#include "component/Light.h"

// singleton instance
EntityManager* EntityManager::instance = nullptr;

#pragma region Public Methods

EntityManager::~EntityManager()
{
	std::for_each(entities.begin(), entities.end(),
		[this](Entity* e) { UnregisterEntity(e); });
	entities.clear();
}

void EntityManager::CreateInstance(Shader* shader)
{
	if (instance == nullptr)
	{
		instance = new EntityManager;
		instance->shader = shader;
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
	shader->Use();
	shader->SetInt("lightsCount", lightsCount);

	for (Entity* e : entities)
	{
		e->Compute();
	}

	const Entity* selectedEntity = Editor::Get()->GetSelectedEntity();
	if (selectedEntity != nullptr)
	{
		selectedEntity->ComputeOutline();
	}
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

	// refresh lights index for shader binding
	UpdateLightsIndex();
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
		std::cerr << "Couldn't not find Entity to unregister! Maybe Entity has already been unregistered!" << std::endl;

	// refresh lights index for shader binding
	UpdateLightsIndex();
}

unsigned int EntityManager::GetLightIndex(Transform* transform) const
{
	unsigned int index = 0;

	for (Entity* e : entities)
	{
		Light* light = nullptr;
		if (e->TryGetComponent<Light>(light))
		{
			if (e->transform == transform)
				return index;
		}
		index++;
		if (index >= MAX_LIGHTS)
		{
			std::cerr << "Too many lights in the scene!" << std::endl;
		}
	}

	std::cerr << "Couldn't find the light index!" << std::endl;

	return 0;
}

void EntityManager::UpdateLightsIndex()
{
	unsigned int index = 0;

	for (Entity* e : entities)
	{
		Light* light = nullptr;
		if (e->TryGetComponent<Light>(light))
		{
			light->SetIndex(index);
			index++;
			if (index >= MAX_LIGHTS)
			{
				std::cerr << "Too many lights in the scene!" << std::endl;
			}
		}
	}

	lightsCount = index;
}

const std::vector<Entity*>& EntityManager::GetEntities() const
{
	return entities;
}

#pragma endregion