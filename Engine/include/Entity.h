#pragma once

#include <vector>
#include <string>

#include "data/Transform.h"
#include "component/Component.h"

class Entity
{
public:
	Entity(const std::string &name);
	~Entity();

	const Component* GetComponent(Component::Type type) const;
	void AddComponent(Component* component);

	Transform* transform = nullptr;

	void Compute();

	std::string Name;

private:
	std::vector<Component*> components = {};
};