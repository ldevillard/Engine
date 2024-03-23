#pragma once

#include "data/Transform.h"
#include "Shader.h"

class Component
{
public:
	void SetTransform(Transform* tr);
	void SetShader(Shader* sh);

	virtual void Compute() = 0;

protected:
	Transform* transform = nullptr;
	Shader* shader = nullptr;
};