#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform
{
public:
	Transform();
	Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
	Transform(const Transform& other);

	glm::quat GetRotationQuaternion() const;
	glm::vec3 GetForwardVector() const;

	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scale;
};