#pragma once

#include "data/Transform.h"

class Component
{
public:
	void SetTransform(Transform* tr);

	virtual void Compute() = 0;

protected:
	Transform* transform = nullptr;
};