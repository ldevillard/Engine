#pragma once

#include <maths/glm/glm.hpp>
#include "data/OBoundingBox.h"
#include "system/entity/Entity.h"

class Entity;

class EditorCollider
{
public:
	EditorCollider(Entity* e);

	void ApplyTransform(const Transform& transform);
	
	const OBoundingBox& GetBoundingBox() const;
	void SetBoundingBox(const OBoundingBox& bb);

private:
	OBoundingBox boundingBox;
	Entity* entity = nullptr;
};