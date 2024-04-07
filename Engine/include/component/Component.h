#pragma once

#include "component/Transform.h"
#include "render/Shader.h"
#include "system/entity/Entity.h"

class Entity;

class Component
{
public:
	void SetTransform(Transform* tr);
	void SetEntity(Entity* en);
	void SetShader(Shader* sh);

	virtual void Compute() = 0;

protected:
	Transform* transform = nullptr;
	Entity*	entity		 = nullptr;
	Shader*	shader		 = nullptr;
};