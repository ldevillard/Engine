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

	template<typename T>
	const T* GetComponent() const;

	template<typename T>
	bool TryGetComponent(T*& outComponent) const;

	void AddComponent(Component* component);

	Transform* transform = nullptr;

	void Compute();

	std::string Name;

private:
	std::vector<Component*> components = {};
};

// template implementation
template<typename T>
const T* Entity::GetComponent() const
{
	auto it = std::find_if(components.begin(), components.end(), [](const Component* comp) {
		return dynamic_cast<const T*>(comp) != nullptr;
		});

	if (it != components.end())
		return dynamic_cast<const T*>(*it);
	else
		return nullptr;
}

template<typename T>
bool Entity::TryGetComponent(T*& outComponent) const
{
	for (const auto& comp : components)
	{
		if (auto castedComponent = dynamic_cast<T*>(comp))
		{
			outComponent = castedComponent;
			return true;
		}
	}
	return false;
}