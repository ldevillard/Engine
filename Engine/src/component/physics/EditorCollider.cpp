#include "component/physics/EditorCollider.h"

#include "component/Transform.h"
#include "data/mesh/Mesh.h"
#include "physics/Physics.h"
#include "system/entity/Entity.h"

#pragma region Public Methods

EditorCollider::EditorCollider(Entity* e) : boundingBox(), entity(e)
{
}

EditorCollider::EditorCollider(const EditorCollider& other) : boundingBox(other.boundingBox), entity(other.entity)
{
}

void EditorCollider::ApplyTransform(const Transform& transform)
{
	boundingBox.Draw(transform);
}

const BoundingBox& EditorCollider::GetBoundingBox() const
{
	return boundingBox;
}

void EditorCollider::UpdateBoundingBox(const std::vector<Mesh>& meshes)
{
    for (Mesh mesh : meshes)
        boundingBox.InsertMesh(mesh);
}

bool EditorCollider::IntersectRay(const Ray& ray, RaycastHit& outRaycastHit) const
{
	// transform the ray to the local space of the entity
	glm::vec3 origin = glm::inverse(entity->transform->GetTransformMatrix()) * glm::vec4(ray.origin, 1.0f);
	// transform the direction to the local space of the entity
	glm::vec3 direction = glm::inverse(entity->transform->GetTransformMatrix()) * glm::vec4(ray.direction, 0.0f);

	Ray localRay(origin, direction);

	 // check if the ray intersects the bounding box
    // calculate the intersections of the ray with each axis-aligned plane of the bounding box
    glm::vec3 tMin = (boundingBox.Min - localRay.origin) / localRay.direction;
    glm::vec3 tMax = (boundingBox.Max - localRay.origin) / localRay.direction;

    // find the nearest and farthest intersections
    glm::vec3 tNear = glm::min(tMin, tMax);
    glm::vec3 tFar = glm::max(tMin, tMax);

    // find the maximum of the nearest intersections
    float tNearMax = glm::max(glm::max(tNear.x, tNear.y), tNear.z);
    // find the minimum of the farthest intersections
    float tFarMin = glm::min(glm::min(tFar.x, tFar.y), tFar.z);

    // check if the ray intersects the bounding box
    if (tNearMax <= tFarMin && outRaycastHit.distance > tNearMax)
    {
        // ray intersects the bounding box
        // calculate the intersection point
        glm::vec3 intersectionPoint = localRay.origin + localRay.direction * tNearMax;
        outRaycastHit.point = intersectionPoint;
        outRaycastHit.distance = tNearMax;
        outRaycastHit.editorCollider = const_cast<EditorCollider*>(this);
        outRaycastHit.hit = true;
        return true;
    }

    // ray does not intersect the bounding box
    return false;
}

#pragma endregion