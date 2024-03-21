#include "component/Component.h"

#pragma region Public Methods

void Component::SetTransform(Transform*tr)
{
	transform = tr;
}

const Component::Type& Component::GetType() const
{
	return type;
}

#pragma endregion