#include "data/OBoundingBox.h"
#include "system/editor/Gizmo.h"
#include "system/editor/Editor.h"

#pragma region Public Methods

OBoundingBox::OBoundingBox() :
	Min(glm::vec3(0.5f)),
	Max(glm::vec3(-0.5f)),
	Center(glm::vec3(0.0f))
{
}

OBoundingBox::OBoundingBox(const glm::vec3& min, const glm::vec3& max) :
	Min(min),
	Max(max),
	Center((min + max) * 0.5f)
{
}

// Apply the transformation to the bounding box and draw it
void OBoundingBox::ApplyTransform(const Transform& transform)
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

    if (Editor::Get()->GetSettings().BoundingBoxGizmo)
        Gizmo::DrawWireCube(Color::Green, tr);
}

#pragma endregion