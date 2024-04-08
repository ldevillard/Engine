#pragma once

#include <vector>
#include <string>

#include "component/Component.h"
#include "component/Transform.h"
#include "component/physics/EditorCollider.h"

class Component;
class EditorCollider;

class Entity
{
public:
	Entity(const std::string &name, Shader* sh);
	~Entity();

	template<typename T>
	const T* GetComponent() const;

	template<typename T>
	bool TryGetComponent(T*& outComponent) const;

	void AddComponent(Component* component);
	const std::vector<Component*>& GetComponents() const;
	const EditorCollider* GetEditorCollider() const;
	bool IsSelectedEntity() const;

	Transform* transform = nullptr;

	void Compute();
	void ComputeOutline() const;

	std::string Name;

private:
	EditorCollider* editorCollider = nullptr;
	Shader* shader = nullptr;
	
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