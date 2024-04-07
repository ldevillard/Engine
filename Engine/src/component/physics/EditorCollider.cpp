#include "component/physics/EditorCollider.h"

#pragma region Public Methods

EditorCollider::EditorCollider(Entity* e) : boundingBox(), entity(e)
{
}

void EditorCollider::ApplyTransform(const Transform& transform)
{
	boundingBox.ApplyTransform(transform);
}

const OBoundingBox& EditorCollider::GetBoundingBox() const
{
	return boundingBox;
}

void EditorCollider::SetBoundingBox(const OBoundingBox& bb)
{
	boundingBox = bb;
}

#pragma endregion