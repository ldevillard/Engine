#include "component/physics/EditorCollider.h"

#include "component/Transform.h"
#include "data/mesh/Mesh.h"
#include "physics/Physics.h"
#include "physics/RayIntersection.h"
#include "system/editor/Editor.h"
#include "system/entity/Entity.h"

#pragma region Public Methods

EditorCollider::EditorCollider(Entity* e) : boundingBox(), entity(e), bvh()
{
}

EditorCollider::EditorCollider(const EditorCollider& other) : boundingBox(other.boundingBox), entity(other.entity), bvh(other.bvh)
{
}

void EditorCollider::Draw(const Transform& transform)
{
    if (Editor::Get().GetSettings().BoundingBoxGizmo)
	    boundingBox.Draw(transform);
	if (Editor::Get().GetSettings().BVHGizmo)
        bvh.DrawNodes(transform);
}

const BoundingBox& EditorCollider::GetBoundingBox() const
{
	return boundingBox;
}

void EditorCollider::UpdateBoundingBox(const std::vector<Mesh>& meshes)
{
    for (const Mesh& mesh : meshes)
        boundingBox.InsertMesh(mesh);
}

void EditorCollider::BuildBVH(const std::vector<Mesh>& meshes)
{
    bvh.BuildBVH(meshes);
	std::cout << "The BVH of entity: " << entity->Name << " successfully built" << std::endl;
}

bool EditorCollider::IntersectRayBVH(const Ray& ray, RaycastHit& outRaycastHit) const
{
	// transform the ray to the local space of the entity
	glm::vec3 origin = glm::inverse(entity->transform->GetTransformMatrix()) * glm::vec4(ray.origin, 1.0f);
	// transform the direction to the local space of the entity
	glm::vec3 direction = glm::inverse(entity->transform->GetTransformMatrix()) * glm::vec4(ray.direction, 0.0f);

	Ray localRay(origin, direction);

	if (bvh.IntersectRay(localRay, outRaycastHit.hitInfo))
		outRaycastHit.editorCollider = const_cast<EditorCollider*>(this);

	return outRaycastHit.hitInfo.hit;
}

bool EditorCollider::IntersectRayBoundingBox(const Ray& ray, RaycastHit& outRaycastHit) const
{
	// transform the ray to the local space of the entity
	glm::vec3 origin = glm::inverse(entity->transform->GetTransformMatrix()) * glm::vec4(ray.origin, 1.0f);
	// transform the direction to the local space of the entity
	glm::vec3 direction = glm::inverse(entity->transform->GetTransformMatrix()) * glm::vec4(ray.direction, 0.0f);

	Ray localRay(origin, direction);

	if (RayAABoxIntersection(localRay, boundingBox, outRaycastHit.hitInfo))
		outRaycastHit.editorCollider = const_cast<EditorCollider*>(this);

	return outRaycastHit.hitInfo.hit;
}

#pragma endregion