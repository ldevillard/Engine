#pragma once

#include <vector>

#include "data/Transform.h"
#include "component/Component.h"

class Entity
{
public:
	Entity();
	~Entity();

	const Component* GetComponent(Component::Type type) const;
	void AddComponent(Component component);

	Transform* transform = nullptr;

private:
	std::vector<Component> components = {};
};