#pragma once

#include "data/Transform.h"

class Component
{
public:
	enum Type
	{
		None_Type,
		Transform_Type,
		Model_Type
	};

	void SetTransform(Transform* tr);
	const Component::Type& GetType() const;

	virtual void Compute() = 0;

protected:
	Transform* transform = nullptr;

private:
	const Component::Type type = Component::Type::None_Type;
};