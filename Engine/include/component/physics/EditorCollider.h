#pragma once

#include <maths/glm/glm.hpp>
#include "data/OBoundingBox.h"
#include "system/entity/Entity.h"
#include "physics/Physics.h"

class Entity;
struct RaycastHit;

class EditorCollider
{
public:
	EditorCollider(Entity* e);
	EditorCollider(const EditorCollider& other);

	void ApplyTransform(const Transform& transform);
	
	const OBoundingBox& GetBoundingBox() const;
	void SetBoundingBox(const OBoundingBox& bb);

	bool IntersectRay(const Ray& ray, RaycastHit& outRaycastHit) const;

	Entity* entity = nullptr;

private:
	OBoundingBox boundingBox;
};