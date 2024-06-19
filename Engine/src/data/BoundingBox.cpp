#include "data/BoundingBox.h"

#include "component/Transform.h"
#include "data/Triangle.h"
#include "system/editor/Gizmo.h"
#include "system/editor/Editor.h"

#pragma region Public Methods

BoundingBox::BoundingBox() :
	Min(glm::vec3(0.5f)),
	Max(glm::vec3(-0.5f)),
	Center(glm::vec3(0.0f))
{
}

BoundingBox::BoundingBox(const glm::vec3& min, const glm::vec3& max) :
	Min(min),
	Max(max),
	Center((min + max) * 0.5f)
{
}

glm::vec3 BoundingBox::GetSize() const
{
    return Max - Min;
}

void BoundingBox::InsertMesh(const Mesh& mesh)
{
    for (const Triangle& triangle : mesh.GetTriangles())
        InsertTriangle(triangle);
}

void BoundingBox::InsertTriangle(const Triangle& triangle)
{
    InsertPoint(triangle.A.Position);
    InsertPoint(triangle.B.Position);
    InsertPoint(triangle.C.Position);
}

void BoundingBox::InsertPoint(const glm::vec3& point)
{
    Min.x = std::min(Min.x, point.x);
    Min.y = std::min(Min.y, point.y);
    Min.z = std::min(Min.z, point.z);

    Max.x = std::max(Max.x, point.x);
    Max.y = std::max(Max.y, point.y);
    Max.z = std::max(Max.z, point.z);

    Center = (Min + Max) * 0.5f;
}

// Apply the transformation to the bounding box and draw it
void BoundingBox::Draw(const Transform& transform, const Color& color) const
{
    Transform tr(transform);

    tr.Scale *= glm::abs(Max - Min) * 0.5f;

    glm::vec3 obbCenter = (Min + Max) * 0.5f;
    obbCenter *= transform.Scale;

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(transform.Rotation.z), glm::vec3(.0f, 0.0f, 1.0f))
                             * glm::rotate(glm::mat4(1.0f), glm::radians(transform.Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f))
                             * glm::rotate(glm::mat4(1.0f), glm::radians(transform.Rotation.x), glm::vec3(1.0f, 0.0f, .0f));

    obbCenter = glm::vec3(rotationMatrix * glm::vec4(obbCenter, 1.0f));

    obbCenter += tr.Position;

    tr.Position = obbCenter;

   if (Editor::Get().GetSettings().BoundingBoxGizmo)
        Gizmo::DrawWireCube(color, tr);
}

#pragma endregion