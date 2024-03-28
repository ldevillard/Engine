#include <stdexcept>
#include <iostream>
#include <glm/gtc/quaternion.hpp>

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
    shader->Use();

    // binding transform data
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transform->Position);

    // get the rotation quaternion
    glm::quat rotationQuaternion = transform->GetRotationQuaternion();
    // convert the quaternion to a rotation matrix
    glm::mat4 rotationMatrix = glm::mat4_cast(rotationQuaternion);
    // apply the rotation matrix to the model matrix
    model *= rotationMatrix;

    model = glm::scale(model, transform->Scale);

    shader->SetMat4("model", model);
}

#pragma endregion