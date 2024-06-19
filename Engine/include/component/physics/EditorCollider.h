#pragma once

#include <maths/glm/glm.hpp>

#include "data/BoundingBox.h"

class Entity;
class Mesh;
struct Ray;
struct RaycastHit;
class Transform;

class EditorCollider
{
public:
	EditorCollider(Entity* e);
	EditorCollider(const EditorCollider& other);

	void ApplyTransform(const Transform& transform);
	
	const BoundingBox& GetBoundingBox() const;
	void UpdateBoundingBox(const std::vector<Mesh>& meshes);

	bool IntersectRay(const Ray& ray, RaycastHit& outRaycastHit) const;

	Entity* entity = nullptr;

private:
	BoundingBox boundingBox;
};