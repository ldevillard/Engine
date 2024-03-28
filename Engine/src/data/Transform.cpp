#include "data/Transform.h"
#include "utils/Gizmo.h"

#pragma region Public Methods

Transform::Transform() :
	Position(glm::vec3(0.0f)),
	Rotation(glm::vec3(0.0f)),
	Scale(glm::vec3(1.0f))
{

}

Transform::Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) :
	Position(position),
	Rotation(rotation),
	Scale(scale)
{

}

Transform::Transform(const Transform& other) :
	Position(other.Position),
	Rotation(other.Rotation),
	Scale(other.Scale)
{

}

glm::quat Transform::GetRotationQuaternion() const
{
	return glm::quat(glm::radians(Rotation));
}

glm::vec3 Transform::GetForwardVector() const 
{
	// rotation matrix from euler angles
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

	// apply the rotation to the forward vector
	glm::vec4 forwardVector = rotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);

	return glm::vec3(forwardVector);
}

#pragma endregion