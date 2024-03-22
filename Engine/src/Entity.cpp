#include <stdexcept>
#include <iostream>

#include "Entity.h"
#include "system/EntityManager.h"

#pragma region Public Methods

Entity::Entity(const std::string &name) :
    Name(name)
{
	transform = new Transform();

    EntityManager* manager = EntityManager::Get();
    if (manager == nullptr)
    {
        throw std::runtime_error("FATAL: try to create entity before the manager is initialized!");
    }

    manager->RegisterEntity(this);
}

Entity::~Entity()
{
    EntityManager::Get()->UnregisterEntity(this);

	delete transform;
}

void Entity::AddComponent(Component* component)
{
    component->SetTransform(transform);
    components.push_back(component);
}

void Entity::Compute()
{
    for (Component* c : components)
        c->Compute();
}

#pragma endregion