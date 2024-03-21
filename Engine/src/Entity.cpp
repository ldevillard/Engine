#include <stdexcept>
#include <iostream>

#include "Entity.h"
#include "system/EntityManager.h"

#pragma region Public Methods

Entity::Entity()
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

void Entity::AddComponent(Component component)
{
    component.SetTransform(transform);
    components.push_back(component);
}

const Component* Entity::GetComponent(Component::Type type) const
{
    auto it = std::find_if(components.begin(), components.end(), [type](const Component& comp) {
        return comp.GetType() == type;
        });

    if (it != components.end()) {
        return &(*it); // component pointer
    }
    else {
        return nullptr; // component isn't in the list
    }
}

#pragma endregion