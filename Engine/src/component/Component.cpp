#include "component/Component.h"

#pragma region Public Methods

void Component::SetTransform(Transform*tr)
{
	transform = tr;
}

void Component::SetEntity(Entity* en)
{
	entity = en;
}

void Component::SetShader(Shader* sh)
{
	shader = sh;
}

#pragma endregion