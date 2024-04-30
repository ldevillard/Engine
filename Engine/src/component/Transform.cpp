#include "component/Transform.h"
#include "system/editor/Gizmo.h"

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

void Transform::Compute(Shader* shader) const
{
	shader->Use();

	// binding transform data
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, Position);

	// get the rotation quaternion
	glm::quat rotationQuaternion = GetRotationQuaternion();
	// convert the quaternion to a rotation matrix
	glm::mat4 rotationMatrix = glm::mat4_cast(rotationQuaternion);
	// apply the rotation matrix to the model matrix
	model *= rotationMatrix;

	model = glm::scale(model, Scale);

	shader->SetMat4("model", model);
}

const glm::quat Transform::GetRotationQuaternion() const
{
	return glm::quat(glm::radians(Rotation));
}

const glm::vec3 Transform::GetForwardVector() const 
{
	// rotation matrix from euler angles
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

	// apply the rotation to the forward vector
	glm::vec4 forwardVector = rotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);

	return glm::vec3(forwardVector);
}

const glm::mat4 Transform::GetTransformMatrix() const
{
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), Position);
	glm::mat4 rotationMatrix = glm::mat4_cast(GetRotationQuaternion());
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), Scale);

	return translationMatrix * rotationMatrix * scaleMatrix;
}

const float Transform::GetRadius() const
{
	//return Scale.x * 0.5f;

	// This is a hot fix, because the radius of the sphere is equal to the scale of the object (blender sphere)
	return Scale.x;
}

const Sphere Transform::AsSphere() const
{
	return Sphere(Position, GetRadius());
}

void Transform::SetPosition(const glm::vec3& position)
{
	Position = position;
}

void Transform::SetRotation(const glm::vec3& rotation)
{
	Rotation = rotation;
}

void Transform::SetScale(const glm::vec3& scale)
{
	Scale = scale;
}

#pragma endregion