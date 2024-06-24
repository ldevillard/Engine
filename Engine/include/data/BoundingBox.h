#pragma once

#include <maths/glm/glm.hpp>
#include <maths/glm/gtc/matrix_transform.hpp>

#include "data/Color.h"

class Mesh;
class Transform;
struct Triangle;

class BoundingBox
{
public:
	glm::vec3 Min = glm::vec3(0);
	glm::vec3 Max = glm::vec3(0);

	BoundingBox();
	BoundingBox(const glm::vec3& min, const glm::vec3& max);
	BoundingBox(const float& min, const float& max);

	glm::vec3 GetSize() const;
	glm::vec3 GetCenter() const;

	void InsertMesh(const Mesh& mesh);
	void InsertTriangle(const Triangle& triangle);
	void InsertPoint(const glm::vec3& point);

	void Draw(const Transform& transform, const Color& color = Color::Green) const;
	void Draw(const Transform& transform, glm::mat4 rotationMatrix , const Color& color = Color::Green) const;
};