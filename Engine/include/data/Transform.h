#pragma once

#include <maths/glm/glm.hpp>
#include <maths/glm/gtc/quaternion.hpp>

class Transform
{
public:
	Transform();
	Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
	Transform(const Transform& other);

	// maybe store these members and return them as references
	const glm::quat GetRotationQuaternion() const;
	const glm::vec3 GetForwardVector() const;
	const glm::mat4 GetTransformMatrix() const;

	void SetPosition(const glm::vec3& position);
	void SetRotation(const glm::vec3& rotation);
	void SetScale(const glm::vec3& scale);

	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scale;
};