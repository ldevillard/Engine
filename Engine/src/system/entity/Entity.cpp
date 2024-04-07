#include <stdexcept>
#include <iostream>
#include <maths/glm/gtc/quaternion.hpp>

#include "system/entity/Entity.h"
#include "system/entity/EntityManager.h"

#pragma region Public Methods

Entity::Entity(const std::string &name, Shader* sh) :
    shader(sh),
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
    if (EntityManager::Get())
    {
        EntityManager::Get()->UnregisterEntity(this);
    }
	delete transform;
}

void Entity::AddComponent(Component* component)
{
    component->SetTransform(transform);
    component->SetEntity(this);
    component->SetShader(shader);
    components.push_back(component);

    EntityManager::Get()->UpdateLightsIndex();
}

const std::vector<Component*>& Entity::GetComponents() const
{
	return components;
}

void Entity::Compute()
{
    transform->Compute(shader);
    for (Component* c : components)
        c->Compute();
}

#pragma endregion