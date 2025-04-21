#pragma once

#include <maths/glm/glm.hpp>
#include <maths/glm/gtc/quaternion.hpp>

#include "data/mesh/MeshData.h"
#include "render/Shader.h"
#include "utils/serializer/json/json.hpp"

class Transform
{
public:
	Transform();
	Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
	Transform(const Transform& other);

	void Compute(Shader* shader) const;

	// maybe store these members and return them as references
	const glm::quat GetRotationQuaternion() const;
	const glm::vec3 GetForwardVector() const;
	const glm::mat4 GetTransformMatrix() const;

	// if the object isn't a sphere, the radius will be the half of x scale
	const float GetRadius() const;
	const Sphere AsSphere() const;

	void SetPosition(const glm::vec3& position);
	void SetRotation(const glm::vec3& rotation);
	void SetScale(const glm::vec3& scale);

	bool HasChanged() const;

	// serialization
	nlohmann::ordered_json Serialize() const;
	void Deserialize(const nlohmann::ordered_json& json);

	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scale;

private:
	glm::vec3 previousPosition;
	glm::vec3 previousRotation;
	glm::vec3 previousScale;
};