#include <stdexcept>
#include <iostream>

#include "Entity.h"
#include "system/EntityManager.h"

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
    component->SetShader(shader);
    components.push_back(component);
}

const std::vector<Component*>& Entity::GetComponents() const
{
	return components;
}

void Entity::Compute()
{
    BindingTransform();

    for (Component* c : components)
        c->Compute();
}

#pragma endregion

#pragma region Private Methods

void Entity::BindingTransform() const
{
    // binding transform data
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transform->Position);

    float angleX = glm::radians(transform->Rotation.x);
    float angleY = glm::radians(transform->Rotation.y);
    float angleZ = glm::radians(transform->Rotation.z);

    model = glm::rotate(model, angleX, glm::vec3(1.0f, 0.f, 0.f));
    model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.f, 0.f));
    model = glm::rotate(model, angleZ, glm::vec3(0.0f, 0.f, 1.f));

    model = glm::scale(model, transform->Scale);

    shader->SetMat4("model", model);
}

#pragma endregion